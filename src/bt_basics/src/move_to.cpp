#include "bt_basics/move_to.hpp"

#include <iostream>

namespace bt_basics
{

int MoveTo::halt_count = 0;

MoveTo::MoveTo(const std::string & name, const BT::NodeConfiguration & config)
: BT::StatefulActionNode(name, config)
{
}

BT::PortsList MoveTo::providedPorts()
{
  return {
    BT::InputPort<double>("goal", "要走到的进度"),
    BT::InputPort<int>("ticks_needed", 3, "onRunning 至少走这么多拍"),
    BT::InputPort<bool>("stop_at_goal", true, "进度到达 goal 时返回 SUCCESS；false 则继续 RUNNING，交给前面的条件节点收尾"),
    BT::BidirectionalPort<int>("fail_times", "剩余的主动失败次数，每次 onStart 失败减一"),
    BT::BidirectionalPort<double>("progress", "当前进度，写回黑板"),
  };
}

BT::NodeStatus MoveTo::onStart()
{
  ticks_done_ = 0;
  double goal = 0.0;
  if (!getInput("goal", goal)) {
    throw BT::RuntimeError("MoveTo 缺少端口 goal");
  }

  int fail_times = 0;
  if (getInput("fail_times", fail_times) && fail_times > 0) {
    setOutput("fail_times", fail_times - 1);
    std::cout << "[MoveTo] onStart 主动失败，剩余 fail_times=" << (fail_times - 1) << "\n";
    return BT::NodeStatus::FAILURE;
  }

  std::cout << "[MoveTo] onStart goal=" << goal << "\n";
  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus MoveTo::onRunning()
{
  int ticks_needed = 3;
  getInput("ticks_needed", ticks_needed);
  double goal = 0.0;
  getInput("goal", goal);
  bool stop_at_goal = true;
  getInput("stop_at_goal", stop_at_goal);

  double progress = 0.0;
  getInput("progress", progress);
  progress += step_;
  setOutput("progress", progress);
  ++ticks_done_;

  std::cout << "[MoveTo] onRunning progress=" << progress
            << " ticks_done=" << ticks_done_ << "\n";

  const bool reached = progress >= goal && ticks_done_ >= ticks_needed;
  if (reached && stop_at_goal) {
    return BT::NodeStatus::SUCCESS;
  }
  return BT::NodeStatus::RUNNING;
}

void MoveTo::onHalted()
{
  ++halt_count;
  std::cout << "[MoveTo] onHalted\n";
}

}  // namespace bt_basics
