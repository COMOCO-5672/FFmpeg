# Codec Workflow Reference

## 1. 标准排障顺序

### 1.1 先确认是不是 demux 问题

如果 `ffprobe` 连流参数都拿不到，先不要急着改 decoder。

检查：

```bash
ffprobe -hide_banner INPUT
ffmpeg -loglevel debug -i INPUT -frames:v 1 -f null -
```

关注：

- stream codec id 是否正确
- 宽高是否在 demux 阶段就是空的
- parser 是否已经介入

### 1.2 再确认是不是 header 语法问题

重点观察：

- sequence header
- picture header
- extension header
- slice 起始码
- profile / level / progressive / interlace

典型信号：

- `unexpected start code`
- `stc ... is too large`
- `No sequence header decoded yet`

这类问题通常不是“播放器不支持”，而是 decoder 读位已经错位。

### 1.3 再判断是不是像素算法缺失

典型信号：

- 语法能走通
- 宽高正确
- 但报 `weighted prediction not yet supported`
- 或者可以出帧但画面不对

这时要区分：

- 是否可以先跳过权重参数以维持语法同步
- 是否必须实现权重补偿才能保证画面正确

## 2. 常用验证模板

### 2.1 只看探测

```bash
ffprobe -v error -show_streams -select_streams v:0 INPUT
```

### 2.2 只看解码能否持续

```bash
ffmpeg -v verbose -i INPUT -frames:v 100 -f null -
```

### 2.3 抽帧看画面

```bash
ffmpeg -v verbose -i INPUT -frames:v 1 out.png
ffmpeg -v verbose -i INPUT -vf "select=eq(n\,49)" -frames:v 1 frame50.png
```

### 2.4 比较新旧样本

```bash
ffmpeg -v warning -i NEW_SAMPLE -frames:v 100 -f null -
ffmpeg -v warning -i OLD_SAMPLE -frames:v 100 -f null -
```

## 3. 做优化时的重点

### 3.1 正确性优先

没有稳定通过回归前，不做大范围重构。

### 3.2 性能热点优先级

通常先看：

- bitreader 频繁分支
- 运动补偿路径
- IDCT / dequant
- intra prediction
- 边界扩展和缓存访问

### 3.3 优化前后至少比较

- 解码是否仍正确
- 样本是否回归
- 启动是否更快
- 是否只是日志减少而不是实质优化

## 4. 提交组织建议

- 正确性修复一个 commit
- 性能优化一个 commit
- 调试日志或注释清理一个 commit

这样后续回退和 bisect 才有意义。