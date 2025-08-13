# Hyprland 3D Carousel Plugin - Development Commands
# Usage: just <command>

# Default recipe - show available commands
default:
    @echo "🚀 Hyprland 3D Carousel Plugin"
    @echo ""
    @echo "📦 Build Commands:"
    @echo "  just build      - Build full plugin with CMake"
    @echo "  just minimal    - Build minimal test plugin (START HERE)"
    @echo "  just nix-build  - Build plugin with Nix"
    @echo "  just clean      - Clean build directory"
    @echo "  just rebuild    - Clean and build"
    @echo ""
    @echo "🔧 Development:"
    @echo "  just dev        - Enter Nix development shell"
    @echo "  just format     - Format code with clang-format"
    @echo "  just lint       - Run static analysis"
    @echo "  just check      - Run all checks"
    @echo ""
    @echo "🚀 Installation:"
    @echo "  just install    - Install plugin to Hyprland"
    @echo "  just uninstall  - Remove plugin from Hyprland"
    @echo ""
    @echo "🧪 Testing:"
    @echo "  just test       - Run development tests"
    @echo "  just debug      - Build with debug symbols"
    @echo "  just minimal    - Build minimal test plugin"
    @echo "  just test-load  - Test minimal plugin loading"
    @echo ""

# Build the plugin using CMake
build:
    @echo "🔨 Building plugin with CMake..."
    mkdir -p build
    cd build && cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
    cd build && ninja
    @echo "✅ Build complete: build/hypr-carousel.so"

# Build with Nix
nix-build:
    @echo "🔨 Building plugin with Nix..."
    nix build .#default
    @echo "✅ Nix build complete: result/lib/hypr-carousel.so"

# Build with debug symbols
debug:
    @echo "🔨 Building plugin with debug symbols..."
    mkdir -p build
    cd build && cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
    cd build && ninja
    @echo "✅ Debug build complete: build/hypr-carousel.so"

# Clean build directory
clean:
    @echo "🧹 Cleaning build directory..."
    rm -rf build
    rm -rf result
    @echo "✅ Clean complete"

# Clean and rebuild
rebuild: clean build

# Enter Nix development shell
dev:
    @echo "🚀 Entering Nix development shell..."
    nix develop

# Format code
format:
    @echo "🎨 Formatting code..."
    find src include -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
    @echo "✅ Code formatted"

# Run static analysis
lint:
    @echo "🔍 Running static analysis..."
    @if command -v clang-tidy >/dev/null 2>&1; then \
        find src -name "*.cpp" | xargs clang-tidy --quiet; \
        echo "✅ Linting complete"; \
    else \
        echo "⚠️  clang-tidy not found, skipping lint"; \
    fi

# Run all checks
check: format lint
    @echo "✅ All checks complete"

# Install plugin to Hyprland plugins directory
install: build
    @echo "📦 Installing plugin to Hyprland..."
    mkdir -p ~/.local/share/hyprland/plugins
    cp build/hypr-carousel.so ~/.local/share/hyprland/plugins/
    @echo "✅ Plugin installed to ~/.local/share/hyprland/plugins/hypr-carousel.so"
    @echo ""
    @echo "📝 Add to your hyprland.conf:"
    @echo "plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so"
    @echo ""
    @echo "🎮 Keybindings:"
    @echo "bind = SUPER, TAB, exec, hyprctl dispatch carousel:toggle"
    @echo "bind = SUPER, right, exec, hyprctl dispatch carousel:next"
    @echo "bind = SUPER, left, exec, hyprctl dispatch carousel:prev"
    @echo "bind = SUPER, RETURN, exec, hyprctl dispatch carousel:select"

# Uninstall plugin
uninstall:
    @echo "🗑️  Uninstalling plugin..."
    rm -f ~/.local/share/hyprland/plugins/hypr-carousel.so
    @echo "✅ Plugin uninstalled"

# Run development tests
test:
    @echo "🧪 Running development tests..."
    @if [ -f build/hypr-carousel.so ]; then \
        echo "✅ Plugin builds successfully"; \
        file build/hypr-carousel.so; \
        echo ""; \
        echo "📊 Plugin size: $(du -h build/hypr-carousel.so | cut -f1)"; \
        echo ""; \
        echo "🔍 Plugin symbols:"; \
        nm -D build/hypr-carousel.so | grep -E "(PLUGIN_|carousel)" || echo "No plugin symbols found"; \
    else \
        echo "❌ Plugin not built. Run 'just build' first"; \
        exit 1; \
    fi

# Show plugin information
info:
    @echo "📋 Plugin Information"
    @echo "Name: Hyprland 3D Carousel"
    @echo "Version: 1.0.0"
    @echo "Description: 3D workspace carousel for Hyprland"
    @echo ""
    @if [ -f build/hypr-carousel.so ]; then \
        echo "📊 Built plugin:"; \
        ls -lh build/hypr-carousel.so; \
        echo ""; \
        echo "🔍 Dependencies:"; \
        ldd build/hypr-carousel.so 2>/dev/null | head -10 || echo "Cannot determine dependencies"; \
    else \
        echo "❌ Plugin not built"; \
    fi

# Create a development package
package: nix-build
    @echo "📦 Creating development package..."
    mkdir -p dist
    cp result/lib/hypr-carousel.so dist/
    cp README.md dist/ 2>/dev/null || echo "# Hyprland 3D Carousel Plugin" > dist/README.md
    @echo "✅ Package created in dist/"

# Quick development cycle
dev-cycle: clean debug test
    @echo "🔄 Development cycle complete"

# Build minimal test plugin
minimal:
    @echo "🔨 Building minimal test plugin..."
    mkdir -p build-minimal
    cp CMakeLists-minimal.txt build-minimal/CMakeLists.txt
    cp -r src build-minimal/
    cd build-minimal && cmake . -G Ninja -DCMAKE_BUILD_TYPE=Debug
    cd build-minimal && ninja
    @echo "✅ Minimal plugin built: build-minimal/hypr-carousel-minimal.so"

# Test minimal plugin loading
test-load:
    @echo "🧪 Testing minimal plugin loading..."
    @if [ -f build-minimal/hypr-carousel-minimal.so ]; then \
        echo "✅ Plugin file exists"; \
        echo "📊 Plugin size: $(du -h build-minimal/hypr-carousel-minimal.so | cut -f1)"; \
        echo ""; \
        echo "🔍 Checking exported symbols..."; \
        nm -D build-minimal/hypr-carousel-minimal.so | grep -E "(plugin|API)" || echo "⚠️  No plugin symbols found"; \
        echo ""; \
        echo "🔗 Checking dependencies..."; \
        ldd build-minimal/hypr-carousel-minimal.so | head -5; \
        echo ""; \
        echo "📝 To test loading: hyprctl plugin load $(pwd)/build-minimal/hypr-carousel-minimal.so"; \
        echo "📝 To test dispatcher: hyprctl dispatch test:hello"; \
    else \
        echo "❌ Minimal plugin not built. Run 'just minimal' first"; \
        exit 1; \
    fi

# Show logs from Hyprland (if running)
logs:
    @echo "📜 Showing Hyprland logs (press Ctrl+C to exit)..."
    journalctl -f -u hyprland.service 2>/dev/null || \
    tail -f ~/.local/share/hyprland/hyprland.log 2>/dev/null || \
    echo "❌ No Hyprland logs found"