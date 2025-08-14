#!/bin/bash

# Test script for 3D Carousel Hyprland Plugin
# This script checks if the plugin can be loaded and tested

set -e

echo "🔍 Testing 3D Carousel Plugin..."

# Check if plugin exists
if [ ! -f "build/hypr-carousel.so" ]; then
    echo "❌ Plugin binary not found! Run: cmake .. && make in build directory"
    exit 1
fi

echo "✅ Plugin binary found: build/hypr-carousel.so"

# Check plugin symbols
echo "🔍 Checking plugin exports..."
if nm build/hypr-carousel.so | grep -q "pluginInit"; then
    echo "✅ pluginInit export found"
else
    echo "❌ pluginInit export missing"
fi

if nm build/hypr-carousel.so | grep -q "pluginExit"; then
    echo "✅ pluginExit export found"
else
    echo "❌ pluginExit export missing"
fi

if nm build/hypr-carousel.so | grep -q "pluginAPIVersion"; then
    echo "✅ pluginAPIVersion export found"
else
    echo "❌ pluginAPIVersion export missing"
fi

# Check dependencies
echo "🔍 Checking dependencies..."
ldd build/hypr-carousel.so | head -10

echo ""
echo "🎯 Plugin Test Summary:"
echo "   📦 Binary: build/hypr-carousel.so"
echo "   🔧 Size: $(ls -lh build/hypr-carousel.so | awk '{print $5}')"
echo "   📋 Exports: $(nm build/hypr-carousel.so | grep -c " T ")"

echo ""
echo "🚀 Ready for installation!"
echo "   Copy to: ~/.local/share/hyprland/plugins/"
echo "   Add to hyprland.conf: plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so"
echo ""
echo "🎮 Usage commands:"
echo "   Toggle: hyprctl dispatch carousel:toggle"
echo "   Next:   hyprctl dispatch carousel:next"
echo "   Prev:   hyprctl dispatch carousel:prev"
echo "   Select: hyprctl dispatch carousel:select"
echo "   Exit:   hyprctl dispatch carousel:exit"