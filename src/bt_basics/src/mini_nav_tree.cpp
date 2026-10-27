#include <iostream>

#include "behaviortree_cpp_v3/bt_factory.h"
#include "bt_basics/demo_runner.hpp"
#include "bt_basics/move_to.hpp"

namespace
{

class Arrived : public BT::ConditionNode
{
public:
  Arrived(const std::string & name, const BT::NodeConfiguration & config)
  : BT::ConditionNode(name, config)
  {
  }

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<double>("progress"),
      BT::InputPort<double>("goal"),
    };
  }

  BT::NodeStatus tick() override
  {
    double progress = 0.0;
    double goal = 0.0;
    if (!getInput("progress", progress) || !getInput("goal", goal)) {
      throw BT::RuntimeError("Arrived 需要 progress 和 goal");
    }
    const bool arrived = progress >= goal;
    std::cout << "[Arrived] progress=" << progress << " goal=" << goal
              << " -> " << (arrived ? "SUCCESS" : "FAILURE") << "\n";
    return arrived ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
  }
};

BT::Blackboard::Ptr boardWith(double goal, double progress, int fail_times)
{
  auto blackboard = BT::Blackboard::create();
  blackboard->set("goal", goal);
  blackboard->set("progress", progress);
  blackboard->set("fail_times", fail_times);
  return blackboard;
}

}  // namespace

int main()
{
  BT::BehaviorTreeFactory factory;
  factory.registerNodeType<bt_basics::MoveTo>("MoveTo");
  factory.registerNodeType<Arrived>("Arrived");

  int failed = 0;

  {
    std::cout << "\n=== Inverter：把 SUCCESS 翻成 FAILURE ===\n";
    auto tree = factory.createTreeFromFile(bt_basics::shareTree("inverter.xml"));
    const auto status = bt_basics::tickUntilDone(tree, 3);
    failed += bt_basics::expectStatus("Inverter", status, BT::NodeStatus::FAILURE);
  }

  {
    std::cout << "\n=== RetryUntilSuccessful：失败两次后再成功 ===\n";
    auto tree = factory.createTreeFromFile(
      bt_basics::shareTree("retry.xml"), boardWith(1.0, 0.0, 2));
    const auto status = bt_basics::tickUntilDone(tree, 8);
    failed += bt_basics::expectStatus("RetryUntilSuccessful", status, BT::NodeStatus::SUCCESS);
    const int left = tree.rootBlackboard()->get<int>("fail_times");
    if (left != 0) {
      std::cerr << "[失败] fail_times 应该被消耗完，实际 " << left << "\n";
      failed += 1;
    }
  }

  {
    std::cout << "\n=== Timeout：孩子还在 RUNNING 时被 halt ===\n";
    bt_basics::MoveTo::halt_count = 0;
    auto tree = factory.createTreeFromFile(
      bt_basics::shareTree("timeout.xml"), boardWith(100.0, 0.0, 0));
    const auto status = bt_basics::tickUntilDone(tree, 6, std::chrono::milliseconds(80));
    failed += bt_basics::expectStatus("Timeout", status, BT::NodeStatus::FAILURE);
    if (bt_basics::MoveTo::halt_count < 1) {
      std::cerr << "[失败] Timeout 应该调用 MoveTo::onHalted\n";
      failed += 1;
    } else {
      std::cout << "[通过] onHalted 被调用了 " << bt_basics::MoveTo::halt_count << " 次\n";
    }
  }

  {
    std::cout << "\n=== ReactiveFallback + 子树：到了就成功，并打断 MoveTo ===\n";
    bt_basics::MoveTo::halt_count = 0;
    auto tree = factory.createTreeFromFile(
      bt_basics::shareTree("approach.xml"), boardWith(3.0, 0.0, 0));
    const auto status = bt_basics::tickUntilDone(tree, 12);
    failed += bt_basics::expectStatus("缩小导航树", status, BT::NodeStatus::SUCCESS);
    const double progress = tree.rootBlackboard()->get<double>("progress");
    if (progress < 3.0) {
      std::cerr << "[失败] 子树没有把 progress 写回父黑板，实际 " << progress << "\n";
      failed += 1;
    }
    if (bt_basics::MoveTo::halt_count < 1) {
      std::cerr << "[失败] 条件成功后应该 halt 还在跑的 MoveTo\n";
      failed += 1;
    } else {
      std::cout << "[通过] progress=" << progress << " 并且 MoveTo 被打断\n";
    }
  }

  if (failed != 0) {
    std::cerr << "mini_nav_tree 有 " << failed << " 项没过\n";
    return 1;
  }
  std::cout << "\nmini_nav_tree 全部通过\n";
  return 0;
}
