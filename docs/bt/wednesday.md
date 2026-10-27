# 周三：叶子、端口、黑板

对照这两个头文件：

- `/opt/ros/humble/include/behaviortree_cpp_v3/condition_node.h`
- `/opt/ros/humble/include/behaviortree_cpp_v3/action_node.h` 里的 `SyncActionNode`

条件节点实现 `tick()`，立刻返回 `SUCCESS` 或 `FAILURE`。`SyncActionNode` 也是一拍做完，返回 `RUNNING` 会抛异常。

端口是节点上的名字，黑板是整棵树共享的键值。XML 里写成 `{count}`，表示这个端口连到黑板键 `count`，而不是字面量。

`CheckCounter` 读 `threshold` 和 `count`，`count < threshold` 时 `SUCCESS`。`Increment` 把同一个键加一写回去。两边的 C++ 在 `src/counter_tree.cpp`。

```bash
ros2 run bt_basics counter_tree
```

`trees/counter.xml` 的阈值是 3，`trees/counter_loose.xml` 是 5。可执行文件相同。`Repeat` 的孩子是同步的，所以多次尝试会发生在同一次 `tickRoot()` 里面，直到 `CheckCounter` 返回 `FAILURE`。这和周四要跨好几拍的 `MoveTo` 是两回事：同步节点不把“还没做完”交还给外层循环。

验收：两份 XML 的结束 `count` 分别是 3 和 5。把 `threshold` 改成别的整数再跑，计数跟着变，不用重编 C++。
