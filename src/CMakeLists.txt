add_subdirectory(game)
add_subdirectory(int)
add_subdirectory(plib)

target_sources(fallout-ce PRIVATE
    movie_lib.cc
    movie_lib.h
    audio_engine.cc
    audio_engine.h
    fps_limiter.cc
    fps_limiter.h
    platform_compat.cc
    platform_compat.h
    pointer_registry.cc
    pointer_registry.h
)
