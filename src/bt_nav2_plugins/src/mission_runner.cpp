#include <chrono>
#include <iostream>
#include <memory>
#include <string>

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "nav2_behavior_tree/behavior_tree_engine.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("mission_runner");

  // MoveTo / IsNumberAbove / Note 在自己的插件里。
  // RecoveryNode 直接用 Nav2 已经安装的插件，加载路径和 bt_navigator 相同。
  nav2_behavior_tree::BehaviorTreeEngine engine(
    {"bt_nav2_plugins_nodes", "nav2_recovery_node_bt_node"});

  auto blackboard = BT::Blackboard::create();
  blackboard->set<rclcpp::Node::SharedPtr>("node", node);
  blackboard->set<double>("goal", 3.0);
  blackboard->set<double>("progress", 0.0);
  blackboard->set<int>("fail_times", 1);

  const auto path =
    ament_index_cpp::get_package_share_directory("bt_nav2_plugins") + "/trees/mission.xml";
  auto tree = engine.createTreeFromFile(path, blackboard);

  int ticks = 0;
  const auto status = engine.run(
    &tree,
    [&]() {
      ++ticks;
      const double progress = blackboard->get<double>("progress");
      std::cout << "tick " << ticks << " progress=" << progress
                << " root=" << BT::toStr(tree.rootNode()->status(), false) << "\n";
    },
    [&]() {return ticks >= 20;},
    std::chrono::milliseconds(20));

  const double progress = blackboard->get<double>("progress");
  int failed = 0;
  if (status != nav2_behavior_tree::BtStatus::SUCCEEDED) {
    std::cerr << "[失败] 任务树应返回 SUCCESS\n";
    failed = 1;
  }
  if (progress < 3.0) {
    std::cerr << "[失败] progress 应到达 goal 3，实际 " << progress << "\n";
    failed = 1;
  }
  if (failed == 0) {
    std::cout << "[通过] 任务树 SUCCESS，progress=" << progress << "\n";
  }

  rclcpp::shutdown();
  return failed == 0 ? 0 : 1;
}
