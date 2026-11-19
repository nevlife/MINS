/*
 * MINS: Efficient and Robust Multisensor-aided Inertial Navigation System
 * Copyright (C) 2019-2026 Woosik Lee
 * Copyright (C) 2019-2026 Guoquan Huang
 * Copyright (C) 2019-2026 MINS Contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <memory>

#include "core/ROS2Publisher.h"
#include "sim/Sim2Visualizer.h"
#include <rclcpp/rclcpp.hpp>
#include "core/SystemManager.h"
#include "options/Options.h"
#include "options/OptionsCamera.h"
#include "options/OptionsEstimator.h"
#include "options/OptionsGPS.h"
#include "options/OptionsIMU.h"
#include "options/OptionsSystem.h"
#include "options/OptionsVicon.h"
#include "options/OptionsWheel.h"
#include "sim/Simulator.h"
#include "update/cam/CamTypes.h"
#include "update/gps/GPSTypes.h"
#include "update/lidar/UpdaterLidar.h"
#include "update/vicon/ViconTypes.h"
#include "update/wheel/WheelTypes.h"
#include "utils/Print_Logger.h"
#include "utils/State_Logger.h"
#include "utils/TimeChecker.h"
#include "utils/colors.h"
#include "utils/opencv_yaml_parse.h"
#include "utils/sensor_data.h"
#include "update/lidar/PointCloud.h"

using namespace mins;
using namespace std;
using namespace Eigen;

shared_ptr<Simulator> sim;
shared_ptr<SystemManager> sys;
shared_ptr<ROS2Publisher> pub;
shared_ptr<Sim2Visualizer> sim_viz;
shared_ptr<Options> op;
shared_ptr<State_Logger> save;

void system_setup(int argc, char **argv);

// Main function
int main(int argc, char **argv) {
  // Load parameters
  system_setup(argc, argv);
  // run simulation
  while (sim->ok() && rclcpp::ok()) {
    // imu: get the next simulated imu measurement if we have it
    ov_core::ImuData imu;
    if (sim->get_next_imu(imu)) {
      bool visualize = sys->feed_measurement_imu(imu);
      pub->publish_imu();
      if (visualize) {
        // If GPS is initialized, make sure visualization is in ENU
        sim->trans_gt_to_ENU = op->est->gps->enabled && sys->up_gps->initialized;
        pub->visualize();
        sim_viz->publish_groundtruth();
        op->sys->save_state ? save->save_state_to_file(sys, sim) : void();
        op->sys->save_trajectory ? save->save_trajectory_to_file(sys) : void();
      }
    }

    // cam: get the next simulated camera uv measurements if we have them
    CamSimData cam;
    if (sim->get_next_cam(cam)) {
      sys->feed_measurement_camsim(cam);
      pub->publish_cam_images(cam.ids, cam.time);
      sim_viz->publish_sim_cam_features();
    }

    GPSData gps;
    if (sim->get_next_gps(gps)) {
      sys->feed_measurement_gps(gps, false);
      pub->publish_gps(gps, false);
    }

    WheelData wheel;
    if (sim->get_next_wheel(wheel)) {
      sys->feed_measurement_wheel(wheel);
    }

    // LIDAR: get the next simulated lidar range measurements
    std::shared_ptr<mins::PointCloud<mins::PointXYZ>> lidar(new mins::PointCloud<mins::PointXYZ>);
    if (sim->get_next_lidar(lidar)) {
      sys->feed_measurement_lidar(lidar);
      pub->publish_lidar_cloud(lidar);
      sim_viz->publish_lidar_structure();
    }

    // vicon: get the next simulated vicon measurement if we have it
    ViconData vicon;
    if (sim->get_next_vicon(vicon)) {
      sys->feed_measurement_vicon(vicon);
      pub->publish_vicon(vicon);
    }
  }

  // Final visualization
  sys->visualize_final();
  sim_viz->visualize_final();
  op->sys->save_timing ? save->save_timing_to_file(sys->tc_sensors->get_total_sum()) : void();
  save->check_files();

  // Release publishers (and the node they hold) while the context is still
  // valid, otherwise their rcl handles are destroyed after shutdown -> segfault.
  pub.reset();
  sim_viz.reset();
  rclcpp::shutdown();
  return EXIT_SUCCESS;
}

void system_setup(int argc, char **argv) {
  // Ensure we have a path, if the user passes it then we should use it
  string config_path = "unset_path_to_config.yaml";
  argc > 1 ? config_path = argv[1] : string();

  // Launch our ros node
  rclcpp::init(argc, argv);

  auto node = std::make_shared<rclcpp::Node>("mins_simulation");

  // Declare so the value can be supplied either via launch parameter or argv[1].
  node->declare_parameter<std::string>("config_path", config_path);
  node->get_parameter<std::string>("config_path", config_path);
  // Load the config
  auto parser = make_shared<ov_core::YamlParser>(config_path);

  parser->set_node(node);
  op = make_shared<Options>();
  op->load_print(parser);
  op->sys->save_prints ? mins::Print_Logger::open_file(op->sys->path_state, true) : void();

  // Create our system
  sim = make_shared<Simulator>(op);
  sys = make_shared<SystemManager>(op->est, sim);

  pub = make_shared<ROS2Publisher>(node, sys, op);
  sim_viz = make_shared<Sim2Visualizer>(node, sys, sim);

  // Ensure we read in all parameters required
  if (!parser->successful()) {
    PRINT4(RED "unable to parse all parameters, please fix\n" RESET);
    exit(EXIT_FAILURE);
  }

  // Create state log files
  save = make_shared<State_Logger>(op, sim);
}
