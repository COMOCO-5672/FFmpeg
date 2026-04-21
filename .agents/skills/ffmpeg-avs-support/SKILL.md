---
name: ffmpeg-avs-support
description: 处理 FFmpeg fork 中与 CAVS、AVS+、AVS 相关的播放、解码、构建、VSCode 调试和回归问题时使用。适用于 D:\github\FFmpeg 仓库、MSYS2/mingw64 构建环境、ffplay/ffprobe/ffmpeg 验证、cavsdec/libcavs 分流、AVS+ 样本修复、普通 CAVS 回归分析、以及提交说明整理。
---

# FFmpeg AVS Support

本 skill 用于快速处理当前 FFmpeg fork 中 AVS 相关问题，重点不是泛讲标准，而是复用这个仓库已经验证过的环境、命令和修复经验。

## 适用场景

- 用户明确提到 `D:\github\FFmpeg`
- 用户提到 `CAVS`、`AVS+`、`Guangdian`、`JiZhun`、`cavsdec`、`libcavs`
- `ffplay`、`ffprobe`、`ffmpeg` 在 AVS 码流上报错、崩溃、只探测不解码、尺寸为 0
- 需要区分普通 CAVS 回归和 AVS+ 新增支持
- 需要整理这次修改是为了解决什么问题

## 基本规则

- 仓库路径默认使用 `D:\github\FFmpeg`，不要误用 `D:\work\nas-player`
- 用户环境默认走 MSYS2，不要切到另一套构建链
- 构建、运行、验证优先复用现有环境
- 源码文件中的注释统一使用英文，优先使用 ASCII，避免中文和乱码注释进入代码库

## 快速判断流程

1. 先判断是不是环境问题
2. 再判断是不是 codec 分流问题
3. 再判断是语法解析缺失还是像素算法未实现
4. 最后再讨论优化和清理

## 经验规则

- 普通 CAVS 常见 `profile 0x20`
- AVS+ / Guangdian 常见 `profile 0x48`
- 直接全量切到导入版 `libcavs`，容易让旧 CAVS 回归
- 更稳的是 dispatcher：
  - `0x20` -> native `cavsdec`
  - `0x48` -> imported `libcavs`
- `weighted prediction not yet supported` 不等于整个码流都不能解
  - 先保证语法不乱，再决定是否补像素补偿

## 推荐验证顺序

1. `ffprobe` 看 `codec_name/profile/width/height`
2. `ffmpeg -frames:v N -f null -` 看是否稳定解码
3. 导出单帧或多帧 PNG 看视觉结果
4. 新样本和旧样本都做回归

## 何时读取详细参考

需要具体命令、样本验证、日志判读、优化方向时，读取：

- `references/ffmpeg-avs-playbook.md`