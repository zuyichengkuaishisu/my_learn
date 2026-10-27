cmake_minimum_required(VERSION 3.8)
project(bt_nav2_plugins)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)
find_package(bt_basics REQUIRED)
find_package(nav2_behavior_tree REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)

add_library(bt_nav2_plugins_nodes SHARED
  src/is_number_above.cpp
  src/register_nodes.cpp
)
target_include_directories(bt_nav2_plugins_nodes PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_compile_definitions(bt_nav2_plugins_nodes PRIVATE BT_PLUGIN_EXPORT)
ament_target_dependencies(bt_nav2_plugins_nodes
  behaviortree_cpp_v3
  bt_basics
  pluginlib
  rclcpp
)

# conda 的 lib 里有另一份 libncurses，链接时会盖住系统库，
# libbehaviortree_cpp_v3.so 需要的版本化符号就对不上。系统库放在搜索路径最前。
set(BT_NCURSES_LINK_OPTS "-Wl,-rpath-link,/lib/x86_64-linux-gnu")
set(BT_NCURSES_LIBS /lib/x86_64-linux-gnu/libncurses.so.6 /lib/x86_64-linux-gnu/libtinfo.so.6)

add_executable(bt_runner src/bt_runner.cpp)
ament_target_dependencies(bt_runner
  ament_index_cpp
  behaviortree_cpp_v3
  nav2_behavior_tree
  rclcpp
)
target_link_options(bt_runner BEFORE PRIVATE ${BT_NCURSES_LINK_OPTS})
target_link_libraries(bt_runner ${BT_NCURSES_LIBS})

add_executable(mission_runner src/mission_runner.cpp)
ament_target_dependencies(mission_runner
  ament_index_cpp
  behaviortree_cpp_v3
  nav2_behavior_tree
  rclcpp
)
target_link_options(mission_runner BEFORE PRIVATE ${BT_NCURSES_LINK_OPTS})
target_link_libraries(mission_runner ${BT_NCURSES_LIBS})

install(TARGETS bt_nav2_plugins_nodes
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(TARGETS bt_runner mission_runner DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY trees DESTINATION share/${PROJECT_NAME})
pluginlib_export_plugin_description_file(bt_nav2_plugins plugins.xml)

ament_package()
