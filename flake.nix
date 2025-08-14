{
  description = "Hyprland 3D Workspace Carousel Plugin";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    # Pin to exact Hyprland 0.50.1 commit for ABI compatibility
    hyprland = {
      url = "github:hyprwm/Hyprland/4e242d086e20b32951fdc0ebcbfb4d41b5be8dcc";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, hyprland, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        hyprlandPkgs = hyprland.packages.${system};
      in
      {
        packages = {
          default = pkgs.stdenv.mkDerivation {
            pname = "hypr-carousel";
            version = "1.0.0";

            src = ./.;

            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
              ninja
            ];

            buildInputs = with pkgs; [
              hyprlandPkgs.hyprland
              
              # Math library for 3D calculations
              glm
              
              # Core dependencies
              wayland
              wayland-protocols
              libGL
              libxkbcommon
              pixman
              libdrm
              mesa
              libinput
              pango
              cairo
            ];

            configurePhase = ''
              runHook preConfigure
              cmake -B build -S . -G Ninja \
                -DCMAKE_BUILD_TYPE=Release \
                -DCMAKE_INSTALL_PREFIX=$out
              runHook postConfigure
            '';

            buildPhase = ''
              runHook preBuild
              ninja -C build
              runHook postBuild
            '';

            installPhase = ''
              runHook preInstall
              mkdir -p $out/lib
              cp build/hypr-carousel.so $out/lib/
              runHook postInstall
            '';

            meta = with pkgs.lib; {
              description = "3D workspace carousel plugin for Hyprland";
              homepage = "https://github.com/user/hypr-carousel";
              license = licenses.mit;
              platforms = platforms.linux;
              maintainers = [ ];
            };
          };
        };

        devShells.default = pkgs.mkShell {
          name = "hypr-carousel-dev";
          
          buildInputs = with pkgs; [
            # Build tools
            cmake
            ninja
            pkg-config
            just
            
            # Hyprland development
            hyprlandPkgs.hyprland
            
            # Math library for 3D calculations
            glm
            
            # Core dependencies
            wayland
            wayland-protocols
            libGL
            libglvnd
            libxkbcommon
            pixman
            libdrm
            mesa
            libinput
            pango
            cairo
            
            # Development tools
            gdb
            valgrind
            clang-tools
            bear
            
            # Nix tools
            nixd
            statix
            deadnix
            nix-tree
            nix-diff
          ];

          PKG_CONFIG_PATH = "${hyprlandPkgs.hyprland}/lib/pkgconfig";
          
          shellHook = ''
            echo "🚀 Hyprland 3D Carousel Plugin Development Environment"
            echo "📦 Available commands:"
            echo "  just build    - Build the plugin"
            echo "  just clean    - Clean build directory"
            echo "  just install  - Install plugin to Hyprland"
            echo "  just test     - Run development tests"
            echo "  just format   - Format code"
            echo "  just lint     - Lint code"
            echo ""
            echo "📚 Development info:"
            echo "  Plugin source: $(pwd)"
            echo "  Hyprland headers: ${hyprlandPkgs.hyprland}"
            echo "  PKG_CONFIG_PATH: $PKG_CONFIG_PATH"
            echo ""
          '';
        };

        # Nix build command
        apps.build = flake-utils.lib.mkApp {
          drv = pkgs.writeShellScriptBin "build-carousel" ''
            set -e
            echo "🔨 Building Hyprland 3D Carousel Plugin..."
            nix build .#default
            echo "✅ Build complete! Plugin: $(realpath result/lib/hypr-carousel.so)"
          '';
        };

        # Development app
        apps.dev = flake-utils.lib.mkApp {
          drv = pkgs.writeShellScriptBin "dev-carousel" ''
            exec nix develop
          '';
        };
      });
}