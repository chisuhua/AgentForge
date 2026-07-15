# AgentForge

> **TUI programming assistant built on HydraForge PDK**
> **Status**: Sprint 24 Day 1 — bootstrap skeleton (per `docs/ADR-AF-001-design.md`)
> **关联设计**: [`HydraForge/docs/proposals/implementation/agentforge-mvp-blueprint.md`](https://github.com/chisuhua/HydraForge/blob/main/docs/proposals/implementation/agentforge-mvp-blueprint.md)

## 是什么

AgentForge 是 HydraForge 的下游项目 — 基于 HydraForge Plugin Development Kit (PDK) 构建**领域专用 Agent**。本仓库聚焦首个领域：**TUI 编程助手**（harness/TUI-style programming assistant）。

```
┌──────────────────────────────────────────────────┐
│  AgentForge (this repo)                           │
│  ├─ TUI shell (FTXUI v6, vendor)                  │
│  ├─ Coding assistant (DEFINE_AGENT React)          │
│  ├─ 7 fs/* + shell/exec tools (DECLARE_TOOL)       │
│  └─ HydraForgeClient (DSLEngine + InMemoryBus)    │
└──────────────────────────────────────────────────┘
           │ consumes
           ▼
┌──────────────────────────────────────────────────┐
│  HydraForge Runtime + PDK (vendored via           │
│  FetchContent + find_package(hydraforge_pdk))    │
└──────────────────────────────────────────────────┘
```

## 为什么

HydraForge PDK + IInteractionBus + DECLARE_TOOL 已 ship 完 15 个 PDK 测试 + 51/51 ctest 通过，但**没有真实下游消费者**验证 PDK 是否能驱动 TUI 应用。AgentForge 是第一个真实 PDK 消费者，反向验证 HydraForge 设计正确性。

## 当前状态 (2026-07-15)

- ✅ `chisuhua/AgentForge` repo created (public)
- ✅ 蓝图设计固化: HydraForge `docs/proposals/implementation/agentforge-mvp-blueprint.md` (295 行)
- ✅ ADR-AF-001 skeleton (本仓库 `docs/ADR-AF-001-design.md`)
- 🟡 Sprint 24 Day 1-2 进行中（CMake 骨架 + FTXUI vendor + hello world）

## 架构决策摘要

详细见 `docs/ADR-AF-001-design.md` §决策 1-5：

| 维度 | 决策 |
|------|------|
| TUI 库 | FTXUI v6 vendor (`vendor/ftxui/`) |
| Agent Loop | React (`DEFINE_AGENT`, 沿用 HydraForge `pdk/g1_coding_assistant/` 范式) |
| LLM 接入 | `LLMProviderFactory::create("openai")` + C16 Decorator 链 |
| 工具注册 | DECLARE_TOOL + `register_tool_function` 混用 |
| 审批流 | ApprovalHandler stdin transport |

## 快速开始（Day 12 后有效）

```bash
# 当前: Day 1 skeleton build
cmake -B build -S . && cmake --build build
./build/agentforge    # 输出 "AgentForge MVP — Sprint 24 Day 1"

# Sprint 24 Day 3+: 真实启动
export OPENAI_API_KEY=sk-...
./build/agentforge    # TUI 启动, 即可与 LLM 对聊代码
```

## 演进路径

6 Sprint 增量演进（详细见 blueprint §六）：

| Sprint | 加什么 |
|--------|--------|
| 24 | MVP 本体 (TUI + React + 7 工具) ← **当前** |
| 25 | 真实 LLM 接入 + 第 2 个 domain agent |
| 26 | 用户插件加载 (`engine.load_plugin()`) |
| 27 | 多 Panel TUI + ModelRouter plugin |
| 28 | 流式 Token 渲染 + TUI ConfirmationDialog |
| 29+ | Session 持久化升级 + 多 agent 协作 (ForkJoin) |

## License

Apache 2.0 — 与 HydraForge 一致。

## 关联

- HydraForge monorepo: https://github.com/chisuhua/HydraForge
- HydraForge PDK standalone: https://github.com/chisuhua/hydraforge-pdk
- 设计蓝图: https://github.com/chisuhua/HydraForge/blob/main/docs/proposals/implementation/agentforge-mvp-blueprint.md