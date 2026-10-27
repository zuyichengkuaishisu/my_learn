# 一周学完 BehaviorTree.CPP v3

工作空间 `/home/wzy/my_learn`，ROS Humble，系统包 `behaviortree_cpp_v3` 3.8.7。不要另装 BT.CPP 4。

每天 2.5–3 小时。代码已经按天放好，笔记在同目录。建议当天先跑程序，再对着笔记改 XML，最后不看笔记复述规则。

| 日期 | 跑这个 | 笔记 |
| --- | --- | --- |
| 周二 10/27 | `ros2 run bt_basics hello_sequence` | [tuesday.md](tuesday.md) |
| 周三 10/28 | `ros2 run bt_basics counter_tree` | [wednesday.md](wednesday.md) |
| 周四 10/29 | `ros2 run bt_basics mini_nav_tree` | [thursday.md](thursday.md) |
| 周五 10/30 | 只读，不写新逻辑 | [friday_nav2.md](friday_nav2.md) |
| 周六 10/31 | `ros2 run bt_nav2_plugins bt_runner` | [saturday.md](saturday.md) |
| 周日 11/01 | `ros2 run bt_nav2_plugins mission_runner` | [sunday.md](sunday.md)、[cheatsheet.md](cheatsheet.md) |

一次性把五段程序都跑完：

```bash
source /opt/ros/humble/setup.bash
cd /home/wzy/my_learn
colcon build --packages-select bt_basics bt_nav2_plugins
source install/setup.bash
./scripts/run_bt_week.sh
```

版本不要和官网 v4 教程混用：

- 整树 tick 用 `tree.tickRoot()`
- XML 根节点是 `<root main_tree_to_execute="MainTree">`
- 跨拍叶子用 `BT::StatefulActionNode` 的 `onStart` / `onRunning` / `onHalted`
- 依赖名是 `behaviortree_cpp_v3`
