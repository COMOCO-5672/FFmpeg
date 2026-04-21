# FFTools Workflow Reference

## 1. 常见入口

- `fftools/ffmpeg.c`
- `fftools/ffplay.c`
- `fftools/ffprobe.c`
- `fftools/cmdutils.c`
- `fftools/opt_common.c`

## 2. 典型问题分类

### 2.1 ffplay 独有问题

- SDL 初始化
- 渲染线程 / 事件循环
- 播放器状态机
- 命令层参数与解码线程交互

### 2.2 ffprobe 独有问题

- 输出字段组织
- show_entries / show_streams / show_format
- probe 结果展示而非底层探测本身

### 2.3 ffmpeg 独有问题

- 转码参数解析
- 输出链路
- filtergraph 初始化

## 3. 先确认库层是否正常

如果 `ffmpeg -f null -` 和 `ffprobe` 都正常，`ffplay` 崩溃通常不应先改 decoder。

## 4. 适合做的优化

- 错误提示更清楚
- 参数冲突提示更早
- 关键状态日志更容易读
- 调试配置更稳定