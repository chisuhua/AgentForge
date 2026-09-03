#ifndef AGENTFORGE_HYDRAFORGE_CLIENT_H
#define AGENTFORGE_HYDRAFORGE_CLIENT_H

#include <memory>
#include <string>
#include <vector>

namespace agentforge { class IAgent; }

namespace agentforge {

// HydraForgeClient wraps the HydraForge DSLEngine + IInteractionBus for use
// from the AgentForge TUI shell.
//
// U4 (2026-09-03) scope: minimal client with Agent registry, no DSLEngine
// wiring yet. Future phases may add DSLEngine construction + IInteractionBus
// subscription + LLM provider wiring per `docs/ADR-AF-001-design.md`.
//
// Thread model: Agent registry is protected by a mutex. Run() requests are
// serialized within a single client instance; multi-threaded run() is out
// of scope for U4.
class HydraForgeClient {
public:
    HydraForgeClient();
    ~HydraForgeClient();

    HydraForgeClient(const HydraForgeClient&) = delete;
    HydraForgeClient& operator=(const HydraForgeClient&) = delete;

    // Initialize: register the baseline set of domain agents.
    // Returns true on success, false on configuration failure.
    // U4 baseline: registers `coding_assistant` only. U4 core adds `doc_writer`.
    bool initialize();

    // Session lifecycle (Day 2 stub semantics; U4 baseline keeps compatibility).
    bool start_session(const std::string& user_input);
    void stop_session();
    bool session_active() const;

    // Agent discovery (U4 baseline).
    bool has_agent(const std::string& agent_id) const;
    size_t agent_count() const;
    std::vector<std::string> list_agents() const;

    // Agent execution. Returns a JSON string with top-level `success` bool.
    // Returns an error JSON if the agent is unknown or the client is not
    // initialized.
    std::string run_agent(const std::string& agent_id, const std::string& input);

    // Singleton accessor — AgentForge has exactly one DSLEngine instance.
    static HydraForgeClient& instance();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace agentforge

#endif // AGENTFORGE_HYDRAFORGE_CLIENT_H
