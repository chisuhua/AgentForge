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

    # U4 (2026-09-03): pin to specific commit per Oracle session
    # `ses_f988ee9adffe86hvD0UBTb3I6h` recommendation + plan §0.4/§0.5.
    # Do NOT use `GIT_TAG main` (causes FetchContent drift per commit `dc33384`).
    # Bump procedure: edit SHA → `cmake -B build_with_hf -DAGENTFORGE_FETCH_HYDRAFORGE=ON` → ctest.
    # Rollback rule: if upstream breaks AgentForge, revert to last green SHA.
    FetchContent_Declare(
        HydraForge
        GIT_REPOSITORY https://github.com/chisuhua/HydraForge.git
        GIT_TAG        f106c97  # 2026-09-03 baseline: docs(active-status): close master plan drift tracking
        GIT_SHALLOW    TRUE
    )

    FetchContent_MakeAvailable(HydraForge)
endif()

# ---------------------------------------------------------------------------
# AgentForge library (U4 refactor 2026-09-03)
# ---------------------------------------------------------------------------
# Sprint 24 Day 1-2: single executable. U4 baseline requires the core client
# to be linkable from both the agentforge binary AND the test executable.
# CMake disallows linking executables into other targets, so we extract
# the core into a STATIC library and link both consumers against it.
add_library(agentforge_core STATIC
    src/hydraforge_client.cpp
    src/agents/coding_assistant.cpp
    src/agents/doc_writer.cpp
)

target_include_directories(agentforge_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(agentforge_core PUBLIC
    Threads::Threads
)

# ---------------------------------------------------------------------------
# AgentForge binary (Day 1 hello world — full TUI in Day 3+)
# ---------------------------------------------------------------------------
add_executable(agentforge
    src/main.cpp
)

target_link_libraries(agentforge PRIVATE
    agentforge_core
)

# Public include directory for AgentForge headers (e.g. hydraforge_client.h).
# Sprint 24 Day 2+: clients can `#include "agentforge/..."`.
target_include_directories(agentforge
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# FTXUI v6.1.9 vendored at vendor/ftxui/ (Day 2 tarball vendor).
# FTXUI is header-only; we add its include path so Day 3+ TUI code can
# `#include <ftxui/...>` without find_package ceremony.
# Note: vendor/ftxui also has its own CMakeLists.txt (for building its
# examples/tests); we do NOT add_subdirectory it — header-only usage only.
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vendor/ftxui/include)
    target_include_directories(agentforge
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/vendor/ftxui/include
    )
endif()

# Sprint 24 Day 1 target_link_libraries is intentionally minimal.
# Day 3+ will add: agenticdsl::engine (DSLEngine), hydraforge::pdk (PDK macros),
#                   ftxui::ftxui (TUI), nlohmann_json::nlohmann_json (JSON).
# Note (U4 2026-09-03): `Threads::Threads` propagated via `agentforge_core`
# PUBLIC link, so no need to repeat here.

if(AGENTFORGE_FETCH_HYDRAFORGE)
    # Day 2+: target_link_libraries(agentforge PRIVATE agenticdsl::engine hydraforge::pdk ...)
    message(STATUS "AgentForge: HydraForge Runtime + PDK enabled")
else()
    message(STATUS "AgentForge: skeleton build (Day 1). Set -DAGENTFORGE_FETCH_HYDRAFORGE=ON for Day 2+.")
endif()

# ---------------------------------------------------------------------------
# Tests (Sprint 25+ U4 baseline) — Task 2.1
# ---------------------------------------------------------------------------
# U4 baseline: enable with -DAGENTFORGE_BUILD_TESTS=ON. Tests intentionally
# use ONLY HydraForgeClient's public surface (no DSLEngine, no PDK macros)
# so they can run in skeleton build mode before HydraForge Runtime is wired.
# Catch2 v3.5+ fetched via FetchContent (no system Catch2 dependency).
option(AGENTFORGE_BUILD_TESTS "Build AgentForge tests (U4 baseline)" OFF)

if(AGENTFORGE_BUILD_TESTS)
    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.4
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(agentforge_tests
        tests/test_coding_assistant.cpp
        tests/test_doc_writer.cpp
        tests/test_two_agents.cpp
    )

    target_link_libraries(agentforge_tests
        PRIVATE
            agentforge_core
            Catch2::Catch2WithMain
    )

    target_compile_options(agentforge_tests PRIVATE
        -Wall -Wextra -Wpedantic
    )

    enable_testing()
    add_test(NAME agentforge_tests COMMAND agentforge_tests)
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