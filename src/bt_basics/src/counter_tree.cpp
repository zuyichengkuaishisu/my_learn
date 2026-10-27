#include <iostream>
#include <string>

#include "behaviortree_cpp_v3/bt_factory.h"
#include "bt_basics/demo_runner.hpp"

namespace
{

// count < threshold 时 SUCCESS，到阈值就 FAILURE，Repeat 因此停下来。
class CheckCounter : public BT::ConditionNode
{
public:
  CheckCounter(const std::string & name, const BT::NodeConfiguration & config)
  : BT::ConditionNode(name, config)
  {
  }

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<int>("threshold", "计数到达这里就失败"),
      BT::InputPort<int>("count", "黑板里的当前计数"),
    };
  }

  BT::NodeStatus tick() override
  {
    int threshold = 0;
    int count = 0;
    if (!getInput("threshold", threshold) || !getInput("count", count)) {
      throw BT::RuntimeError("CheckCounter 需要 threshold 和 count");
    }
    const bool under = count < threshold;
    std::cout << "[CheckCounter] count=" << count << " threshold=" << threshold
              << " -> " << (under ? "SUCCESS" : "FAILURE") << "\n";
    return under ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
  }
};

class Increment : public BT::SyncActionNode
{
public:
  Increment(const std::string & name, const BT::NodeConfiguration & config)
  : BT::SyncActionNode(name, config)
  {
  }

  static BT::PortsList providedPorts()
  {
    return {BT::BidirectionalPort<int>("count", "读出后加一，再写回同一个黑板键")};
  }

  BT::NodeStatus tick() override
  {
    int count = 0;
    if (!getInput("count", count)) {
      throw BT::RuntimeError("Increment 需要 count");
    }
    ++count;
    setOutput("count", count);
    std::cout << "[Increment] count=" << count << "\n";
    return BT::NodeStatus::SUCCESS;
  }
};

int runCounter(BT::BehaviorTreeFactory & factory, const std::string & file, int expected_count)
{
  std::cout << "\n=== " << file << " 期望 count=" << expected_count << " ===\n";
  auto blackboard = BT::Blackboard::create();
  blackboard->set("count", 0);
  auto tree = factory.createTreeFromFile(bt_basics::shareTree(file), blackboard);
  const BT::NodeStatus status = bt_basics::tickUntilDone(tree, 20);
  const int count = tree.rootBlackboard()->get<int>("count");
  std::cout << "结束 count=" << count << "\n";

  int failed = bt_basics::expectStatus(file, status, BT::NodeStatus::FAILURE);
  if (count != expected_count) {
    std::cerr << "[失败] " << file << " 期望 count=" << expected_count
              << " 实际 " << count << "\n";
    failed = 1;
  } else {
    std::cout << "[通过] " << file << " count=" << count << "\n";
  }
  return failed;
}

}  // namespace

int main()
{
  BT::BehaviorTreeFactory factory;
  factory.registerNodeType<CheckCounter>("CheckCounter");
  factory.registerNodeType<Increment>("Increment");

  // 两份 XML 只改阈值。C++ 相同，行为跟着 XML 变。
  int failed = 0;
  failed += runCounter(factory, "counter.xml", 3);
  failed += runCounter(factory, "counter_loose.xml", 5);
  if (failed != 0) {
    std::cerr << "counter_tree 有 " << failed << " 项没过\n";
    return 1;
  }
  std::cout << "\ncounter_tree 全部通过\n";
  return 0;
}
