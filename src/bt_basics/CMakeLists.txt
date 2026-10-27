cmake_minimum_required(VERSION 3.8)
project(bt_basics)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)

add_library(bt_basics SHARED
  src/move_to.cpp
  src/note.cpp
)
target_include_directories(bt_basics PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(bt_basics behaviortree_cpp_v3)

set(BT_EXECUTABLES hello_sequence counter_tree mini_nav_tree)
foreach(target ${BT_EXECUTABLES})
  add_executable(${target} src/${target}.cpp)
  target_link_libraries(${target} bt_basics)
  ament_target_dependencies(${target} behaviortree_cpp_v3 ament_index_cpp)
  install(TARGETS ${target} DESTINATION lib/${PROJECT_NAME})
endforeach()

install(TARGETS bt_basics
  EXPORT export_bt_basics
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY trees DESTINATION share/${PROJECT_NAME})

ament_export_targets(export_bt_basics HAS_LIBRARY_TARGET)
ament_export_dependencies(behaviortree_cpp_v3)

ament_package()
