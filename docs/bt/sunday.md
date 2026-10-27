# 周日：一棵小任务树

`mission_runner` 同时加载两个插件：

- `bt_nav2_plugins_nodes`：`IsNumberAbove`、`MoveTo`、`Note`
- `nav2_recovery_node_bt_node`：Nav2 自带的 `RecoveryNode`

黑板在 tick 之前写好：`node`、`goal=3`、`progress=0`、`fail_times=1`。树在 `trees/mission.xml`。

```text
ReactiveFallback UntilArrived
  IsNumberAbove Arrived          progress >= goal 就成功
  RecoveryNode MoveRecovery      否则最多恢复 2 次
    SubTreePlus Approach         子树端口 goal / progress / fail_times 接父黑板
      MoveTo                     第一次 onStart 因为 fail_times 失败
    Sequence ClearAndRetry       恢复孩子：打印一行，用 SetInt 把 fail_times 写成整数 0
```

`MoveTo` 的 `stop_at_goal="false"`，走到 3 以后继续 `RUNNING`。下一拍最前面的 `IsNumberAbove` 成功，整棵树 `SUCCESS`。

`fail_times` 一开始以 `int` 放进黑板。`SetBlackboard` 写的是字符串，类型对不上会直接抛异常，所以恢复分支用 `SetInt`。

```bash
ros2 run bt_nav2_plugins mission_runner
```

验收：输出里能看到一次主动失败、一句恢复日志、进度涨到 3，并且最后是 `[通过] 任务树 SUCCESS`。

对照 [cheatsheet.md](cheatsheet.md) 把四条完成标准过一遍。四条都有对应的可执行文件，跑 `scripts/run_bt_week.sh` 时它们会各自打印 `[通过]`。
