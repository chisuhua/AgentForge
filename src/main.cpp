// AgentForge MVP — Sprint 24 Day 2 entry point
//
// Day 2: HydrForgeClient interface introduced; actual implementation
// lands in Day 3+ when HydraForge FetchContent is wired + DSLEngine
// construction signature confirmed against HydraForge main branch.
//
// See:
//   - include/agentforge/hydraforge_client.h (interface)
//   - docs/ADR-AF-001-design.md (5 architectural decisions)
//   - HydraForge docs/proposals/implementation/agentforge-mvp-blueprint.md

#include <iostream>

#include "agentforge/hydraforge_client.h"

int main() {
    std::cout << "AgentForge MVP \u2014 Sprint 24 Day 2\n"
              << "Status: HydraForgeClient interface ready; Day 3+ wiring\n"
              << "Design: docs/ADR-AF-001-design.md\n";

    // Day 2 dry-run: exercise client lifecycle without real LLM/tools.
    // Day 3+: replace with real initialize() that loads LLMProviderFactory.
    auto& client = agentforge::HydraForgeClient::instance();
    (void)client;  // suppress unused warning until Day 3+ wires it up

    return 0;
}