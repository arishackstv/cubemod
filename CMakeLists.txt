cmake_minimum_required(VERSION 3.13)
project(cubemod)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "-W0")
set(CMAKE_CXX_FLAGS_RELEASE "-Od")
set(CMAKE_SHARED_LINKER_FLAGS "/manifest:no")

add_library(cubemod SHARED src/dllmain.cpp src/core/main.h src/core/memory/memory_helper.h src/core/game_structures.h src/core/hook/hooks/item_generation/hook_create_weapon.h src/core/hook/hooks/scaling/hook_get_item_modifier.h src/core/hook/hooks/scaling/hook_get_item_regen.h src/core/hook/hooks/pvp/hook_can_attack_entity.h src/core/hook/hooks/scaling/hook_get_item_haste.h src/core/hook/hooks/item_generation/hook_create_armor.h src/core/main.cpp src/core/hook/hooks/scaling/hook_get_item_crit.h src/core/hook/hooks/item_generation/hook_give_item.h src/core/hook/hooks/artifact/display/hook_concat_artifact_suffix.h src/core/hook/hooks/artifact/display/hook_artifact_display_roundf.h src/core/hook/hooks/artifact/display/hook_round_artifact_thing.h src/core/hook/hooks/artifact/base_stats/hook_get_hit_points.h src/core/hook/hooks/artifact/base_stats/hook_get_player_armor.h src/core/hook/hooks/artifact/hook_set_artifact_stats.h src/core/hook/hooks/artifact/base_stats/hook_get_attack_power.h src/core/hook/hooks/artifact/base_stats/hook_get_spell_power.h src/core/hook/hooks/artifact/base_stats/hook_get_player_resistance.h src/core/hook/hooks/artifact/base_stats/hook_get_player_haste.h src/core/hook/hooks/artifact/base_stats/hook_get_player_regeneration.h src/core/hook/hook.h src/core/hook/hooks/artifact/base_stats/hook_get_player_crit.h src/core/hook/hooks/command/hook_send_chat.h)
target_link_libraries(cubemod dbghelp)