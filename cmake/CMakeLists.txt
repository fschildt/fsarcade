cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_COMPILER clang++)

# set binary directory
set(FSARCADE_ROOT_DIR    "${CMAKE_CURRENT_LIST_DIR}/..")
set(FSARCADE_SRC_DIR     "${FSARCADE_ROOT_DIR}/src")
set(IMGUI_SRC_DIR        "${FSARCADE_ROOT_DIR}/external/imgui")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)


project(fsarcade)

add_executable(fsarcade)
target_sources(fsarcade PUBLIC
    ${FSARCADE_SRC_DIR}/main.cpp
    ${FSARCADE_SRC_DIR}/basic/math.cpp
    ${FSARCADE_SRC_DIR}/renderer/Renderer.cpp
    ${FSARCADE_SRC_DIR}/renderer/RenderGroup.cpp
    ${FSARCADE_SRC_DIR}/renderer/gl_renderer/GlRenderer.cpp
    ${FSARCADE_SRC_DIR}/renderer/gl_renderer/GlVertexBuffer.cpp
    ${FSARCADE_SRC_DIR}/renderer/gl_renderer/GlIndexBuffer.cpp
    ${FSARCADE_SRC_DIR}/renderer/gl_renderer/GlShader.cpp
    ${FSARCADE_SRC_DIR}/games/Game.cpp
    ${FSARCADE_SRC_DIR}/games/tetris/Tetris.cpp
    ${FSARCADE_SRC_DIR}/games/tetris/Tetromino.cpp
    ${FSARCADE_SRC_DIR}/games/tetris/Board.cpp
    ${FSARCADE_SRC_DIR}/games/snake/Snake.cpp
    ${FSARCADE_SRC_DIR}/games/minesweeper/Minesweeper.cpp
    ${IMGUI_SRC_DIR}/imgui.cpp
    ${IMGUI_SRC_DIR}/imgui_draw.cpp
    ${IMGUI_SRC_DIR}/imgui_tables.cpp
    ${IMGUI_SRC_DIR}/imgui_widgets.cpp
    ${IMGUI_SRC_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_SRC_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(fsarcade PUBLIC
    ${FSARCADE_SRC_DIR}
    ${IMGUI_SRC_DIR}
    ${IMGUI_SRC_DIR}/backends
)
target_link_libraries(fsarcade GL GLEW SDL3 dl)
