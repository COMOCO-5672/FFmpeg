---
name: ffmpeg-project-map
description: 用于理解 D:\github\FFmpeg 这个基于 FFmpeg 6.0 维护分支的代码结构、目录职责、常见入口、搜索路径，以及在排障或优化时如何快速把问题落到正确模块。适用于需要快速判断问题应看 libavcodec、libavformat、fftools、libavfilter、libavutil 还是本地 .vscode/MSYS2 环境的场景。
---

# FFmpeg Project Map

本 skill 不是泛泛介绍 FFmpeg，而是帮助 AI 在这个仓库里快速回答两个问题：

- 这个问题应该先查哪个模块
- 这次改动应该落在哪一层

## 适用场景

- 用户提到 `D:\github\FFmpeg`
- 需要快速理解 `note_log/6.0` 分支的目录结构和入口
- 需要判断问题更像 demux、decode、filter、fftools 还是环境问题
- 需要给出排查顺序，而不是在全仓库盲搜

## 仓库背景

- 当前维护分支：`note_log/6.0`
- 这不是 FFmpeg 主线 7.x/8.x，优先按 6.0 的 API 和代码组织理解问题
- 如果参考上游 patch，必须额外判断 6.0 的 API、结构体和注册方式是否兼容

## 模块速查

- `libavcodec/`
  - 编解码器、parser、bitstream 语法、DSP、像素重建
  - 典型现象：`no frame decoded`、`Dimensions invalid`、`unexpected start code`
- `libavformat/`
  - 容器、demux/mux、probe、协议、流参数填充
  - 典型现象：探测不到宽高、codec id 不对、流参数缺失、时长异常
- `libavfilter/`
  - 滤镜图和图像处理链路
- `libavutil/`
  - 基础工具、日志、像素格式、公共结构
- `fftools/`
  - `ffmpeg`、`ffplay`、`ffprobe` 命令行程序入口与选项处理
  - 典型现象：命令参数行为不对、ffplay 播放层崩溃、ffprobe 输出异常
- `.vscode/`
  - 本地任务、调试、PATH、shell、launch 配置
- `.agents/skills/`
  - 面向 AI 的仓库知识和工作流约束

## 快速落点规则

- `ffprobe` 结果就不对：先看 `libavformat`，再看 `libavcodec`
- `ffprobe` 正常但 `ffmpeg -f null -` 解不出来：优先看 `libavcodec`
- 只有 `ffplay` 崩溃而 `ffmpeg` 解码正常：先看 `fftools/ffplay*` 和 SDL/线程路径
- 只有 Windows / VSCode / PowerShell 出问题：先看 `.vscode` 和 MSYS2 环境
- 新格式完全不识别：优先判断缺的是 demux/probe 还是 codec 语法支持
- 同一样本以前能播，现在不能播：优先做回归定位，不要先重构

## 搜索策略

1. 先搜错误日志、codec 名、profile 名、起始码常量
2. 再搜注册表、`Makefile`、`allcodecs.c`、wrapper 或 parser
3. 最后才扩散到跨模块联查

优先工具：

- `rg "cavs|AVS|unexpected start code|Dimensions invalid" libavcodec libavformat fftools`
- `rg "AV_CODEC_ID_|codec_tag|probe" libavformat libavcodec`
- `rg "ffplay|opt_|parse_option" fftools`

## 优化定位规则

当用户说“找问题和优化”时，先判断优化属于哪一类：

- 语法解析优化：header、slice、parser、bitreader
- 解码性能优化：DSP、运动补偿、IDCT、缓存访问
- 探测与启动速度优化：probe、analyzeduration、parser 初始化
- 命令层体验优化：fftools 日志、默认行为、错误输出
- 本地开发效率优化：MSYS2、VSCode、构建目标、最小验证命令

## 何时读取详细参考

出现下面需求时再读 `references/project-map.md`：

- 需要更细的“症状到模块”映射
- 需要典型搜索关键词和入口文件
- 需要给出模块级优化清单