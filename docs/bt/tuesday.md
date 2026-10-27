# 周二：一次 tick 是什么

`NodeStatus` 有四个：`SUCCESS`、`FAILURE`、`RUNNING`、`IDLE`。自定义节点不要返回 `IDLE`。一次 `tickRoot()` 从根往下走一步。`RUNNING` 的意思是这步还没做完，下一拍从这里继续。

程序：`src/bt_basics`，入口 `hello_sequence`。

```bash
ros2 run bt_basics hello_sequence
```

四棵树，对应四条规则。

1. `Sequence` 从左到右 tick。孩子 `SUCCESS` 才轮到下一个。两个都成功时，根是 `SUCCESS`。见 `trees/hello_sequence.xml`。
2. `Sequence` 遇到 `FAILURE` 或 `RUNNING` 就停。`trees/hello_sequence_fail.xml` 里 `first` 失败后，`second` 不会离开 `IDLE`。
3. `Fallback` 遇到 `SUCCESS` 或 `RUNNING` 就停。`first` 失败时，它会去 tick `second`，第二个成功则根成功。见 `trees/hello_fallback.xml`。
4. `ReactiveSequence` / `ReactiveFallback` 每一拍都从第一个孩子重新开始。周四的缩小导航树用的就是 `ReactiveFallback`。

`trees/hello_running.xml` 把第 2 条里的 `RUNNING` 单独演示出来：`first` 还要再跑两拍时，`second` 保持 `IDLE`。`TickGate` 在 `src/hello_sequence.cpp`，它是后面 `StatefulActionNode` 的缩小版。

验收：终端里四项都是 `[通过]`，并且你能不看代码说出上面四条。
