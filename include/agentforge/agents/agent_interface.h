#ifndef AGENTFORGE_AGENTS_AGENT_INTERFACE_H
#define AGENTFORGE_AGENTS_AGENT_INTERFACE_H

#include <string>

namespace agentforge {

// IAgent is the minimal abstraction for a domain agent in AgentForge.
// Each agent exposes a JSON-serializable run() that takes an input string
// and returns a JSON output. The abstraction intentionally avoids HydraForge
// PDK types so the baseline does not require DSLEngine/PDK macros.
//
// U4 (2026-09-03): keep this header simple. Future phases may evolve the
// input/output shape to richer value types.
class IAgent {
public:
    virtual ~IAgent() = default;

    // Run the agent on the given input. Returns a JSON string with at least
    // a top-level `success` boolean. Implementations MUST NOT throw.
    virtual std::string run(const std::string& input) = 0;

    // Stable identifier matching `kAgentId` of the concrete implementation.
    virtual const char* agent_id() const = 0;
};

}  // namespace agentforge

#endif
