// HydraForgeClient implementation (U4 baseline 2026-09-03)
//
// U4: register `coding_assistant` agent on initialize(). U4 core adds
// `doc_writer` in a subsequent change. Full DSLEngine + IInteractionBus
// wiring is deferred to a later sprint.

#include "agentforge/hydraforge_client.h"

#include "agentforge/agents/agent_interface.h"
#include "agentforge/agents/coding_assistant.h"
#include "agentforge/agents/doc_writer.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace agentforge {

struct HydraForgeClient::Impl {
    std::mutex lifecycle_mutex_;
    std::atomic<bool> session_active_{false};
    std::atomic<bool> initialized_{false};
    std::unordered_map<std::string, std::unique_ptr<IAgent>> agents_;
};

HydraForgeClient::HydraForgeClient()
    : impl_(std::make_unique<Impl>()) {}

HydraForgeClient::~HydraForgeClient() = default;

bool HydraForgeClient::initialize() {
    std::lock_guard<std::mutex> lock(impl_->lifecycle_mutex_);
    if (impl_->initialized_.load()) {
        return true;
    }

    impl_->agents_.clear();
    impl_->agents_.emplace(
        CodingAssistantAgent::kAgentId,
        std::make_unique<CodingAssistantAgent>()
    );
    impl_->agents_.emplace(
        DocWriterAgent::kAgentId,
        std::make_unique<DocWriterAgent>()
    );
    impl_->initialized_.store(true);
    return true;
}

bool HydraForgeClient::start_session(const std::string& /*user_input*/) {
    if (!impl_->initialized_.load()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(impl_->lifecycle_mutex_);
    impl_->session_active_.store(true);
    return true;
}

void HydraForgeClient::stop_session() {
    if (!impl_) return;
    std::lock_guard<std::mutex> lock(impl_->lifecycle_mutex_);
    impl_->session_active_.store(false);
}

bool HydraForgeClient::session_active() const {
    if (!impl_) return false;
    return impl_->session_active_.load();
}

bool HydraForgeClient::has_agent(const std::string& agent_id) const {
    if (!impl_) return false;
    std::lock_guard<std::mutex> lock(impl_->lifecycle_mutex_);
    return impl_->agents_.count(agent_id) > 0;
}

size_t HydraForgeClient::agent_count() const {
    if (!impl_) return 0;
    std::lock_guard<std::mutex> lock(impl_->lifecycle_mutex_);
    return impl_->agents_.size();
}

std::vector<std::string> HydraForgeClient::list_agents() const {
    std::vector<std::string> ids;
    if (!impl_) return ids;
    std::lock_guard<std::mutex> lock(impl_->lifecycle_mutex_);
    ids.reserve(impl_->agents_.size());
    for (const auto& [id, _] : impl_->agents_) {
        ids.push_back(id);
    }
    return ids;
}

std::string HydraForgeClient::run_agent(const std::string& agent_id, const std::string& input) {
    if (!impl_) return R"({"success":false,"error":"client not initialized"})";
    std::lock_guard<std::mutex> lock(impl_->lifecycle_mutex_);

    auto it = impl_->agents_.find(agent_id);
    if (it == impl_->agents_.end()) {
        return R"({"success":false,"error":"unknown agent"})";
    }

    try {
        return it->second->run(input);
    } catch (...) {
        return R"({"success":false,"error":"agent threw exception"})";
    }
}

HydraForgeClient& HydraForgeClient::instance() {
    static HydraForgeClient client;
    return client;
}

} // namespace agentforge
