# Project Map Reference

## 1. 先判断是哪一层出问题

### 1.1 容器/探测层问题

常见表现：

- `ffprobe` 看不到正确 `codec_name`
- `width/height` 为空或为 0
- `Could not find codec parameters`
- duration、time base、stream count 异常

优先目录：

- `libavformat/`
- 必要时联查 `libavcodec/*_parser.c`

优先搜索：

- `probe`
- `read_header`
- `read_packet`
- `codecpar`
- `AVSTREAM_PARSE_`

### 1.2 编解码语法层问题

常见表现：

- `unexpected start code`
- `no frame decoded`
- `Dimensions invalid`
- `profile` 识别错误
- 只能探测不能解码

优先目录：

- `libavcodec/`
- `parser`、`wrapper`、`bitstream` 辅助文件

优先搜索：

- 错误日志原文
- profile 常量
- sequence header / picture header 字段名
- `get_bits` / `show_bits` / `skip_bits`

### 1.3 像素重建或算法缺失

常见表现：

- 码流能走下去，但画面错误、花屏、绿屏、块错位
- 日志出现某算法 not yet supported
- 解码成功但视觉结果不对

优先目录：

- `libavcodec/*dsp*`
- motion compensation
- intra prediction
- deblock
- weighted prediction

### 1.4 命令层或播放器层问题

常见表现：

- `ffplay` 崩溃但 `ffmpeg -f null -` 正常
- 参数行为与预期不符
- 选项解析错误
- SDL 初始化、窗口或事件循环异常

优先目录：

- `fftools/ffplay*`
- `fftools/cmdutils*`
- `fftools/opt*`

### 1.5 本地环境问题

常见表现：

- 只在 VSCode 调试失败
- PowerShell 可以复现，MSYS2 正常
- 缺 DLL、PATH 错误、调试器用错 shell

优先目录：

- `.vscode/`
- 本地 PATH / shell 配置

## 2. 常用入口文件

- codec 注册与选择：`libavcodec/allcodecs.c`
- codec 构建开关：`libavcodec/Makefile`
- ffmpeg CLI 入口：`fftools/ffmpeg.c`
- ffplay CLI 入口：`fftools/ffplay.c`
- ffprobe CLI 入口：`fftools/ffprobe.c`
- 常见日志设施：`libavutil/log.*`

## 3. 处理优化需求时的顺序

1. 先给问题分类，不要把“不能播”和“播得慢”混成一个问题
2. 先拿到最小可复现命令
3. 先确认问题在 demux、decode、filter、fftools 还是环境
4. 只在必要模块里搜索和加日志
5. 修改后至少做一条正向验证和一条回归验证

## 4. 典型最小验证命令

```bash
ffprobe -hide_banner INPUT
ffmpeg -v verbose -i INPUT -frames:v 100 -f null -
ffmpeg -v verbose -i INPUT -frames:v 1 out.png
ffplay -v verbose -i INPUT
```

## 5. 回归检查清单

- 新样本好了没有
- 旧样本是否退化
- `ffprobe`、`ffmpeg`、`ffplay` 三者是否一致
- 是否引入仅 Windows 才有的问题
- 是否误把环境问题当成代码问题