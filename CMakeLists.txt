cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)

project(cubemod)
include_directories(src/core)

include_directories(lib)
link_directories("lib/xed")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "-w -fpermissive -masm=intel -static-libgcc -static-libstdc++ -s -Wl,--exclude-all-symbols")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_library(cubemod SHARED src/dllmain.cpp src/core/main.h src/core/memory/memory_helper.h src/core/game_structures.h src/core/hook/hooks/item_generation/hook_create_weapon.h src/core/hook/hooks/scaling/hook_get_item_modifier.h src/core/hook/hooks/scaling/hook_get_item_regen.h src/core/hook/hooks/pvp/hook_can_attack_entity.h src/core/hook/hooks/scaling/hook_get_item_haste.h src/core/hook/hooks/item_generation/hook_create_armor.h src/core/main.cpp src/core/hook/hooks/scaling/hook_get_item_crit.h src/core/hook/hooks/artifact/display/hook_concat_artifact_suffix.h src/core/hook/hooks/artifact/display/hook_artifact_display_roundf.h src/core/hook/hooks/artifact/display/hook_round_artifact_thing.h src/core/hook/hooks/artifact/base_stats/hook_get_hit_points.h src/core/hook/hooks/artifact/base_stats/hook_get_player_armor.h src/core/hook/hooks/artifact/hook_set_artifact_stats.h src/core/hook/hooks/artifact/base_stats/hook_get_attack_power.h src/core/hook/hooks/artifact/base_stats/hook_get_spell_power.h src/core/hook/hooks/artifact/base_stats/hook_get_player_resistance.h src/core/hook/hooks/artifact/base_stats/hook_get_player_haste.h src/core/hook/hooks/artifact/base_stats/hook_get_player_regeneration.h src/core/hook/hook.h src/core/hook/hooks/artifact/base_stats/hook_get_player_crit.h src/core/hook/hooks/command/hook_send_chat.h)
set_target_properties(cubemod PROPERTIES SUFFIX ".cwmod")

target_link_libraries(cubemod psapi xed)