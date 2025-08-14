{
  description = "Hyprland 3D Workspace Carousel Plugin";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    
    # Pin to exact Hyprland 0.50.1 commit for ABI compatibility
    hyprland = {
      url = "github:hyprwm/Hyprland/4e242d086e20b32951fdc0ebcbfb4d41b5be8dcc";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    
    # Hyprland dependencies - using specific versions
    hyprutils = {
      url = "github:hyprwm/hyprutils";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    
    hyprlang = {
      url = "github:hyprwm/hyprlang";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    
    hyprcursor = {
      url = "github:hyprwm/hyprcursor";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    
    hyprgraphics = {
      url = "github:hyprwm/hyprgraphics";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    
    aquamarine = {
      url = "github:hyprwm/aquamarine";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, hyprland, hyprutils, hyprlang, hyprcursor, hyprgraphics, aquamarine, flake-utils }:
    let
      # Support systems
      supportedSystems = [ "x86_64-linux" "aarch64-linux" ];
      
      # Generate packages for supported systems
      forAllSystems = f: nixpkgs.lib.genAttrs supportedSystems (system: f system);
      
    in
    {
      # NixOS Module
      nixosModules = {
        default = import ./nixos-module.nix;
        hypr-carousel = import ./nixos-module.nix;
      };

      # Per-system outputs
    } // flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        hyprlandPkgs = hyprland.packages.${system};
        
        # Hyprland dependency packages
        hyprutils-pkg = hyprutils.packages.${system}.default;
        hyprlang-pkg = hyprlang.packages.${system}.default;
        hyprcursor-pkg = hyprcursor.packages.${system}.default;
        hyprgraphics-pkg = hyprgraphics.packages.${system}.default;
        aquamarine-pkg = aquamarine.packages.${system}.default;
      in
      {
        packages = {
          hypr-carousel = pkgs.stdenv.mkDerivation {
            pname = "hypr-carousel";
            version = "1.0.0";

            src = ./.;

            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
              ninja
            ];

            buildInputs = with pkgs; [
              # Hyprland and its dependencies
              hyprlandPkgs.hyprland
              hyprutils-pkg
              hyprlang-pkg
              hyprcursor-pkg
              hyprgraphics-pkg
              aquamarine-pkg
              
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
              
              # Additional dependencies for hyprcursor/hyprgraphics
              libzip
              librsvg
              libjpeg
              libwebp
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
          
          # Default package
          default = self.packages.${system}.hypr-carousel;
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
            hyprutils-pkg
            hyprlang-pkg
            hyprcursor-pkg
            hyprgraphics-pkg
            aquamarine-pkg
            
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
            
            # Additional dependencies
            libzip
            librsvg
            libjpeg
            libwebp
            
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