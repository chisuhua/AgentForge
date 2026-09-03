#ifndef AGENTFORGE_AGENTS_CODING_ASSISTANT_H
#define AGENTFORGE_AGENTS_CODING_ASSISTANT_H

#include "agentforge/agents/agent_interface.h"

#include <string>

namespace agentforge {

struct CodingRequest {
    std::string task;       // 用户任务描述（如 "review this function"）
    std::string code;       // 待审查代码（可选）
};

struct CodingResult {
    bool success = false;
    std::string summary;     // Mock 输出：固定字符串
    std::string error;
};

class CodingAssistantAgent final : public IAgent {
public:
    static constexpr const char* kAgentId = "coding_assistant";

    CodingResult run(const CodingRequest& request) const;

    // IAgent
    std::string run(const std::string& input) override;
    const char* agent_id() const override { return kAgentId; }
};

}  // namespace agentforge

#endif
