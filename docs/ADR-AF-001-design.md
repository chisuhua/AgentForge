# ADR-AF-001: AgentForge 总体设计

> **状态**: 🟡 **DRAFT** (Sprint 24 Day 1 — 骨架固化, 实施中)
> **创建日期**: 2026-07-15
> **关联设计**: HydraForge `docs/proposals/implementation/agentforge-mvp-blueprint.md`
> **关联 ADR**: HydraForge [ADR-0019 IInteractionBus + TUI Chat MVP](../HydraForge/blob/main/docs/adr/adr-0019-iinteraction-bus-mvp.md) / [ADR-0021 PDK Design](../HydraForge/blob/main/docs/adr/adr-0021-pdk-design.md) / [ADR-0020 Thread Model](../HydraForge/blob/main/docs/adr/adr-0020-thread-model-isolation.md) / [ADR-0031 Execution Policy](../HydraForge/blob/main/docs/adr/adr-0031-execution-policy.md) / [ADR-0043 PDK Tool Naming](../HydraForge/blob/main/docs/adr/adr-0043-pdk-tool-naming-convention.md)

---

## 决策摘要 (5 个)

### 决策 1: TUI 库 = FTXUI v6 vendor

| 选项 | 接受/拒绝 | 理由 |
|------|:---------:|------|
| **FTXUI v6 vendor** | ✅ | header-only；与 HydraForge ADR-0019 §5.3 FTXUI 伪代码设计一致；事件循环天然适配 `IInteractionBus::subscribe()` token 流 |
| raw termios | ❌ | 调试 I/O 时间估算 2.5 周；多 panel 难；窗口尺寸自适应难 |
| Inkwell / ncurses | ❌ | 增加学习成本；与 PDK 风格不匹配 |

**演进路径**: Sprint 27 升级多 Panel 时 FTXUI Renderer 抽象让 panel 添加是 copy-paste + 事件分发 2 行变更；未来 web TUI 替换为 `httplib::Server` 即可。

### 决策 2: Agent Loop = React (`DEFINE_AGENT`)

| 选项 | 接受/拒绝 | 理由 |
|------|:---------:|------|
| **React** | ✅ | 沿用 HydraForge `pdk/g1_coding_assistant/` 2-step ReAct 范式；`SimpleCognitiveOrchestrator` 已有 6/6 测试覆盖；PDK `ReactLoop` Sprint 20 ship |
| PlanExecute | ❌ (MVP) | 5 状态机 + retry 机制对 MVP 过于复杂；Phase 2 升级路径清晰（5 行替换 `LoopType` 模板参数） |
| ForkJoin | ❌ (MVP) | 依赖 `DomainWorkerPool` + 事件订阅同步，需多 agent 协作场景 |

**演进路径**: Sprint 25 升级 `PlanExecute` 仅替换 `AgentLoopType` 模板参数 + 添加规划 prompt 模板；Sprint 28+ 多 agent 协作时加 `ForkJoin`。

### 决策 3: LLM 接入 = `LLMProviderFactory` + C16 Decorator 链

| 选项 | 接受/拒绝 | 理由 |
|------|:---------:|------|
| **`LLMProviderFactory` + Decorator** | ✅ | 复用 HydraForge C16 ship 的 CostTracking + Compliance + RateLimit 装饰器链；用户配置 `OPENAI_API_KEY` env var 即用 |
| 直连 OpenAI HTTP API | ❌ | 绕过 HydraForge ILLMProvider 抽象，浪费 Sprint 17-21 投资 |
| llama.cpp 本地推理 | ❌ (MVP) | 需要 `pdk/llama_engine/` 12 工具全部实现（当前 🟡 stub） |

**演进路径**: Sprint 27 加 `ModelRouter` 装饰器实现多策略路由；Sprint 28+ 加 Claude / Gemini provider 仅 1 个新 ILLMProvider 子类。

### 决策 4: 工具注册 = DECLARE_TOOL + `register_tool_function` 混用

| 选项 | 接受/拒绝 | 理由 |
|------|:---------:|------|
| **DECLARE_TOOL + `register_tool_function` 混用** | ✅ | 简单工具走 `DECLARE_TOOL` 宏（5 行领域逻辑）；需访问 plugin state 的走 `register_tool_function`（参考 HydraForge `g3_knowledge_base` 范式） |
| 仅 DECLARE_TOOL | ❌ | 不能访问 state，未来扩展阻塞 |
| 仅 `register_tool_function` | ❌ | 失去 PDK 宏的 Schema + 权限自动生成能力 |

**演进路径**: Phase 2 用户自定义工具 plugin 仍可二选一；Phase 3+ 工具 manifest 用 g1 范式（spec 声明依赖）。

### 决策 5: 审批流 = ApprovalHandler stdin transport

| 选项 | 接受/拒绝 | 理由 |
|------|:---------:|------|
| **ApprovalHandler stdin transport** | ✅ | HydraForge ADR-0031 §决策 5 已 ship 3 种 transport（stdin/event_bus/test_auto），stdin 模式适合 TUI 阻塞提示；避免实现 ConfirmationDialog（HydraForge ADR-0004 deferred 债） |
| TUI ConfirmationDialog | ❌ (MVP) | 实现成本高（多 panel + 焦点 + Esc 取消） |
| 默认 auto-approve | ❌ | HydraForge ADR-0031 §决策 6 "YOLO 切换需用户确认"，自动批准破坏安全模型 |

**演进路径**: Sprint 28+ 实现 TUI ConfirmationDialog 填 HydraForge ADR-0004 债；ApprovalHandler 切换 transport 零代码。

---

## 详细蓝图

实施 Day-by-Day 计划、风险评估、Sprint 24-29 演进路径见：

👉 **[HydraForge `docs/proposals/implementation/agentforge-mvp-blueprint.md`](https://github.com/chisuhua/HydraForge/blob/main/docs/proposals/implementation/agentforge-mvp-blueprint.md)** (295 行)

---

## 决策日志

| 日期 | 决策 | 依据 |
|------|------|------|
| 2026-07-15 | TUI 库 = FTXUI v6 vendor | HydraForge ADR-0019 §5.3 设计一致 + header-only |
| 2026-07-15 | Agent Loop = React | HydraForge `pdk/g1_coding_assistant/` 范式 |
| 2026-07-15 | LLM = `LLMProviderFactory` + C16 Decorator 链 | 复用 Sprint 21 ship 投资 |
| 2026-07-15 | 工具注册混用 | g1 + g3 双范式覆盖 |
| 2026-07-15 | 审批 = stdin transport | 避免实现 ConfirmationDialog deferred 债 |
| 2026-07-15 | HydraForge Runtime 接入 = `FetchContent` | 绕过无 install rules 鸡生蛋 |
| 2026-07-15 | Repo = 新建 `chisuhua/AgentForge` (非 hydraforge-pdk) | Oracle 验证 + HydraForge ADR-0021 §7 字面判读 |

---

## 状态更新规则

本文档在 Sprint 24 末 (2026-07-29) 升级为 ✅ **Accepted**，届时移除 `DRAFT` 标记 + 添加实施验证段落（CTest/77 基线保护 / 7 工具测试覆盖 / TUI 渲染验证）。

---

## §已知限制 (Day 3 lessons learned)

### 限制 1: HydraForge FetchContent 因 monorepo 内部 `external/taskflow` 缺失而阻塞 (2026-07-15)

**问题**:
```
$ cmake -DAGENTFORGE_FETCH_HYDRAFORGE=ON -B build -S .
CMake Error at build/_deps/hydraforge-src/CMakeLists.txt:46 (add_subdirectory):
  add_subdirectory given source "external/taskflow" which is not an existing
  directory.
```

**根因**: HydraForge monorepo `CMakeLists.txt` 第 46 行引用 `external/taskflow`，但该目录不在 git tree 中（不是 submodule，`.gitmodules` 中也没有 — HydraForge 内部 vendoring 不完整）。

**Oracle B+ STOP 条件触发**: "If FetchContent fails after 2h debugging → commit ADR-AF-001 lessons learned + pivot to alternative path"

**Day 3 实际决策**（<2h 失败，立即 pivot）:
- ❌ **不**等待 HydraForge 修 `external/taskflow`（阻塞 AgentForge 进度）
- ❌ **不**尝试 vendor HydraForge 整个 monorepo（100s MB，超过 AgentForge MVP 合理体积）
- ✅ **采用 Mock-only Day 4 路径**：跳过 FetchContent，使用 HydraForge `examples/agent_simple/` 的 MockLLMProvider 模式作为参考，但**不直接 fetch 整个 Runtime**
- ✅ **建议 HydraForge 侧起草 ADR-AF-001 镜像提案**：推动 Runtime install rules + `find_package(HydraForge ...)` consumer entry（这是 Sprint 24 末决策点要重新评估的硬依赖）

**对 §决策 3 (LLM 接入) 的影响**:
- Day 4 起改用简化 LLM 接入：`HydraForgeMockClient` 复用 `agenticdsl::MockLLMProvider` 模式
- 真实 LLM (OpenAI API) 接入推迟到 HydraForge install rules ship 后 (Sprint 25+)

**演进路径调整**:
| 路径 | 原计划 | Day 3 调整后 |
|------|--------|-------------|
| Day 4 (原本：HydraForgeClient 真实 LLM) | `LLMProviderFactory::create("openai")` + Decorator | 改用 MockLLMProvider 模式 + `DEFINE_AGENT` 端到端验证 |
| Day 5-8 工具 | 7 个 fs/* + shell/exec | 不变（DECLARE_TOOL 不依赖 HydraForge Runtime） |
| 真实 LLM 接入 | Day 9-10 | **推迟到 HydraForge install rules ship 后**（ADR-AF-001 提案触发） |

**对本文档 §决策 1-5 的影响**: 决策 1/2/4/5 不变（FTXUI vendor + React loop + DECLARE_TOOL + stdin transport 均不依赖 HydraForge Runtime）。决策 3 (LLM) 暂时降级为 MockLLMProvider，等 install rules ship 后升级。

**对 §蓝图的影响**: 同步 HydraForge `docs/proposals/implementation/agentforge-mvp-blueprint.md` 标记 Day 4 调整。

---

## §下一决策点

1. **2026-07-29 (Sprint 24 末验收)**: 评估 4 个决策点
   - (a) HydraForge install rules 是否 ship → 决定 Day 9 真实 LLM 是否能继续
   - (b) external/taskflow 是否修复 → 决定 FetchContent 路径是否可用
   - (c) AgentForge Day 4-8 工具开发是否按 Mock-only 路径前进
   - (d) Sprint 25 是否启动第二 domain agent (`sql_assistant` / `doc_writer`)

2. **Sprint 25 末 (2026-08-12)**: 评估 Phase 6 服务化是否重启（per HydraForge plan §六）

3. **HydraForge 侧 ADR-AF-001 提案**: 起草 Runtime install rules + consumer entry 提案（推动 FetchContent 路径解锁）