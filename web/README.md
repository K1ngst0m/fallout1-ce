# Fallout 1 Community Edition — WebAssembly Build

## Prerequisites

Install the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html):

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source emsdk_env.sh
```

You need `emsdk_env.sh` sourced in every new terminal session, or add it to
your shell profile.

## Building

From the repository root:

```bash
emcmake cmake -B build-wasm
cmake --build build-wasm -j $(nproc)
```

This produces three output files in `build-wasm/`:

| File | Description |
|------|-------------|
| `fallout-ce.wasm` | WebAssembly binary |
| `fallout-ce.js` | JavaScript glue code |
| `fallout-ce.html` | HTML shell page |

## Notes

- The `third_party/sdl2/` directory is a stale leftover from the pre-SDL3 era
  and is **not** used by the Emscripten build. SDL3 is provided via Emscripten's
  built-in port system (`-sUSE_SDL=3`).
- Game data files (`.dat` archives) are not bundled during this build step.
  Data packaging via `--preload-file` is handled separately.
- The build uses `-sALLOW_MEMORY_GROWTH=1` with an initial memory of 256 MB
  and a 1 MB stack. Memory will grow automatically as needed at runtime.

## Native Linux Build (unchanged)

The native build is unaffected by Emscripten additions:

```bash
cmake -B build -D CMAKE_BUILD_TYPE=RelWithDebInfo -D BUILD_TESTING=ON
cmake --build build -j $(nproc)
ctest --test-dir build --verbose
```
