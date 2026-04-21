# FFmpeg AVS 问题处理手册

本文档给出处理 `D:\github\FFmpeg` 中 AVS 相关问题的高频命令和判断要点。

## 1. 构建命令

优先使用用户现有 MSYS2 环境：

```powershell
D:\msys64\usr\bin\bash.exe -lc 'cd /d/github/FFmpeg/build && export PATH=/d/github/davs2/install/bin:/d/msys64/mingw64/bin:/d/msys64/usr/bin:$PATH && make -j10 ffmpeg.exe ffprobe.exe ffplay.exe'
```

如果只想验证是否能启动：

```powershell
$env:PATH='D:\github\davs2\install\bin;D:\msys64\mingw64\bin;D:\msys64\usr\bin;' + $env:PATH
D:\github\FFmpeg\build\ffplay_g.exe -version
```

## 2. 探测命令

普通 CAVS / AVS+ 都先跑 `ffprobe`：

```powershell
D:\msys64\usr\bin\bash.exe -lc 'cd /d/github/FFmpeg && export PATH=/d/github/davs2/install/bin:/d/msys64/mingw64/bin:/d/msys64/usr/bin:$PATH && ./build/ffprobe.exe -v error -select_streams v:0 -show_entries stream=codec_name,profile,width,height,pix_fmt,r_frame_rate -of default=nw=1 "<input>"'
```

重点看：

- `codec_name=cavs`
- `profile`
- `width`
- `height`

如果这里就出现 `width=0` / `height=0`，优先查序列头解析与 decoder 分流。

## 3. 解码验证命令

用 `null` 验证最稳：

```powershell
D:\msys64\usr\bin\bash.exe -lc 'cd /d/github/FFmpeg && export PATH=/d/github/davs2/install/bin:/d/msys64/mingw64/bin:/d/msys64/usr/bin:$PATH && ./build/ffmpeg.exe -v error -i "<input>" -map 0:v:0 -frames:v 100 -f null -'
```

AVS+ TS 样本建议带大一点的探测参数：

```powershell
D:\msys64\usr\bin\bash.exe -lc 'cd /d/github/FFmpeg && export PATH=/d/github/davs2/install/bin:/d/msys64/mingw64/bin:/d/msys64/usr/bin:$PATH && ./build/ffmpeg.exe -v error -analyzeduration 100M -probesize 100M -i "<input>" -map 0:v:0 -frames:v 500 -f null -'
```

## 4. 抽帧验证命令

不要只信日志，必要时导出 PNG：

```powershell
$env:PATH='D:\github\davs2\install\bin;D:\msys64\mingw64\bin;D:\msys64\usr\bin;' + $env:PATH
& D:\github\FFmpeg\build\ffmpeg.exe -v error -y -i "<input>" -map 0:v:0 -vf "select=eq(n\,50)" -frames:v 1 D:\msys64\tmp\check.png
```

## 5. 典型问题与处理

### 5.1 `Dimensions invalid`

通常说明当前 decoder 没正确拿到 sequence header，常见原因：

- 普通 CAVS 被错误路由到导入版 `libcavs`
- packet 内 profile 还没识别就提前锁定 decoder
- 语法错位导致 sequence header 解析失败

优先方案：

- 做 dispatcher，不要全量替换 `ff_cavs_decoder`
- 等读到 `CAVS_START_CODE` 和 profile 后再锁定 native/lib 路径

### 5.2 `weighted prediction not yet supported`

含义要拆开看：

- 如果只是打印提示，但后续还能出帧，说明主要问题是功能缺失，不一定阻塞播放
- 如果一报这个后面就 `no frame decoded`、`unexpected start code`，说明语法位没有被正确跳过

优先处理顺序：

1. 先补齐 slice header 中的 weighting 相关参数读取或跳过
2. 再补宏块级的 weighting flag 位读取或跳过
3. 最后再看是否需要真正实现加权预测像素补偿

### 5.3 VSCode 调试一运行就崩

优先排查：

- `launch.json` 的 `program` 是否是 `.exe`
- `environment.PATH` 是否包含：
  - `D:\github\davs2\install\bin`
  - `D:\msys64\mingw64\bin`
  - `D:\msys64\usr\bin`
- 是否误以为终端 profile 会自动传给 `cppdbg`

## 6. 建议的提交拆分

### 提交一：解码器逻辑

适合的说明：

- 修复 CAVS 与 AVS+ 解码路径分流
- 避免普通 CAVS 被错误送入 AVS+ decoder
- 补齐 weighted prediction 相关语法跳过，避免解析错位

### 提交二：VSCode 调试环境

适合的说明：

- 修复 VSCode 下 FFmpeg 调试环境
- 显式指定 MSYS2 shell 和 PATH
- 避免 PowerShell 直接启动缺失运行时 DLL

## 7. 回答“这次修改是为了修复什么”时的建议结构

建议按这三点回答：

1. 修复对象
   - 哪个样本、哪类码流
2. 根因
   - decoder 分流错误、语法错位，还是调试环境缺 PATH
3. 当前状态
   - 已恢复播放/解码
   - 是否仍然存在“已解析但未完整实现像素算法”的剩余限制

## 8. 安全与边界

- 不要在技能文档里写用户真实 token、cookie、PAT、密码
- 不要默认把 `build/` 和 `build_msvc/` 纳入提交
- 不要把 `D:\work\nas-player` 和 `D:\github\FFmpeg` 混用
