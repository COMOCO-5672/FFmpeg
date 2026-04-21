# FFmpeg AVS Playbook

## 1. AVS 问题先按四类拆

### 1.1 环境问题

表现：

- 只在 PowerShell 或 VSCode 崩
- MSYS2 正常
- 缺 DLL

优先处理：

- PATH
- launch.json
- 调试入口

### 1.2 探测问题

表现：

- `Could not find codec parameters`
- `width/height` 不明
- stream 已识别为 `cavs` 但参数不完整

优先处理：

- demux + parser + decoder header 协同

### 1.3 语法错位问题

表现：

- `No sequence header decoded yet`
- `unexpected start code`
- `stc ... is too large`

优先处理：

- bitreader 对齐
- header 跳过逻辑
- parser / wrapper 分流

### 1.4 算法缺失问题

表现：

- `weighted prediction not yet supported`
- 能拿到宽高，但出不来正确画面

优先处理：

- 先判断是否可以只补语法同步
- 再决定是否实现真正的像素补偿

## 2. 固定验证命令

```bash
ffprobe -v error -show_streams -select_streams v:0 INPUT
ffmpeg -v verbose -i INPUT -frames:v 100 -f null -
ffmpeg -v verbose -i INPUT -frames:v 1 out.png
ffplay -v verbose -i INPUT
```

在本仓库 Windows 环境下，优先用 MSYS2 Bash 包一层。

## 3. 修改时的防回归要求

- AVS+ 样本通过
- 旧 CAVS 样本不回归
- `ffprobe` 与 `ffmpeg` 结论一致
- 如果 `ffplay` 单独失败，先查播放器层

## 4. 优化方向

### 4.1 正确性优化

- 完整 profile 分流
- 补齐 header 语法字段
- 避免错误日志导致误导定位

### 4.2 性能优化

- bitreader 热路径
- weighted prediction 实现
- intra prediction / MC 热点

### 4.3 维护性优化

- 分开提交调试环境和 codec 逻辑
- 注释和日志保持英文
- 对样本和限制做清楚说明