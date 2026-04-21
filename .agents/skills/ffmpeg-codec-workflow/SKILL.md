---
name: ffmpeg-codec-workflow
description: 在 D:\github\FFmpeg 中处理 codec、parser、bitstream、demux/decode 边界、回归和性能优化时使用。适用于新增 codec 支持、修复 decoder 回归、分析 sequence header 和 picture header、定位 parser 与 decoder 分工、以及设计最小验证和回归命令。
---

# FFmpeg Codec Workflow

本 skill 用于把 codec 类问题快速拆成可执行步骤，避免把“探测错误”“语法错位”“像素算法缺失”“ffplay 层问题”混在一起处理。

## 适用场景

- 新增 codec 或 profile 支持
- 旧样本回归
- `Could not find codec parameters`
- `Dimensions invalid`
- `unexpected start code`
- `no frame decoded`
- `not yet supported`
- 需要做 codec 相关优化

## 第一层判断

1. `ffprobe` 是否已经拿到正确 `codec_name/profile/width/height`
2. `ffmpeg -frames:v N -f null -` 是否能稳定解码
3. `ffplay` 是否只是播放层问题
4. 问题是语法没对齐，还是像素算法没实现

## 症状到落点

- 宽高、profile、level 不对
  - 先查 sequence header / picture header
- 起始码错位、持续报 `stc is too large`
  - 先查 bitreader 对齐和字段跳过逻辑
- `not yet supported` 后立即花屏或崩溃
  - 先判断是否需要先把语法读完整，再决定是否补像素算法
- 新增 AVS+ 后旧 CAVS 回归
  - 优先做 dispatcher / profile 分流，不要整体替换
- ffprobe 正常、ffmpeg 正常、ffplay 崩溃
  - codec 不是第一嫌疑，先看 fftools

## 修改原则

- 优先做最小侵入修改
- 优先保住旧样本，不要为了新样本破坏原路径
- 新 codec/profile 支持优先考虑分流，而不是整条替换
- 先补语法解析，再补像素算法
- 注释用英文，源码优先 ASCII

## 标准验证流程

1. `ffprobe` 看流参数
2. `ffmpeg -frames:v N -f null -` 看是否真解码
3. 抽一帧或多帧 PNG 看视觉结果
4. 新旧样本都跑一遍
5. 必要时对 header 字段和 profile 做日志抓取

## 优化工作流

用户提到“优化”时，先分类型：

- 解析正确性优化
- 启动和 probe 速度优化
- 解码性能优化
- 日志和可调试性优化

只有在正确性稳定后，才继续性能优化。

## 何时读取详细参考

需要更细的症状映射、命令模板、性能热点和回归模板时，读取：

- `references/codec-workflow.md`