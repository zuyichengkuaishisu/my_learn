# 行为树速查（Humble / BT.CPP v3）

## 完成标准

- [x] 能说出 `Sequence`、`Fallback`、`ReactiveSequence`、`ReactiveFallback` 在孩子返回 `RUNNING` 或 `FAILURE` 时怎么走。对应 `ros2 run bt_basics hello_sequence`，反应式控制对应 `mini_nav_tree` 的最后一棵树。
- [x] 能手写 `SyncActionNode` 和 `ConditionNode`，用端口和黑板传数据。对应 `ros2 run bt_basics counter_tree`。
- [x] 能逐行讲清 Nav2 的 `navigate_to_pose_w_replanning_and_recovery.xml`。要点在 [friday_nav2.md](friday_nav2.md)。
- [x] 自己的 pluginlib 插件被加载进树，tick 出预期状态。对应 `ros2 run bt_nav2_plugins bt_runner`：低于阈值 `FAILURE`，高于阈值 `SUCCESS`。

## 四类节点

- 条件：一拍内 `SUCCESS` 或 `FAILURE`。`CheckCounter`、`IsNumberAbove`、`Arrived`
- 动作：同步的 `Increment` / `Note` 一拍做完；跨拍的 `MoveTo` 用 `onStart`、`onRunning`、`onHalted`
- 控制：`Sequence`、`Fallback`、`ReactiveFallback`、Nav2 的 `RecoveryNode` 和 `PipelineSequence`
- 装饰：`Inverter`、`RetryUntilSuccessful`、`Timeout`、`Repeat`，Nav2 里还有 `RateController`

`RUNNING` 表示这一拍没有做完。父节点下一拍会再来。返回 `SUCCESS` 或 `FAILURE` 才算这个节点结束。

## 控制规则

- `Sequence`：孩子 `FAILURE` 或 `RUNNING` 时停下，后面的兄弟本拍不 tick。全成功才成功
- `Fallback`：孩子 `SUCCESS` 或 `RUNNING` 时停下。全失败才失败
- `ReactiveSequence` / `ReactiveFallback`：每一拍从第一个孩子重新 tick。新的结果可以打断后面还在 `RUNNING` 的兄弟
- `RecoveryNode`：只有第一个孩子成功才成功。它失败时跑第二个孩子；第二个成功就再试第一个

## v3 和 v4 最容易混的四处

- 整树往下走用 `tickRoot()`。v4 教程里的 `tickOnce()` 在这套库上没有
- XML 写成 `<root main_tree_to_execute="MainTree">`。`BTCPP_format="4"` 是 v4 文件
- 跨拍叶子继承 `BT::StatefulActionNode`。v4 的 `BT::RosNodeParams` 不属于 Humble 这份包装
- 看 v3 的 XML 用 Groot 1。Groot2 对不上 `main_tree_to_execute` 这种文件，这周不依赖它

依赖写 `behaviortree_cpp_v3`，头文件从 `behaviortree_cpp_v3/bt_factory.h` 进。

## Nav2 默认树一句

1 Hz 重规划加上持续跟线；跟线或规划失败就清代价地图，整段导航再失败就轮转清图、旋转、等待和后退。目标中途变了，恢复动作让路，回到规划。

## 本周没做的

BT.CPP 4、改 `bt_navigator` 源码、把插件装进正在跑的导航器、完整 SLAM。插件的加载方式和导航器相同，tick 放在 `bt_runner` / `mission_runner` 里，是为了失败时只看这一个进程。
