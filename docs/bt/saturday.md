# 周六：一个能被引擎加载的插件

插件库 `bt_nav2_plugins_nodes` 里有三样东西，登记都在 `src/register_nodes.cpp` 的 `BT_REGISTER_NODES`：

- `IsNumberAbove`：`value >= threshold` 则 `SUCCESS`。头文件和 `tick()` 对齐 `IsBatteryLowCondition`
- `MoveTo`、`Note`：周四已经写过，编进同一个 `.so`，周日直接加载

`plugins.xml` 是 pluginlib 的描述，`library path` 指向这同一个 so。真正把节点交到工厂的是 `BT_RegisterNodesFromPlugin`。`nav2_behavior_tree::BehaviorTreeEngine` 的构造函数收到库名 `"bt_nav2_plugins_nodes"` 后，会拼成 `libbt_nav2_plugins_nodes.so` 再加载。这和 `bt_navigator` 加载 `nav2_recovery_node_bt_node` 是一条路。插件没有挂进正在运行的导航器，runner 自己 tick，失败时只看这一个进程。

`bt_runner` 先把 `rclcpp::Node::SharedPtr` 放进黑板键 `node`，再分别加载两份 XML：

- `trees/number_below.xml`：`1.0` 对 `5.0`，根 `FAILURE`
- `trees/number_above.xml`：`9.0` 对 `5.0`，根 `SUCCESS`

```bash
ros2 run bt_nav2_plugins bt_runner
```

验收：终端先出现 `FAILURE`，再出现 `SUCCESS`。把 XML 里的数字对调，结果跟着对调，不用改 C++。
