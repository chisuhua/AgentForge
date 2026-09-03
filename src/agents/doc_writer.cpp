#include "agentforge/agents/doc_writer.h"

namespace agentforge {

namespace {

std::string normalize_format(const std::string& format) {
    return format.empty() ? "markdown" : format;
}

std::string render_markdown(const DocumentRequest& req) {
    std::string out;
    out += "# Generated Document\n\n";
    out += "Request: " + req.request + "\n\n";
    if (!req.context.empty()) {
        out += "## Context\n\n";
        out += req.context + "\n";
    }
    return out;
}

}  // namespace

DocumentResult DocWriterAgent::run(const DocumentRequest& request) const {
    if (request.request.empty()) {
        return DocumentResult{
            .success = false,
            .agent = kAgentId,
            .format = normalize_format(request.format),
            .content = {},
            .error = "Missing arg: request",
        };
    }

    const auto format = normalize_format(request.format);

    // MockLLM 路径：确定性输出
    // 真实 LLM 仅通过 HydraForge ILLMProvider 抽象调用（不在 U4 范围）
    return DocumentResult{
        .success = true,
        .agent = kAgentId,
        .format = format,
        .content = render_markdown(request),
        .error = {},
    };
}

std::string DocWriterAgent::run(const std::string& input) {
    DocumentRequest req{.request = input, .context = {}, .format = {}};
    auto result = run(req);
    // Minimal JSON serialization (U4 baseline: avoid pulling nlohmann_json).
    return std::string("{\"success\":") + (result.success ? "true" : "false") +
           ",\"agent\":\"doc_writer\",\"format\":\"" + result.format +
           "\",\"content\":\"" + result.content + "\",\"error\":\"" +
           result.error + "\"}";
}

}  // namespace agentforge
