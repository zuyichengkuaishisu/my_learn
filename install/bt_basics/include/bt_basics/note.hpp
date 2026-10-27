#ifndef BT_BASICS__NOTE_HPP_
#define BT_BASICS__NOTE_HPP_

#include "behaviortree_cpp_v3/action_node.h"

namespace bt_basics
{

// 同步叶子：本拍打印一行并返回 SUCCESS。恢复分支用它把“重试”打出来。
class Note : public BT::SyncActionNode
{
public:
  Note(const std::string & name, const BT::NodeConfiguration & config);

  static BT::PortsList providedPorts();

  BT::NodeStatus tick() override;
};

// 把一个 int 写进黑板。类型必须和这个键第一次写入时一致。
class SetInt : public BT::SyncActionNode
{
public:
  SetInt(const std::string & name, const BT::NodeConfiguration & config);

  static BT::PortsList providedPorts();

  BT::NodeStatus tick() override;
};

}  // namespace bt_basics

#endif  // BT_BASICS__NOTE_HPP_
