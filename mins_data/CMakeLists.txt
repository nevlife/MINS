cmake_minimum_required(VERSION 3.5.1)

# Project name
project(mins_data)

find_package(ament_cmake REQUIRED)
install(DIRECTORY BSplineDataset FloorPlans GroundTruths
        DESTINATION share/${PROJECT_NAME})
ament_package()
