#ifndef BT_BASICS__DEMO_RUNNER_HPP_
#define BT_BASICS__DEMO_RUNNER_HPP_

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "behaviortree_cpp_v3/loggers/bt_cout_logger.h"

namespace bt_basics
{

inline std::string shareTree(const std::string & filename)
{
  return ament_index_cpp::get_package_share_directory("bt_basics") + "/trees/" + filename;
}

inline void printNodes(const BT::Tree & tree)
{
  for (const auto & node : tree.nodes) {
    std::cout << "  " << node->name() << " [" << node->registrationName() << "] "
              << BT::toStr(node->status(), false) << "\n";
  }
}

// 一直 tick 到根节点不再是 RUNNING，或达到 max_ticks。
// sleep_between 留给 Timeout 这种靠墙钟的装饰器。
inline BT::NodeStatus tickUntilDone(
  BT::Tree & tree,
  int max_ticks,
  std::chrono::milliseconds sleep_between = std::chrono::milliseconds(0))
{
  BT::StdCoutLogger logger(tree);
  BT::NodeStatus status = BT::NodeStatus::IDLE;
  for (int i = 1; i <= max_ticks; ++i) {
    status = tree.tickRoot();
    std::cout << "tick " << i << " root=" << BT::toStr(status, false) << "\n";
    printNodes(tree);
    if (status != BT::NodeStatus::RUNNING) {
      return status;
    }
    if (sleep_between.count() > 0) {
      std::this_thread::sleep_for(sleep_between);
    }
  }
  return status;
}

inline int expectStatus(const std::string & title, BT::NodeStatus actual, BT::NodeStatus expected)
{
  if (actual == expected) {
    std::cout << "[通过] " << title << " -> " << BT::toStr(actual, false) << "\n";
    return 0;
  }
  std::cerr << "[失败] " << title << " 期望 " << BT::toStr(expected, false)
            << " 实际 " << BT::toStr(actual, false) << "\n";
  return 1;
}

}  // namespace bt_basics

#endif  // BT_BASICS__DEMO_RUNNER_HPP_
