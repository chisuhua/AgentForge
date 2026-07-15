// HydraForgeClient — minimal interface for HydraForge Runtime + PDK integration
//
// Sprint 24 Day 2: interface only (no implementation yet). Implementation
// arrives in Day 3+ when HydraForge FetchContent is verified + DSLEngine
// construction signature is confirmed.
//
// Design (per ADR-AF-001-design.md §五决策 3):
//   - LLM 接入: LLMProviderFactory::create("openai") + C16 Decorator chain
//   - 工具注册: DECLARE_TOOL (simple) + register_tool_function (stateful)
//   - Agent loop: DEFINE_AGENT(CodingAssistant, React) (per blueprint §四)
//
// Thread model (per ADR-0020):
//   - DSLEngine per-instance owned by main thread
//   - IInteractionBus = std::shared_ptr (atomic cross-thread safe)
//   - Worker threads push tokens; TUI main thread reads via subscription

#ifndef AGENTFORGE_HYDRAFORGE_CLIENT_H
#define AGENTFORGE_HYDRAFORGE_CLIENT_H

#include <memory>
#include <string>

// Forward declarations — keep header light, full HydraForge types stay in .cpp
namespace agenticdsl {
    class DSLEngine;
    class IInteractionBus;
}

namespace agentforge {

// HydraForgeClient wraps the HydraForge DSLEngine + IInteractionBus for use
// from the AgentForge TUI shell. Owns the DSLEngine instance and the bus;
// exposes minimal methods for the TUI to start/stop agent sessions.
//
// Lifecycle:
//   1. HydraForgeClient client;            // construct (no I/O)
//   2. client.initialize();                // load LLM provider, register tools
//   3. client.start_session(user_input);   // kicks off async agent run
//   4. (tokens stream via IInteractionBus subscription in TUI)
//   5. client.stop_session();              // user Ctrl+C or natural end
//
// Sprint 24 Day 3+ responsibilities:
//   - Construct DSLEngine
//   - Build InMemoryBus + inject via DSLEngine::set_interaction_bus()
//   - Wrap LLMProviderFactory::create("openai") with CostTracking+RateLimit decorators
//   - DECLARE_TOOL × 7 (fs/read, fs/write, fs/edit, fs/glob, fs/grep, shell/exec, +1 ext)
//   - DEFINE_AGENT(CodingAssistant, AgentLoopType::React)
//   - Map IInteractionBus::subscribe_events() → TUI event callbacks
class HydraForgeClient {
public:
    HydraForgeClient();
    ~HydraForgeClient();

    HydraForgeClient(const HydraForgeClient&) = delete;
    HydraForgeClient& operator=(const HydraForgeClient&) = delete;

    // Initialize: load LLM provider, register tools, create agent.
    // Returns true on success, false on configuration failure (e.g., missing
    // OPENAI_API_KEY env var).
    bool initialize();

    // Start a new agent session with user_input. Returns false if a session
    // is already running or the engine failed to start.
    bool start_session(const std::string& user_input);

    // Stop the running session (if any). Safe to call when no session is
    // active. Sends a stop_token to the worker thread and joins cleanly.
    void stop_session();

    // Check whether a session is currently active (for TUI status panel).
    bool session_active() const;

    // Singleton accessor — for now AgentForge has exactly one DSLEngine
    // instance. Future: per-session DSLEngine if we add multi-session UI.
    static HydraForgeClient& instance();

private:
    // PIMPL pattern — keep HydraForge headers out of this public header.
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace agentforge

#endif // AGENTFORGE_HYDRAFORGE_CLIENT_H