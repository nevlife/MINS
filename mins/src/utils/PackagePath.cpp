/*
 * MINS: Efficient and Robust Multisensor-aided Inertial Navigation System
 * Copyright (C) 2019-2026 Woosik Lee
 * Copyright (C) 2019-2026 Guoquan Huang
 * Copyright (C) 2019-2026 MINS Contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

// This is the only translation unit that touches the ROS package-path API,
// keeping ament out of the shared core.
#include "utils/PackagePath.h"
#include <ament_index_cpp/get_package_share_directory.hpp>

std::string mins::get_package_path(const std::string &pkg) {
  return ament_index_cpp::get_package_share_directory(pkg);
}
