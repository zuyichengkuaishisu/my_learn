#include "bt_nav2_plugins/is_number_above.hpp"

#include <iostream>

namespace bt_nav2_plugins
{

IsNumberAbove::IsNumberAbove(const std::string & name, const BT::NodeConfiguration & config)
: BT::ConditionNode(name, config)
{
}

BT::PortsList IsNumberAbove::providedPorts()
{
  return {
    BT::InputPort<double>("value", "要比较的数"),
    BT::InputPort<double>("threshold", "阈值，value 大于等于它则 SUCCESS"),
  };
}

BT::NodeStatus IsNumberAbove::tick()
{
  double value = 0.0;
  double threshold = 0.0;
  if (!getInput("value", value) || !getInput("threshold", threshold)) {
    throw BT::RuntimeError("IsNumberAbove 需要 value 和 threshold");
  }
  const bool above = value >= threshold;
  std::cout << "[IsNumberAbove] " << value << " >= " << threshold
            << " -> " << (above ? "SUCCESS" : "FAILURE") << "\n";
  return above ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

}  // namespace bt_nav2_plugins
