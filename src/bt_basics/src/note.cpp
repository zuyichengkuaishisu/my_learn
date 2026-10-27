#include "bt_basics/note.hpp"

#include <iostream>
#include <string>

namespace bt_basics
{

Note::Note(const std::string & name, const BT::NodeConfiguration & config)
: BT::SyncActionNode(name, config)
{
}

BT::PortsList Note::providedPorts()
{
  return {BT::InputPort<std::string>("text", "要打印的一句话")};
}

BT::NodeStatus Note::tick()
{
  std::string text;
  getInput("text", text);
  std::cout << "[Note] " << text << "\n";
  return BT::NodeStatus::SUCCESS;
}

SetInt::SetInt(const std::string & name, const BT::NodeConfiguration & config)
: BT::SyncActionNode(name, config)
{
}

BT::PortsList SetInt::providedPorts()
{
  return {
    BT::InputPort<int>("value", "要写入的整数"),
    BT::OutputPort<int>("output", "写到这个黑板键，类型保持 int"),
  };
}

BT::NodeStatus SetInt::tick()
{
  int value = 0;
  if (!getInput("value", value)) {
    throw BT::RuntimeError("SetInt 需要 value");
  }
  if (!setOutput("output", value)) {
    throw BT::RuntimeError("SetInt 写 output 失败");
  }
  std::cout << "[SetInt] output=" << value << "\n";
  return BT::NodeStatus::SUCCESS;
}

}  // namespace bt_basics
