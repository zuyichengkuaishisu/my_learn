#ifndef BT_NAV2_PLUGINS__IS_NUMBER_ABOVE_HPP_
#define BT_NAV2_PLUGINS__IS_NUMBER_ABOVE_HPP_

#include <string>

#include "behaviortree_cpp_v3/condition_node.h"

namespace bt_nav2_plugins
{

// 形状对齐 nav2_behavior_tree::IsBatteryLowCondition：tick() + providedPorts()。
// value >= threshold 返回 SUCCESS，否则 FAILURE。
class IsNumberAbove : public BT::ConditionNode
{
public:
  IsNumberAbove(const std::string & name, const BT::NodeConfiguration & config);

  static BT::PortsList providedPorts();

  BT::NodeStatus tick() override;
};

}  // namespace bt_nav2_plugins

#endif  // BT_NAV2_PLUGINS__IS_NUMBER_ABOVE_HPP_
