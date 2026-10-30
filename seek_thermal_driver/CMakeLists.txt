cmake_minimum_required(VERSION 3.10)
project(seek_thermal_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)

# Manually find the libseek-thermal installed in /usr/local
include_directories(/usr/local/include/seek)
link_directories(/usr/local/lib)

add_executable(seek_thermal_driver src/seek_thermal_driver.cpp)
ament_target_dependencies(seek_thermal_driver rclcpp sensor_msgs cv_bridge OpenCV)
target_link_libraries(seek_thermal_driver seek)

add_executable(seek_thermal_driver_node src/seek_thermal_driver.cpp)
ament_target_dependencies(seek_thermal_driver_node rclcpp sensor_msgs cv_bridge OpenCV)
target_link_libraries(seek_thermal_driver_node seek)

install(
  TARGETS
    seek_thermal_driver
    seek_thermal_driver_node
  DESTINATION
    lib/${PROJECT_NAME}
)

ament_package()
