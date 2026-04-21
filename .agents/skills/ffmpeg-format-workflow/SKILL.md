---
name: ffmpeg-format-workflow
description: 在 D:\github\FFmpeg 中处理 libavformat、probe、demux、stream 参数、codecpar 填充、TS/MP4/MKV 等容器问题，以及分析“只能探测一部分”“流信息不完整”“解码器拿不到参数”时使用。
---

# FFmpeg Format Workflow

本 skill 用于处理容器、probe 和 demux 侧问题，避免把所有现象都错误地归到 decoder。

## 适用场景

- `Could not find codec parameters`
- `ffprobe` 输出参数缺失
- `codec_name`、`width/height`、时长、stream 信息异常
- TS、MP4、MKV、网络流探测不稳定
- 需要优化 probe 启动速度或参数完整性

## 快速判断

- `ffprobe` 就不对：优先看 `libavformat`
- `ffprobe` 对、`ffmpeg` 错：再看 parser / decoder
- 只有网络流不稳定：额外看协议层和 probe 时机

## 核心关注点

- `read_probe`
- `read_header`
- `read_packet`
- `codecpar`
- stream side data
- parser 是否参与
- `probesize` / `analyzeduration`

## 何时读取详细参考

需要容器层排障模板、probe 参数建议和 demux/decoder 边界判断时，读取：

- `references/format-workflow.md`