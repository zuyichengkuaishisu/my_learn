# 周四：跨拍节点、装饰器、子树

Nav2 的动作节点能包住 ROS Action，是因为一拍里只发一次目标或查一次结果，没完成就返回 `RUNNING`。`MoveTo`（`include/bt_basics/move_to.hpp`）按这个节奏写：

- `onStart`：记录目标。`fail_times > 0` 时本拍直接 `FAILURE`，并把次数减一
- `onRunning`：进度加 1
- `onHalted`：被父节点打断时打印一行

```bash
ros2 run bt_basics mini_nav_tree
```

四个例子：

1. `trees/inverter.xml`：`Inverter` 包住 `AlwaysSuccess`，根变成 `FAILURE`。
2. `trees/retry.xml`：`RetryUntilSuccessful` 在孩子 `FAILURE` 时再试。`fail_times` 初始为 2，两次失败后第三次走完，根 `SUCCESS`。
3. `trees/timeout.xml`：`Timeout msec="30"` 在孩子还是 `RUNNING` 时调用 `halt()`，`MoveTo::onHalted` 会打印，根是 `FAILURE`。
4. `trees/approach.xml`：这是周五那棵 Nav2 树的缩小版。

```text
ReactiveFallback
  Arrived                  进度到了就 SUCCESS
  SubTreePlus Approach     否则跑子树里的 MoveTo
```

`SubTreePlus` 上的 `goal="{goal}"` 和 `progress="{progress}"` 把父黑板的键映射进子树。`stop_at_goal="false"` 让 `MoveTo` 走到目标后继续 `RUNNING`，下一拍 `Arrived` 先成功，`ReactiveFallback` 打断 `MoveTo`。所以最后能看到 `onHalted`，同时父黑板里的 `progress` 已经到 3。

验收：四项都是 `[通过]`。能指出“条件在每一拍的最前面被重新问一次”。
