#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "behaviortree_cpp_v3/bt_factory.h"
#include "bt_basics/demo_runner.hpp"

namespace
{

// 前 ticks 拍返回 RUNNING，用来看 Sequence 会不会在兄弟节点还在跑时去 tick 下一个。
class TickGate : public BT::StatefulActionNode
{
public:
  TickGate(const std::string & name, const BT::NodeConfiguration & config)
  : BT::StatefulActionNode(name, config)
  {
  }

  static BT::PortsList providedPorts()
  {
    return {BT::InputPort<int>("ticks", 2, "保持 RUNNING 的拍数")};
  }

  BT::NodeStatus onStart() override
  {
    remaining_ = 2;
    getInput("ticks", remaining_);
    std::cout << "[" << name() << "] onStart remaining=" << remaining_ << "\n";
    if (remaining_ <= 0) {
      return BT::NodeStatus::SUCCESS;
    }
    return BT::NodeStatus::RUNNING;
  }

  BT::NodeStatus onRunning() override
  {
    --remaining_;
    std::cout << "[" << name() << "] onRunning remaining=" << remaining_ << "\n";
    if (remaining_ <= 0) {
      return BT::NodeStatus::SUCCESS;
    }
    return BT::NodeStatus::RUNNING;
  }

  void onHalted() override
  {
    std::cout << "[" << name() << "] onHalted\n";
  }

private:
  int remaining_{0};
};

bool wasTicked(const std::vector<std::string> & names, const std::string & name)
{
  for (const auto & item : names) {
    if (item == name) {
      return true;
    }
  }
  return false;
}

int runOnce(
  BT::BehaviorTreeFactory & factory,
  const std::string & file,
  const std::string & title,
  const std::function<int(BT::NodeStatus, const std::vector<std::string> &)> & check,
  int max_ticks)
{
  std::cout << "\n=== " << title << " ===\n";
  auto tree = factory.createTreeFromFile(bt_basics::shareTree(file));
  std::vector<std::string> ticked;
  std::vector<BT::TreeNode::StatusChangeSubscriber> subs;
  subs.reserve(tree.nodes.size());
  for (const auto & node : tree.nodes) {
    subs.push_back(node->subscribeToStatusChange(
      [&](BT::TimePoint, const BT::TreeNode & n, BT::NodeStatus prev, BT::NodeStatus status) {
        if (prev == BT::NodeStatus::IDLE && status != BT::NodeStatus::IDLE) {
          ticked.push_back(n.name());
        }
      }));
  }

  const BT::NodeStatus status = bt_basics::tickUntilDone(tree, max_ticks);
  std::cout << "本轮离开 IDLE 的节点:";
  for (const auto & name : ticked) {
    std::cout << " " << name;
  }
  std::cout << "\n";
  return check(status, ticked);
}

}  // namespace

int main()
{
  BT::BehaviorTreeFactory factory;
  factory.registerNodeType<TickGate>("TickGate");

  int failed = 0;

  failed += runOnce(
    factory, "hello_sequence.xml", "Sequence：两个立刻成功的动作",
    [](BT::NodeStatus status, const std::vector<std::string> & ticked) {
      int bad = bt_basics::expectStatus("两个成功", status, BT::NodeStatus::SUCCESS);
      if (!wasTicked(ticked, "first") || !wasTicked(ticked, "second")) {
        std::cerr << "[失败] Sequence 成功时两个孩子都应该被 tick\n";
        bad = 1;
      }
      return bad;
    },
    3);

  failed += runOnce(
    factory, "hello_sequence_fail.xml", "Sequence：第一个失败，第二个本拍不 tick",
    [](BT::NodeStatus status, const std::vector<std::string> & ticked) {
      int bad = bt_basics::expectStatus("遇失败即停", status, BT::NodeStatus::FAILURE);
      if (!wasTicked(ticked, "first") || wasTicked(ticked, "second")) {
        std::cerr << "[失败] Sequence 在 first 失败后不应该 tick second\n";
        bad = 1;
      }
      return bad;
    },
    3);

  failed += runOnce(
    factory, "hello_fallback.xml", "Fallback：第一个失败，第二个会被 tick",
    [](BT::NodeStatus status, const std::vector<std::string> & ticked) {
      int bad = bt_basics::expectStatus("换一条路", status, BT::NodeStatus::SUCCESS);
      if (!wasTicked(ticked, "first") || !wasTicked(ticked, "second")) {
        std::cerr << "[失败] Fallback 在 first 失败后应该 tick second\n";
        bad = 1;
      }
      return bad;
    },
    3);

  {
    std::cout << "\n=== Sequence：前一个还在 RUNNING 时不 tick 下一个 ===\n";
    auto tree = factory.createTreeFromFile(bt_basics::shareTree("hello_running.xml"));
    std::vector<std::string> ticked;
    std::vector<BT::TreeNode::StatusChangeSubscriber> subs;
    for (const auto & node : tree.nodes) {
      subs.push_back(node->subscribeToStatusChange(
        [&](BT::TimePoint, const BT::TreeNode & n, BT::NodeStatus prev, BT::NodeStatus status) {
          if (prev == BT::NodeStatus::IDLE && status != BT::NodeStatus::IDLE) {
            ticked.push_back(n.name());
          }
        }));
    }
    const BT::NodeStatus first_tick = tree.tickRoot();
    bt_basics::printNodes(tree);
    if (first_tick != BT::NodeStatus::RUNNING || wasTicked(ticked, "second")) {
      std::cerr << "[失败] 第一拍应该停在 first，second 仍是 IDLE\n";
      failed += 1;
    } else {
      std::cout << "[通过] 第一拍 second 还没被 tick\n";
    }
    const BT::NodeStatus status = bt_basics::tickUntilDone(tree, 6);
    failed += bt_basics::expectStatus("跨拍走完", status, BT::NodeStatus::SUCCESS);
    if (!wasTicked(ticked, "second")) {
      std::cerr << "[失败] first 成功后应该 tick second\n";
      failed += 1;
    }
  }

  if (failed != 0) {
    std::cerr << "hello_sequence 有 " << failed << " 项没过\n";
    return 1;
  }
  std::cout << "\nhello_sequence 全部通过\n";
  return 0;
}
