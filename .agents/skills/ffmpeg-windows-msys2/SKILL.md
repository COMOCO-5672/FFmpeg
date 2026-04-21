---
name: ffmpeg-windows-msys2
description: 处理 D:\github\FFmpeg 在 Windows、MSYS2/mingw64、VSCode 环境下的构建、运行、调试、PATH/DLL、launch/tasks 配置和 PowerShell 行为差异时使用。适用于 ffmpeg.exe、ffprobe.exe、ffplay.exe 构建，ffplay_g.exe 调试失败，以及本地环境和代码逻辑问题的边界判断。
---

# FFmpeg Windows MSYS2

本 skill 只处理这个仓库的 Windows 本地开发环境，不讨论通用 Linux/macOS 流程。

## 适用场景

- `make -j10 ffmpeg.exe ffprobe.exe ffplay.exe`
- VSCode 调试起不来
- `ffplay_g.exe` 在 PowerShell 下直接退出
- PATH/DLL 缺失
- MSYS2 里正常，VSCode/PowerShell 里失败
- 需要判断问题到底是环境还是代码

## 已知环境

- 仓库：`D:\github\FFmpeg`
- MSYS2 Bash：`D:\msys64\usr\bin\bash.exe`
- 常用构建目录：`D:\github\FFmpeg\build`
- 必要 PATH 前缀：
  - `D:\github\davs2\install\bin`
  - `D:\msys64\mingw64\bin`
  - `D:\msys64\usr\bin`

## 快速判断

- MSYS2 正常，PowerShell 失败：优先查 PATH/DLL
- `ffmpeg.exe`、`ffprobe.exe` 正常，只有 `ffplay_g.exe` 崩：优先查调试环境和 SDL 路径
- VSCode task 正常，launch 崩：优先查 `launch.json` 的 `PATH`、`program`、`cwd`
- 所有入口都失败：先查构建产物和依赖库

## 推荐命令

构建：

```powershell
D:\msys64\usr\bin\bash.exe -lc 'cd /d/github/FFmpeg/build && export PATH=/d/github/davs2/install/bin:/d/msys64/mingw64/bin:/d/msys64/usr/bin:$PATH && make -j10 ffmpeg.exe ffprobe.exe ffplay.exe'
```

运行验证：

```powershell
$env:PATH='D:\github\davs2\install\bin;D:\msys64\mingw64\bin;D:\msys64\usr\bin;' + $env:PATH
D:\github\FFmpeg\build\ffplay_g.exe -version
```

## 调试规则

- `tasks.json` 明确使用 `bash.exe -lc`
- `launch.json` 显式设置 `PATH`
- `program` 必须指向真实 `.exe`
- 不要假设 VSCode 集成终端里的环境会自动传给调试器

## 何时读取详细参考

需要更细的 VSCode 配置模板、DLL 排查命令和常见崩溃路径时，读取：

- `references/windows-msys2.md`