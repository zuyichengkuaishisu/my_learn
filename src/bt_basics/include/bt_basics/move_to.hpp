#ifndef BT_BASICS__MOVE_TO_HPP_
#define BT_BASICS__MOVE_TO_HPP_

#include "behaviortree_cpp_v3/action_node.h"

namespace bt_basics
{

// 跨拍动作：onStart 只做一次，未完成时 onRunning 每拍前进一步。
// Nav2 的 BtActionNode 也是这个节奏：一拍里发目标或查一次结果，没完就返回 RUNNING。
class MoveTo : public BT::StatefulActionNode
{
public:
  MoveTo(const std::string & name, const BT::NodeConfiguration & config);

  static BT::PortsList providedPorts();

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

  // 给周四、周日的验收用：Timeout 或 ReactiveFallback 打断时会加一。
  static int halt_count;

private:
  int ticks_done_{0};
  double step_{1.0};
};

}  // namespace bt_basics

#endif  // BT_BASICS__MOVE_TO_HPP_
