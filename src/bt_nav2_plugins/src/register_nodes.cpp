#include "behaviortree_cpp_v3/bt_factory.h"
#include "bt_basics/move_to.hpp"
#include "bt_basics/note.hpp"
#include "bt_nav2_plugins/is_number_above.hpp"

BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<bt_nav2_plugins::IsNumberAbove>("IsNumberAbove");
  factory.registerNodeType<bt_basics::MoveTo>("MoveTo");
  factory.registerNodeType<bt_basics::Note>("Note");
  factory.registerNodeType<bt_basics::SetInt>("SetInt");
}
