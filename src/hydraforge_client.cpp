// HydraForgeClient implementation (Day 2 stub)
//
// Day 2: minimal skeleton to satisfy the linker for the interface declared
// in include/agentforge/hydraforge_client.h. Full implementation arrives in
// Day 3+ when:
//   - HydraForge FetchContent is wired (cmake -DAGENTFORGE_FETCH_HYDRAFORGE=ON)
//   - DSLEngine construction signature is verified against HydraForge main
//   - LLMProviderFactory + decorator chain is integrated
//   - 7 tools (fs/* + shell/exec) registered via DECLARE_TOOL
//   - DEFINE_AGENT(CodingAssistant, React) instantiated

#include "agentforge/hydraforge_client.h"

#include <atomic>
#include <mutex>

namespace agentforge {

// PIMPL forward declaration — full Impl lands in Day 3+.
struct HydraForgeClient::Impl {
    // Placeholder state for Day 2 linker satisfaction.
    // Day 3+ will hold: unique_ptr<agenticdsl::DSLEngine>,
    //                   shared_ptr<agenticdsl::IInteractionBus>,
    //                   atomic<bool> session_active_.
    std::mutex lifecycle_mutex_;
    std::atomic<bool> session_active_{false};
};

HydraForgeClient::HydraForgeClient()
    : impl_(std::make_unique<Impl>()) {}

HydraForgeClient::~HydraForgeClient() = default;

bool HydraForgeClient::initialize() {
    // Day 2: stub returns false to signal "not yet wired".
    // Day 3+: load OPENAI_API_KEY, build LLMProviderFactory::create("openai"),
    //         wrap with CostTracking+RateLimit decorators, register 7 tools,
    //         instantiate DEFINE_AGENT, attach InMemoryBus.
    return false;
}

bool HydraForgeClient::start_session(const std::string& /*user_input*/) {
    // Day 2: stub. Day 3+: kick off async agent run, return true on success.
    return false;
}

void HydraForgeClient::stop_session() {
    // Day 2: stub. Day 3+: send stop_token to worker, join.
    if (impl_) {
        std::lock_guard<std::mutex> lock(impl_->lifecycle_mutex_);
        impl_->session_active_.store(false);
    }
}

bool HydraForgeClient::session_active() const {
    if (!impl_) return false;
    return impl_->session_active_.load();
}

HydraForgeClient& HydraForgeClient::instance() {
    // Day 2: function-local static gives us a leak-on-exit singleton with
    // thread-safe initialization (C++11 magic statics). Day 3+ may move to
    // a unique_ptr singleton if cleanup ordering becomes important.
    static HydraForgeClient client;
    return client;
}

} // namespace agentforge