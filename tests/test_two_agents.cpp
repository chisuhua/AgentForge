#include <catch2/catch_test_macros.hpp>

#include "agentforge/hydraforge_client.h"

using agentforge::HydraForgeClient;

TEST_CASE("AgentForge exposes two independent domain agents", "[u4][integration]") {
    HydraForgeClient client;
    REQUIRE(client.initialize());

    REQUIRE(client.has_agent("coding_assistant"));
    REQUIRE(client.has_agent("doc_writer"));
    REQUIRE(client.agent_count() == 2);

    auto agents = client.list_agents();
    REQUIRE(agents.size() == 2);
}

TEST_CASE("two agents can run independently", "[u4][integration]") {
    HydraForgeClient client;
    REQUIRE(client.initialize());

    auto coding_output = client.run_agent("coding_assistant", "review int add(int, int)");
    REQUIRE(coding_output.find("\"success\":true") != std::string::npos);

    auto docs_output = client.run_agent("doc_writer", "document int add(int, int)");
    REQUIRE(docs_output.find("\"success\":true") != std::string::npos);
}

TEST_CASE("agent failure does not unregister another agent", "[u4][integration]") {
    HydraForgeClient client;
    REQUIRE(client.initialize());

    // doc_writer fails (empty input)
    auto failed = client.run_agent("doc_writer", "");
    REQUIRE(failed.find("\"success\":false") != std::string::npos);

    // coding_assistant still available
    REQUIRE(client.has_agent("coding_assistant"));
    auto coding = client.run_agent("coding_assistant", "noop");
    REQUIRE(coding.find("\"success\":true") != std::string::npos);
}

TEST_CASE("unknown agent returns failure without unregistering known agents", "[u4][integration]") {
    HydraForgeClient client;
    REQUIRE(client.initialize());

    auto unknown = client.run_agent("nonexistent_agent", "test");
    REQUIRE(unknown.find("\"success\":false") != std::string::npos);

    REQUIRE(client.has_agent("coding_assistant"));
    REQUIRE(client.has_agent("doc_writer"));
}
