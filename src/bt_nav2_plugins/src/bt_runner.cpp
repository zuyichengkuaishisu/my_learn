#include <chrono>
#include <iostream>
#include <memory>
#include <string>

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "nav2_behavior_tree/behavior_tree_engine.hpp"
#include "rclcpp/rclcpp.hpp"

namespace
{

const char * toText(nav2_behavior_tree::BtStatus status)
{
  switch (status) {
    case nav2_behavior_tree::BtStatus::SUCCEEDED:
      return "SUCCESS";
    case nav2_behavior_tree::BtStatus::FAILED:
      return "FAILURE";
    case nav2_behavior_tree::BtStatus::CANCELED:
      return "CANCELED";
  }
  return "UNKNOWN";
}

int runFile(
  nav2_behavior_tree::BehaviorTreeEngine & engine,
  const BT::Blackboard::Ptr & blackboard,
  const std::string & file,
  nav2_behavior_tree::BtStatus expected)
{
  const auto path =
    ament_index_cpp::get_package_share_directory("bt_nav2_plugins") + "/trees/" + file;
  std::cout << "\n=== " << file << " ===\n";
  auto tree = engine.createTreeFromFile(path, blackboard);
  int ticks = 0;
  const auto status = engine.run(
    &tree,
    [&]() {
      ++ticks;
      std::cout << "tick " << ticks << " root=" << BT::toStr(tree.rootNode()->status(), false) << "\n";
    },
    [&]() {return ticks >= 5;},
    std::chrono::milliseconds(10));
  if (status == expected) {
    std::cout << "[通过] " << file << " -> " << toText(status) << "\n";
    return 0;
  }
  std::cerr << "[失败] " << file << " 期望 " << toText(expected)
            << " 实际 " << toText(status) << "\n";
  return 1;
}

}  // namespace

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("bt_runner");

  // 和 Nav2 一样：插件名不带 lib 前缀和 .so，引擎内部会拼出来再 dlopen。
  nav2_behavior_tree::BehaviorTreeEngine engine({"bt_nav2_plugins_nodes"});

  auto blackboard = BT::Blackboard::create();
  blackboard->set<rclcpp::Node::SharedPtr>("node", node);

  int failed = 0;
  failed += runFile(engine, blackboard, "number_below.xml", nav2_behavior_tree::BtStatus::FAILED);
  failed += runFile(engine, blackboard, "number_above.xml", nav2_behavior_tree::BtStatus::SUCCEEDED);

  rclcpp::shutdown();
  if (failed != 0) {
    std::cerr << "bt_runner 有 " << failed << " 项没过\n";
    return 1;
  }
  std::cout << "\nbt_runner 全部通过\n";
  return 0;
}
