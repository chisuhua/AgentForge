#include <catch2/catch_test_macros.hpp>

#include "agentforge/agents/coding_assistant.h"
#include "agentforge/agents/doc_writer.h"

using agentforge::CodingAssistantAgent;
using agentforge::CodingRequest;
using agentforge::DocWriterAgent;
using agentforge::DocumentRequest;
using agentforge::DocumentResult;

namespace {

DocumentResult run_doc_writer(const DocumentRequest& req) {
    DocWriterAgent agent;
    return agent.run(req);
}

}  // namespace

TEST_CASE("doc writer creates markdown from a request", "[agent][u4]") {
    auto result = run_doc_writer({
        .request = "Describe the public API",
        .context = "HydraForgeClient exposes initialize and stop_session",
        .format = "markdown",
    });

    REQUIRE(result.success);
    REQUIRE(result.agent == "doc_writer");
    REQUIRE(result.format == "markdown");
    REQUIRE_FALSE(result.content.empty());
}

TEST_CASE("doc writer defaults to markdown when format is empty", "[agent][u4]") {
    auto result = run_doc_writer({
        .request = "Write a release note",
        .context = "",
        .format = "",
    });

    REQUIRE(result.success);
    REQUIRE(result.format == "markdown");
}

TEST_CASE("doc writer rejects missing request", "[agent][u4]") {
    auto result = run_doc_writer({
        .request = "",
        .context = "context",
        .format = "markdown",
    });

    REQUIRE_FALSE(result.success);
    REQUIRE(result.error == "Missing arg: request");
}

TEST_CASE("doc writer does not invoke coding assistant", "[agent][u4]") {
    // Isolation test: doc_writer failure/success must not affect coding_assistant.
    auto docs = run_doc_writer({.request = "", .context = "", .format = ""});
    REQUIRE_FALSE(docs.success);

    CodingAssistantAgent coding;
    auto coding_result = coding.run(CodingRequest{.task = "noop", .code = ""});
    REQUIRE(coding_result.success);
}
