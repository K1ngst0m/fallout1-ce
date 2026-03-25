# Emscripten platform configuration for fallout-ce
# SDL3 is provided as an Emscripten port — no find_package needed.

target_compile_options(fallout-ce PUBLIC "-sUSE_SDL=3")
target_link_options(fallout-ce PUBLIC
    "-sUSE_SDL=3"
    "-lidbfs.js"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sINITIAL_MEMORY=268435456"
    "-sSTACK_SIZE=1048576"
    "-sASYNCIFY"
    "-sASYNCIFY_STACK_SIZE=32768"
    "-sEXPORTED_RUNTIME_METHODS=callMain,FS"
    "-sEXPORTED_FUNCTIONS=_main,_fallout_push_mouse_move,_fallout_push_mouse_move_relative,_fallout_push_mouse_button,_fallout_get_mouse_bounds_width,_fallout_get_mouse_bounds_height,_fallout_get_mouse_x,_fallout_get_mouse_y,_fallout_get_mouse_buttons"
    "--shell-file" "${CMAKE_SOURCE_DIR}/src/shell.html"
)
set_target_properties(fallout-ce PROPERTIES SUFFIX ".html")
