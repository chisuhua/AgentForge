#include "agentforge/agents/coding_assistant.h"

namespace agentforge {

CodingResult CodingAssistantAgent::run(const CodingRequest& request) const {
    if (request.task.empty()) {
        return CodingResult{
            .success = false,
            .summary = {},
            .error = "Missing arg: task",
        };
    }

    // MockLLM 路径：确定性输出
    // 真实 LLM 仅通过 HydraForge ILLMProvider 抽象调用（不在 U4 范围）
    return CodingResult{
        .success = true,
        .summary = "[coding_assistant mock] reviewed: " + request.task,
        .error = {},
    };
}

std::string CodingAssistantAgent::run(const std::string& input) {
    CodingRequest req{.task = input, .code = {}};
    auto result = run(req);
    // Minimal JSON serialization (U4 baseline: avoid pulling nlohmann_json).
    return std::string("{\"success\":") + (result.success ? "true" : "false") +
           ",\"agent\":\"coding_assistant\",\"summary\":\"" + result.summary +
           "\",\"error\":\"" + result.error + "\"}";
}

}  // namespace agentforge
