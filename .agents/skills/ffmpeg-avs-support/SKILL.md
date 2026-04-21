---
name: ffmpeg-avs-support
description: 处理 FFmpeg fork 中与 CAVS、AVS+、AVS 相关的播放、解码、构建、VSCode 调试和回归问题时使用。适用于 D:\github\FFmpeg 仓库、MSYS2/mingw64 构建环境、ffplay/ffprobe/ffmpeg 验证、cavsdec/libcavs 分流、AVS+ 样本修复、普通 CAVS 回归分析、以及提交说明整理。
---

# FFmpeg AVS Support

本技能用于快速处理用户本地 FFmpeg fork 中与 AVS 相关的问题，目标是少走弯路，优先复用已经验证过的环境、命令和定位路径。

## 适用场景

- 用户明确提到 `D:\github\FFmpeg`
- 用户提到 `CAVS`、`AVS+`、`Guangdian`、`JiZhun`、`cavsdec`、`libcavs`
- `ffplay`、`ffprobe`、`ffmpeg` 在 AVS 码流上报错、崩溃、只探测不解码、尺寸为 0
- 需要区分“普通 CAVS 回归”和“AVS+ 新增支持”
- VSCode 调试调用了 PowerShell，导致 `ffplay_g.exe` 因 DLL/PATH 问题直接失败
- 需要整理本次修改是为了解决什么问题，以便 git 提交

## 基本规则

- 仓库路径默认使用 `D:\github\FFmpeg`，不要误用 `D:\work\nas-player`
- 用户这套 FFmpeg 环境默认走 MSYS2，不要改成另一套构建链
- 构建、运行、验证时优先使用用户现有环境，不重新发明一套
- 源码文件中的注释统一使用英文，优先使用 ASCII，避免中文和乱码注释进入代码库
- 先区分问题属于：
  - AVS+ 以前不支持，现在要新增支持
  - 普通 CAVS 以前能探测/能播，新增 AVS+ 后发生回归
  - VSCode/PowerShell 调试环境问题，不是解码器逻辑问题

## 已知有效环境

- FFmpeg 仓库：`D:\github\FFmpeg`
- MSYS2 Bash：`D:\msys64\usr\bin\bash.exe`
- 必要 PATH 前缀：
  - `D:\github\davs2\install\bin`
  - `D:\msys64\mingw64\bin`
  - `D:\msys64\usr\bin`
- 如果从 PowerShell 或 VSCode 直接启动 `build\ffplay_g.exe`，必须确认 PATH 中能找到 `libdavs2-16.dll`

## 快速判断流程

1. 先确认是不是环境问题
   - `ffplay_g.exe -version` 在 PowerShell 里直接退出，优先查 PATH/DLL
   - VSCode `cppdbg` 不会自动继承集成终端 shell，本质上是单独进程环境
2. 再确认是不是码流分流问题
   - 普通 CAVS 样本一般是 `profile 0x20`
   - AVS+ / Guangdian 一般是 `profile 0x48`
3. 再确认是“语法没跟上”还是“像素算法没实现”
   - `Dimensions invalid`、`stc is too large`、`unexpected start code` 通常是语法错位
   - `weighted prediction not yet supported` 往往说明已经读到功能位，但实现还不完整

## 推荐工作流

1. 用 `ffprobe` 看视频流能否稳定探测出 `codec_name/profile/width/height`
2. 用 `ffmpeg -frames:v N -f null -` 做无界面解码验证
3. 必要时导出单帧 PNG 看画面是否正常，而不是只看日志
4. 如果是 AVS+ 支持：
   - 保留导入的 `libcavs` 路径
5. 如果是普通 CAVS 回归：
   - 不要让普通 `0x20` 全部走导入版 `libcavs`
   - 应优先保留原生 `cavsdec`，只让 `0x48` 走 AVS+ 路径
6. 如果原生 `cavsdec` 因某个扩展语法错位：
   - 先补齐语法解析或跳过逻辑
   - 再判断是否必须实现对应像素处理

## 本仓库当前经验结论

- 直接把 `cavs` decoder 全量替换成导入版，会导致普通 CAVS 回归
- 更稳的方式是做一个 dispatcher：
  - `0x20` -> native `cavsdec`
  - `0x48` -> imported `libcavs`
- 普通 CAVS 的某些样本可能带 `slice_weighting_flag`
  - 如果只报“不支持”但不跳过参数，后续语法会错位
  - 先补齐语法跳过，可以恢复播放和解码连续性
  - 但这不等于完整实现了加权预测像素补偿

## VSCode 相关规则

- `terminal.integrated.defaultProfile.windows` 应明确指定 MSYS2
- `tasks.json` 里的 shell 要显式写成 `bash.exe -lc`
- `launch.json` 要给 `PATH`、`MSYSTEM`、`CHERE_INVOKING`
- `program` 要写成带 `.exe` 的实际可执行文件

## 提交建议

- 解码器逻辑修复与 `.vscode` 调试环境修复分开提交
- 不要把 `build/`、`build_msvc/` 提交进去
- 当用户问“这次修改是为了修复什么”时，先回答：
  - 修复了什么样本
  - 原因是分流错误、语法错位，还是调试环境缺 PATH
  - 是否仍有 residual limitation

## 何时读取详细参考

- 需要具体命令、样本验证、VSCode 配置、提交话术时，读取：
  - `references/ffmpeg-avs-playbook.md`
