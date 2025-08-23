# Pulse -  System Monitor for Windows

Pulse is a small, fast, and friendly system monitoring tool for Windows. It gives you real-time insights into CPU, memory, disk, and network usage through a clean, dark-themed interface that adapts to any window size. Whether you're a developer, a power user, or just curious about your system’s performance, Pulse has you covered.

## Architecture & Design Decisions

Pulse is built around simplicity, modularity, and performance:

- **DataCollector**: Uses native Windows APIs (PDH, WMI, Win32) to sample system metrics efficiently.
- **RingBuffer**: Fixed-size circular buffers store the last 5 seconds of data at ~60 Hz, keeping memory usage predictable.
- **ClayMan UI**: An immediate-mode layout system that makes it easy to compose, clip, and style UI elements with minimal overhead.
- **Modular Screens**: Each view (`PerformanceScreen`, `ProcessesScreen`, etc.) lives in its own class for clear separation and easy extensions.
- **Reusable Components**: Graphs, cards, tables, and navigation buttons are all built as standalone components.

## Optimization Techniques

To keep Pulse lean and responsive:

- **Release Build Flags**: On MSVC, we use `/O1 /Os /Gy /GF /GL` (and `-Os -flto` on GCC/Clang) to optimize for size and speed.
- **Fixed Data Window**: Limiting graph history to 5 seconds prevents unbounded memory growth and keeps rendering fast.
- **Custom Ring Buffer**: A templated, fixed-size circular buffer (`src/core/ring_buffer.hpp`) that holds ~300 samples (5 seconds at 60Hz) without dynamic allocations, ensuring O(1) access and a predictable memory footprint.
- **Working Set Trimming**: Calls `SetProcessWorkingSetSize` on Windows each frame to reduce memory footprint in Task Manager.
- **Font Hinting & Filtering**: Enabled TTF font hinting and `SDL_HINT_RENDER_SCALE_QUALITY=1` for crisp text at all sizes.
- **Minimal Asset Footprint**: Only one font file and core DLLs are shipped alongside the EXE.

## Screens

- **Performance**: CPU, GPU, memory, and disk I/O graphs with key stats.
- **Processes**: Sortable table of running processes with CPU & memory usage.
- **Network**: Per-interface upload/download speeds plus an overall view.
- **Alerts**: Custom thresholds and visual warnings when metrics cross limits.

## Getting Started

### Prerequisites

- Windows 10 or later
- Visual Studio 2019 (or newer) with C++20 support
- CMake 3.27+ and Ninja (or another generator)

### Build & Run

```powershell
git clone https://github.com/harsh-thota/Pulse.git
cd Pulse
mkdir build; cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
.build\Release\pulse.exe
```

### Packaging for Release

After building, bundle these files into a ZIP:

- `Release\pulse.exe`
- `Release\SDL2.dll`
- `Release\SDL2_ttf.dll`
- `assets\fonts\Roboto-Regular.ttf`
- Any other files in `assets/`

## Codebase
```
Pulse/
├─ src/
│  ├─ core/
│  │  ├─ ring_buffer.hpp
│  │  └─ system_state.hpp
│  ├─ monitoring/
│  ├─ ui/
│  └─ main.cpp
├─ external/
├─ assets/
├─ CMakeLists.txt
└─ README.md
```