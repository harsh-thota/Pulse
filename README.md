# Pulse - System Monitor

Pulse is a minimal, lightweight system monitor for Windows designed to provide real-time system performance statistics in a clean, modern, and responsive user interface.

## Features

### Modern UI Design
- **Clean, Dark Theme**: Professional dark interface with modern styling
- **Responsive Layout**: Adapts smoothly to different window sizes
- **Scrollable Content**: Vertical scrolling support for all content areas
- **Modular Cards**: Organized information in clean, grouped cards
- **Modern Sidebar**: Icon-based navigation with subtle active highlights

### Performance Monitoring
- **Real-time Graphs**: Enhanced graphs with grid lines, axis labels, and thick colored lines
- **Dynamic Scaling**: Automatic scaling based on data ranges
- **Multiple Metrics**: CPU, GPU, Memory, Disk I/O, and Network monitoring
- **Historical Data**: Ring buffer implementation for efficient memory usage

### System Information
- **Hardware Details**: Comprehensive CPU, GPU, and memory information
- **Process Management**: Detailed process list with sorting and filtering
- **Network Interfaces**: Network adapter information and statistics
- **Alert System**: Configurable alerts for system thresholds

### Performance Optimized
- **Memory Efficient**: Ring buffers for graph data storage
- **Optimized Rendering**: SDL2 with hardware acceleration
- **Minimal Dependencies**: Lightweight architecture
- **60 FPS Target**: Smooth rendering with vsync support

## Screenshots

### Performance Screen
- System overview with key metrics
- Real-time performance graphs with enhanced visualization
- Hardware information cards

### Processes Screen
- Process summary with top CPU and memory usage
- Sortable process table
- Color-coded status indicators

### Network Screen
- Network activity overview
- Combined and separate upload/download graphs
- Network interface details

### Alerts Screen
- System status overview
- Active alerts with severity levels
- Configurable alert rules

## Technical Architecture

### Built With
- **C++20**: Modern C++ with latest features
- **SDL2**: Cross-platform graphics and windowing
- **Clay UI**: Immediate-mode UI layout library
- **CMake**: Build system with Ninja generator
- **Windows APIs**: Native system monitoring

### Key Components
- **DataCollector**: System monitoring and data collection
- **ClayMan**: UI layout and rendering management
- **Screens**: Modular screen system for different views
- **Components**: Reusable UI components (graphs, cards, tables)
- **Ring Buffers**: Efficient circular data storage

## Building and Setup

### Prerequisites
- Windows 10/11
- Visual Studio 2019+ or compatible C++20 compiler
- CMake 3.27 or later
- Ninja build system

### Dependencies Included
- SDL2 (graphics and windowing)
- SDL2_ttf (font rendering)
- Clay UI (layout system)

### Build Instructions

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd Pulse
   ```

2. **Create build directory**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake**
   ```bash
   cmake -G Ninja ..
   ```

4. **Build the project**
   ```bash
   ninja
   ```

5. **Run Pulse**
   ```bash
   ./pulse.exe
   ```

### Building for Lower Memory Usage
To reduce the runtime memory footprint (target ~75 MB), build and run the Release configuration:
```powershell
# Create build directory if it doesn't exist
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
# Build in Release mode
cmake --build build --config Release
# Run the optimized executable
.
  build\Release\pulse.exe
```

### Font Setup
The application expects a font file at `assets/fonts/Roboto-Regular.ttf`. You can:
- Download Roboto font from Google Fonts
- Place any TTF font in the assets/fonts directory
- Update the font path in `application.cpp` if needed

## Project Structure

```
Pulse/
? src/
?   ? main.cpp                    # Application entry point
?   ? core/
?   ?   ? system_state.hpp        # Data structures
?   ?   ? ring_buffer.hpp         # Efficient data storage
?   ?   ? interfaces/             # Abstract interfaces
?   ? monitoring/
?   ?   ? data_collector.*        # System monitoring
?   ? platform/
?   ?   ? windows/                # Windows-specific monitoring
?   ? ui/
?       ? application.*           # Main application class
?       ? components/             # Reusable UI components
?       ? screens/                # Different app screens
? external/
?   ? SDL2/                       # SDL2 library
?   ? clay/                       # Clay UI library
? assets/
?   ? fonts/                      # Font files
? CMakeLists.txt                  # Build configuration
? README.md                       # This file
```

## Usage

### Navigation
- Click on sidebar icons to switch between screens
- Use mouse wheel to scroll content vertically
- Resize window for responsive layout adaptation

### Performance Screen
- View real-time system metrics
- Monitor CPU, GPU, Memory, and Disk usage
- Analyze performance graphs with grid overlays

### Processes Screen
- Browse running processes
- Sort by CPU or memory usage
- View process details and status

### Network Screen
- Monitor network interface activity
- View upload/download speeds
- Analyze network performance graphs

### Alerts Screen
- Configure system monitoring rules
- View active alerts and their severity
- Monitor system health status

## Customization

### Adding New Metrics
1. Extend `SystemState` structure
2. Update `DataCollector` to gather new data
3. Add ring buffer for historical data
4. Create UI components to display the metric

### Modifying UI Theme
- Update color values in component files
- Modify `Clay_Color` structures throughout the codebase
- Adjust corner radius and padding values for different styling

### Performance Tuning
- Adjust ring buffer sizes in `ring_buffer.hpp`
- Modify update intervals in `DataCollector`
- Change FPS target in main loop

## Contributing

1. Follow C++20 coding standards
2. Maintain memory efficiency principles
3. Ensure responsive UI design
4. Add appropriate error handling
5. Document new features and APIs

## License

This project is open-source. Please check the individual library licenses for SDL2 and Clay UI.

## Acknowledgments

- **SDL2**: Cross-platform development library
- **Clay UI**: Immediate-mode UI layout library by nicbarker
- **ClayMan**: C++ wrapper for Clay UI by TimothyHoytBSME

## Future Enhancements

- Additional system metrics (temperature, battery, etc.)
- Export functionality for performance data
- Custom alert notifications
- Multiple theme support
- Plugin system for extensibility
- Cross-platform support (Linux, macOS)

---

*Pulse - Keep your finger on the system's pulse*
