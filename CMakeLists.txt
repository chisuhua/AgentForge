cmake_minimum_required(VERSION 3.20)
project(agentforge
    VERSION 0.1.0
    DESCRIPTION "TUI programming assistant built on HydraForge PDK"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Threads: needed by HydraForgeClient (Day 2+), but also referenced by target_link_libraries below
# so we resolve it here for Day 1 forward-compat.
find_package(Threads REQUIRED)

# ---------------------------------------------------------------------------
# HydraForge Runtime (fetched at configure time)
# ---------------------------------------------------------------------------
# Day 1.1: gated behind AGENTFORGE_FETCH_HYDRAFORGE=ON (default OFF) so the
# skeleton can compile without the long FetchContent clone. Day 2+ flips this
# on once vendored deps + HydraForgeClient wiring is in place.
option(AGENTFORGE_FETCH_HYDRAFORGE "Fetch and link HydraForge Runtime + PDK (Day 2+)" OFF)

if(AGENTFORGE_FETCH_HYDRAFORGE)
    include(FetchContent)

    FetchContent_Declare(
        HydraForge
        GIT_REPOSITORY https://github.com/chisuhua/HydraForge.git
        GIT_TAG        main  # Sprint 24: pin to specific commit after first green build
        GIT_SHALLOW    TRUE
    )

    FetchContent_MakeAvailable(HydraForge)
endif()

# ---------------------------------------------------------------------------
# AgentForge binary (Day 1 hello world — full TUI in Day 3+)
# ---------------------------------------------------------------------------
add_executable(agentforge
    src/main.cpp
)

# Sprint 24 Day 1 target_link_libraries is intentionally minimal.
# Day 3+ will add: agenticdsl::engine (DSLEngine), hydraforge::pdk (PDK macros),
#                   ftxui::ftxui (TUI), nlohmann_json::nlohmann_json (JSON).
target_link_libraries(agentforge PRIVATE
    Threads::Threads
)

if(AGENTFORGE_FETCH_HYDRAFORGE)
    # Day 2+: target_link_libraries(agentforge PRIVATE agenticdsl::engine hydraforge::pdk ...)
    message(STATUS "AgentForge: HydraForge Runtime + PDK enabled")
else()
    message(STATUS "AgentForge: skeleton build (Day 1). Set -DAGENTFORGE_FETCH_HYDRAFORGE=ON for Day 2+.")
endif()

# ---------------------------------------------------------------------------
# Warnings (project style: -Wall -Wextra -Wpedantic, no -Werror for MVP)
# ---------------------------------------------------------------------------
target_compile_options(agentforge PRIVATE
    -Wall -Wextra -Wpedantic
)

# ---------------------------------------------------------------------------
# Install (placeholder — Sprint 25+)
# ---------------------------------------------------------------------------
# install(TARGETS agentforge RUNTIME DESTINATION bin)