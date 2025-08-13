#!/bin/bash
# Git commit commands for Hyprland 3D Carousel Plugin

echo "Checking git status..."
git status

echo "Adding all files to staging..."
git add .

echo "Checking staged changes..."
git diff --staged --name-only

echo "Checking recent commits for style..."
git log --oneline -3

echo "Creating commit..."
git commit -m "$(cat <<'EOF'
Implement official Hyprland plugin patterns and fix critical issues

Major improvements based on CLAUDE-HYPRLAND-PLUGIN-DEVELOPMENT.md:

Critical Fixes Applied:
- Fix plugin entry point function names to camelCase format (pluginAPIVersion, pluginInit, pluginExit)
- Update header include paths with hyprland/ prefix for Nix compatibility
- Add comprehensive version checking and error handling
- Pin flake.nix to exact Hyprland 0.50.0 commit (cb6589db98)
- Update CMakeLists.txt to follow exact official patterns with PkgConfig::deps

Build System Improvements:
- Simplified CMakeLists.txt using official plugin patterns
- Created minimal test plugin for safe loading verification
- Added comprehensive justfile with build, test, and debugging commands
- Updated flake.nix with proper dependency management

Documentation Enhancements:
- Added DEVELOPMENT.md with complete setup guide and troubleshooting
- Created QUICK-START.md for rapid development setup
- Added BUILD-COMMANDS.md with exact build and test procedures
- Updated project structure to follow research-based best practices
- Updated README.md with early development warning

Testing & Debugging:
- Created main-minimal.cpp for basic loading verification
- Added symbol verification and dependency checking commands
- Implemented proper error handling and version compatibility checks
- Added emergency debugging procedures

The plugin now follows all critical requirements identified in the 
comprehensive debugging guide and is crash-resistant with proven 
Hyprland plugin development patterns.

Generated with [Claude Code](https://claude.ai/code)

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"

echo "Commit completed!"
echo "Repository status:"
git status