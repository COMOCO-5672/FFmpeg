---
name: ffmpeg-fftools-workflow
description: 在 D:\github\FFmpeg 中处理 fftools 层问题时使用。适用于 ffmpeg、ffprobe、ffplay 的命令解析、日志输出、主循环行为、播放器层崩溃、以及“codec 已正常但 CLI/播放器行为异常”的场景。
---

# FFmpeg FFTools Workflow

本 skill 用于区分“核心库问题”和“fftools 程序层问题”，特别适合排查 ffplay 单独异常、参数解析不符、日志体验差等问题。

## 适用场景

- `ffplay` 崩溃，但 `ffmpeg -f null -` 正常
- `ffprobe` 输出格式或字段异常
- CLI 参数行为不符合预期
- 需要改日志、错误提示、默认行为

## 快速判断

- 只有 `ffplay` 有问题：先看 `fftools/ffplay*`
- 只有 `ffprobe` 输出不对：先看 `fftools/ffprobe*`
- 参数解析不对：看 `cmdutils*`、`opt*`
- 三个程序都一致失败：多半不是 fftools 主问题

## 何时读取详细参考

需要 ffplay/ffprobe/ffmpeg 入口、参数解析和播放器层排障路径时，读取：

- `references/fftools-workflow.md`