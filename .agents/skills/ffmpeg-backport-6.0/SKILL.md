---
name: ffmpeg-backport-6.0
description: 处理当前 D:\github\FFmpeg 的 note_log/6.0 分支维护、上游或第三方 patch 回移植、FFmpeg 6.0 API 兼容判断、以及提交组织时使用。适用于把 FFmpeg 主线或外部 patch 迁回 6.0、分析 API 差异、处理编译错误和拆分维护提交。
---

# FFmpeg Backport 6.0

本 skill 专门服务于当前 FFmpeg 6.0 维护分支，不按主线最新版本默认处理。

## 适用场景

- 从上游 FFmpeg 回移植 patch
- 从第三方项目迁回 codec 或 format patch
- 遇到 6.0 API 差异导致的编译或行为问题
- 需要整理本地维护分支的提交结构

## 基本规则

- 目标是功能迁移，不是把主线代码原样搬过来
- 优先保留 6.0 的结构和接口风格
- 不要为了贴近主线引入大批无关重构
- 先解决正确性，再考虑风格统一

## 推荐流程

1. 明确 patch 来源和目标问题
2. 找到 6.0 中对应文件、结构体和注册入口
3. 先迁移数据结构和注册逻辑
4. 再迁移核心逻辑
5. 最后处理 API 差异、编译和告警
6. 用新样本和旧样本都做验证

## 何时读取详细参考

需要具体的 backport 判断规则、常见 API 差异和提交拆分模板时，读取：

- `references/backport-6.0.md`