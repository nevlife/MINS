# MINS
[![ROS2 Jazzy (Ubuntu 24.04)](https://github.com/rpng/MINS/actions/workflows/build_ros2_jazzy.yml/badge.svg)](https://github.com/rpng/MINS/actions/workflows/build_ros2_jazzy.yml)

An efficient, robust, and tightly-coupled **Multisensor-aided Inertial Navigation System (MINS)** which is capable of 
flexibly fusing all five sensing modalities (**IMU**, **wheel** **encoders**, **camera**, **GNSS**, and **LiDAR**) in a filtering 
fashion by overcoming the hurdles of computational complexity, sensor asynchronicity, and intra-sensor calibration. 

Exemplary use case of MINS: 
* VINS (mono, stereo, multi-cam)
* GPS-IMU (single, multiple)
* LiDAR-IMU (single, multiple)
* wheel-IMU
* Camera-GPS-LiDAR-wheel-IMU or more combinations.

![alt text](thirdparty/frames.png)
![alt text](thirdparty/kaist38.gif)

* Publication reference - [https://doi.org/10.1002/rob.22546](https://doi.org/10.1002/rob.22546) (preprint: [arXiv:2309.15390](https://arxiv.org/pdf/2309.15390.pdf))
  
## Key Features
* Inertial(IMU)-based multi-sensor fusion including wheel odometry and arbitrary numbers of cameras, LiDARs, and GNSSs (+ VICON or loop-closure) for localization.
* Online calibration of all onboard sensors (check [exemplary results](https://github.com/rpng/mins/blob/master/mins_eval/ReadMe.md#run-example)).
* Consistent high-order state on manifold interpolation improved from our prior work ([MIMC-VINS](https://ieeexplore.ieee.org/abstract/document/9363450)) and dynamic cloning strategy for light-weight estimation performance.
* Multi-sensor simulation toolbox for IMU, camera, LiDAR, GNSS, and wheel enhanced from our prior work ([OpenVINS](https://github.com/rpng/open_vins))
* Evaluation toolbox for consistency, accuracy, and timing analysis.
* Very detailed [options](https://github.com/rpng/mins/tree/master/mins/src/options) for each sensor enabling general multi-sensor application.

## Dependency
This branch builds only on ROS 2 Jazzy (Ubuntu 24.04). `Dockerfile_jazzy_24_04` lists the dependencies.
`subscribe` also needs flux (`flux_cpp`, `flux_wrapper`) and capa (`capa_ros`).

## Build and Source
```sh
cd <colcon workspace>/src && git clone -b jazzy <this repository> MINS
cd .. && colcon build --symlink-install --packages-up-to mins mins_eval
source install/setup.bash
```

## Run Examples
### Simulation
```sh
ros2 launch mins simulation.launch.py
```

![alt text](thirdparty/simulation.png)

### Real-World Dataset
```sh
ros2 launch mins mins.launch.py config:=euroc_mav
ros2 bag play data/MH_01_easy
```

Convert ROS 1 bags such as [euroc_mav](https://projects.asl.ethz.ch/datasets/doku.php?id=kmavvisualinertialdatasets) with [rosbags](https://pypi.org/project/rosbags/) first.
Raw images come through `flux_wrapper`: from flux when the process has `ENABLE_FLUX=1`, else from DDS, so run MINS without `ENABLE_FLUX` to read a bag played over DDS. Compressed images, IMU, wheel, GNSS and LiDAR stay on DDS.
The node runs on a flux executor with one thread, and when `CAPA_SPEC` names a chain file, capa schedules that thread by the stage of node `/mins`.

![alt text](thirdparty/real_sub.png)

The rosbag files and ground truths used in the evaluation were converted from the [KAIST urban dataset](https://sites.google.com/view/complex-urban-dataset) with [kaist2bag](https://github.com/tsyxyz/kaist2bag). All rights reserved to the KAIST urban dataset.

Mirrors of the converted rosbags (`urban18`-`urban39`) and their ground truths:
* [Hugging Face - kaist-urban-dataset](https://huggingface.co/datasets/gladiator7737/kaist-urban-dataset)
* [UD Robots NAS](https://gofile.me/7B392/kNl7XHqHx) (scan the QR code below)

![KAIST dataset QR code](thirdparty/kaist-QRcode.png)

## Acknowledgements
This project was built on top of the following libraries which are in the thirdparty folder.
* [OpenVINS](https://github.com/rpng/open_vins): Open-source filter-based visual-inertial estimator.
* [ikd-tree](https://github.com/hku-mars/ikd-Tree): Incremental k-d tree.

## Credit / Licensing


This code was written by the [Robot Perception and Navigation Group (RPNG)](https://sites.udel.edu/robot/) at the
University of Delaware. If you have any issues with the code please open an issue on our GitHub page with relevant
implementation details and references. For researchers that have leveraged or compared to this work, please cite the
following:

```bibtex
@article{Lee2025jfr,
    title        = {MINS: Efficient and Robust Multisensor-aided Inertial Navigation System},
    author       = {Woosik Lee and Patrick Geneva and Chuchu Chen and Guoquan Huang},
    year         = 2025,
    journal      = {Journal of Field Robotics},
    volume       = 42,
    number       = 7,
    pages        = {3252--3284},
    doi          = {10.1002/rob.22546},
    url          = {https://github.com/rpng/MINS},
}
```

The codebase and documentation is licensed under the [GNU General Public License v3 (GPL-3)](https://www.gnu.org/licenses/gpl-3.0.txt).
You must preserve the copyright and license notices in your derivative work and make available the complete source code with modifications under the same license ([see this](https://choosealicense.com/licenses/gpl-3.0/); this is not legal advice).
