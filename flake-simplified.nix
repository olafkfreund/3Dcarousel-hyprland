{
  description = "Hyprland 3D Workspace Carousel Plugin - Simplified Version";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    
    # Pin to exact Hyprland commit like working 3D stack plugin
    hyprland = {
      url = "github:hyprwm/Hyprland/4e242d086e20b32951fdc0ebcbfb4d41b5be8dcc";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, hyprland, flake-utils }:
    let
      supportedSystems = [ "x86_64-linux" "aarch64-linux" ];
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
            ];

            # Simplified dependencies like working 3D stack plugin
            buildInputs = with pkgs; [
              hyprlandPkgs.hyprland
              libdrm
              pixman
              wayland
            ];

            # Use simplified CMakeLists.txt
            preConfigure = ''
              cp CMakeLists-fixed.txt CMakeLists.txt
            '';

            configurePhase = ''
              runHook preConfigure
              cmake -B build -S . \
                -DCMAKE_BUILD_TYPE=Release \
                -DCMAKE_INSTALL_PREFIX=$out
              runHook postConfigure
            '';

            buildPhase = ''
              runHook preBuild
              make -C build -j$NIX_BUILD_CORES
              runHook postBuild
            '';

            installPhase = ''
              runHook preInstall
              mkdir -p $out/lib
              cp build/hypr-carousel.so $out/lib/
              runHook postInstall
            '';

            meta = with pkgs.lib; {
              description = "3D workspace carousel plugin for Hyprland (simplified)";
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
          name = "hypr-carousel-dev-simplified";
          
          buildInputs = with pkgs; [
            # Build tools
            cmake
            pkg-config
            
            # Simplified dependencies matching working plugin
            hyprlandPkgs.hyprland
            libdrm
            pixman
            wayland
            
            # Development tools
            gdb
            valgrind
            clang-tools
          ];

          PKG_CONFIG_PATH = "${hyprlandPkgs.hyprland}/lib/pkgconfig";
          
          shellHook = ''
            echo "🚀 Simplified Hyprland 3D Carousel Plugin Development"
            echo "📦 Based on working 3D stack plugin patterns"
            echo "🔧 Build: cmake -B build -S . && make -C build"
            echo "📍 Plugin: $(pwd)"
            echo "🏗️  Hyprland: ${hyprlandPkgs.hyprland}"
            echo ""
          '';
        };
      });
}