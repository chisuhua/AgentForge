cmake_minimum_required(VERSION 3.20)
project(agentforge
    VERSION 0.1.0
    DESCRIPTION "TUI programming assistant built on HydraForge PDK"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# HydraForge Runtime (fetched at configure time)
# ---------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    HydraForge
    GIT_REPOSITORY https://github.com/chisuhua/HydraForge.git
    GIT_TAG        main  # Sprint 24: pin to specific commit after first green build
    GIT_SHALLOW    TRUE
)

# Sprint 24 Day 1: fetch only — actual targets wired up in Day 2-3
FetchContent_MakeAvailable(HydraForge)

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