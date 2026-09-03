#ifndef AGENTFORGE_AGENTS_DOC_WRITER_H
#define AGENTFORGE_AGENTS_DOC_WRITER_H

#include "agentforge/agents/agent_interface.h"

#include <string>

namespace agentforge {

struct DocumentRequest {
    std::string request;     // 必填
    std::string context;     // 可选
    std::string format;      // 默认 "markdown"
};

struct DocumentResult {
    bool success = false;
    std::string agent;       // "doc_writer"
    std::string format;      // 实际使用的 format
    std::string content;
    std::string error;
};

class DocWriterAgent final : public IAgent {
public:
    static constexpr const char* kAgentId = "doc_writer";

    DocumentResult run(const DocumentRequest& request) const;

    // IAgent
    std::string run(const std::string& input) override;
    const char* agent_id() const override { return kAgentId; }
};

}  // namespace agentforge

#endif
