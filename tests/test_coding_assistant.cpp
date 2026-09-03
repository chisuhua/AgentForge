#include <catch2/catch_test_macros.hpp>

#include "agentforge/hydraforge_client.h"

TEST_CASE("coding assistant can be initialized", "[agent][baseline]") {
    agentforge::HydraForgeClient client;
    REQUIRE(client.initialize());
}

TEST_CASE("coding assistant handles a request", "[agent][baseline]") {
    agentforge::HydraForgeClient client;
    REQUIRE(client.initialize());

    REQUIRE(client.start_session("review this function"));
    REQUIRE(client.session_active());

    client.stop_session();
    REQUIRE_FALSE(client.session_active());
}

TEST_CASE("coding assistant lifecycle is idempotent", "[agent][baseline]") {
    agentforge::HydraForgeClient client;
    REQUIRE(client.initialize());

    client.start_session("task one");
    client.stop_session();
    client.start_session("task two");
    REQUIRE(client.session_active());

    client.stop_session();
    REQUIRE_FALSE(client.session_active());
}
