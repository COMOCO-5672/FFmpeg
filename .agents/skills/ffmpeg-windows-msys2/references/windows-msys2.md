# Windows MSYS2 Reference

## 1. 最小构建命令

```bash
cd /d/github/FFmpeg/build
export PATH=/d/github/davs2/install/bin:/d/msys64/mingw64/bin:/d/msys64/usr/bin:$PATH
make -j10 ffmpeg.exe ffprobe.exe ffplay.exe
```

## 2. 最小运行检查

先检查依赖：

```bash
ldd ./ffplay.exe
ldd ./ffprobe.exe
```

在 PowerShell 里检查：

```powershell
$env:PATH='D:\github\davs2\install\bin;D:\msys64\mingw64\bin;D:\msys64\usr\bin;' + $env:PATH
D:\github\FFmpeg\build\ffplay_g.exe -version
```

## 3. VSCode 常见错误

### 3.1 只在 launch 崩

通常原因：

- PATH 不完整
- `program` 指向错文件
- `cwd` 不对
- 使用了 PowerShell 默认环境，而不是 MSYS2 运行时环境

### 3.2 只在 ffplay 崩

优先排查：

- SDL2 DLL
- codec 依赖 DLL
- PATH 顺序
- 调试版和发布版二进制是否混用

## 4. 环境问题和代码问题如何切分

如果下面三个都一致失败，再考虑代码：

- MSYS2 直接跑
- PowerShell 补 PATH 后跑
- VSCode launch 跑

如果只有某一种入口失败，优先查环境。

## 5. 推荐提交拆分

- `.vscode` 配置单独提交
- 代码逻辑修复单独提交
- 依赖或构建脚本调整单独提交