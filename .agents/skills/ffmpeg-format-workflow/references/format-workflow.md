# Format Workflow Reference

## 1. 容器层问题的典型信号

- `Could not find codec parameters`
- `unspecified size`
- stream 个数不对
- codec id 识别错误
- start time / duration 明显异常

## 2. 先查什么

### 2.1 probe

看容器探测是否稳定命中。

### 2.2 stream 参数填充

看 `codecpar` 是否已经拿到 codec id、宽高、采样率等关键参数。

### 2.3 parser 边界

有些格式的宽高要靠 parser 或 decoder header 才能补齐，不要误判为单纯 demux bug。

## 3. 常用命令

```bash
ffprobe -hide_banner INPUT
ffprobe -v error -show_format -show_streams INPUT
ffmpeg -loglevel debug -probesize 5000000 -analyzeduration 0 -i INPUT -frames:v 1 -f null -
```

## 4. 优化方向

- 减少不必要的 probe 开销
- 更早填充关键参数
- 避免把 codec 层错误伪装成 format 层错误
- 对网络流保持更稳的起播行为