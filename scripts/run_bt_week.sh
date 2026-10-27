#!/usr/bin/env bash
# 按周二到周日的顺序跑全部验收程序。先 colcon build，再 source install/setup.bash。
set -euo pipefail

if [[ ! -f /opt/ros/humble/setup.bash ]]; then
  echo "找不到 /opt/ros/humble/setup.bash" >&2
  exit 1
fi

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
if [[ ! -f "${ROOT}/install/setup.bash" ]]; then
  echo "还没有 install/setup.bash，先在 ${ROOT} 里 colcon build" >&2
  exit 1
fi

set +u
# shellcheck disable=SC1091
source /opt/ros/humble/setup.bash
# shellcheck disable=SC1091
source "${ROOT}/install/setup.bash"
set -u

ros2 run bt_basics hello_sequence
ros2 run bt_basics counter_tree
ros2 run bt_basics mini_nav_tree
ros2 run bt_nav2_plugins bt_runner
ros2 run bt_nav2_plugins mission_runner

echo
echo "五段程序都通过了。"
