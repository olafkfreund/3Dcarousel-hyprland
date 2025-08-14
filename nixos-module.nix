# NixOS Module for Hyprland 3D Carousel Plugin
{ config, lib, pkgs, ... }:

with lib;

let
  cfg = config.programs.hypr-carousel;
  
  # Plugin package from this flake
  hypr-carousel-pkg = pkgs.callPackage ./default.nix { };
  
  # Generate plugin configuration
  configSection = ''
    plugin {
        carousel {
            radius = ${toString cfg.radius}
            spacing = ${toString cfg.spacing}
            transparency_gradient = ${toString cfg.transparencyGradient}
            animation_duration = ${toString cfg.animationDuration}
            ${optionalString cfg.enableDepthBlur "enable_depth_blur = true"}
            max_workspaces = ${toString cfg.maxWorkspaces}
        }
    }
  '';

in {
  options.programs.hypr-carousel = {
    enable = mkEnableOption "Hyprland 3D Carousel Plugin";

    package = mkPackageOption pkgs "hypr-carousel" {
      default = hypr-carousel-pkg;
      description = "The hypr-carousel package to use";
    };

    # Carousel Configuration Options
    radius = mkOption {
      type = types.int;
      default = 800;
      example = 1000;
      description = ''
        Carousel radius in pixels. Larger values create a wider carousel circle.
        Recommended range: 400-1200 pixels.
      '';
    };

    spacing = mkOption {
      type = types.float;
      default = 1.2;
      example = 1.5;
      description = ''
        Workspace spacing multiplier. Controls distance between workspaces.
        Values > 1.0 increase spacing, < 1.0 decrease spacing.
      '';
    };

    transparencyGradient = mkOption {
      type = types.float;
      default = 0.3;
      example = 0.5;
      description = ''
        Transparency effect strength (0.0-1.0). Higher values make distant
        workspaces more transparent, enhancing the depth effect.
      '';
    };

    animationDuration = mkOption {
      type = types.int;
      default = 300;
      example = 250;
      description = ''
        Animation duration in milliseconds. Controls how fast transitions
        occur when switching between workspaces.
      '';
    };

    enableDepthBlur = mkOption {
      type = types.bool;
      default = false;
      example = true;
      description = ''
        Enable depth-based blur effect for distant workspaces.
        May impact performance on older hardware.
      '';
    };

    maxWorkspaces = mkOption {
      type = types.int;
      default = 10;
      example = 12;
      description = ''
        Maximum number of workspaces to display in the carousel.
        Higher values may impact performance.
      '';
    };

    # Keybinding Configuration
    keybindings = {
      toggle = mkOption {
        type = types.str;
        default = "SUPER, TAB";
        example = "ALT, TAB";
        description = "Keybinding to toggle carousel view";
      };

      next = mkOption {
        type = types.str;
        default = "SUPER, right";
        example = "SUPER, l";
        description = "Keybinding to navigate to next workspace";
      };

      previous = mkOption {
        type = types.str;
        default = "SUPER, left";
        example = "SUPER, h";
        description = "Keybinding to navigate to previous workspace";
      };

      select = mkOption {
        type = types.str;
        default = "SUPER, RETURN";
        example = "SUPER, SPACE";
        description = "Keybinding to select highlighted workspace";
      };

      exit = mkOption {
        type = types.str;
        default = "SUPER, ESCAPE";
        example = "SUPER, q";
        description = "Keybinding to exit carousel without switching";
      };
    };

    # Advanced Options
    autoLoadPlugin = mkOption {
      type = types.bool;
      default = true;
      description = ''
        Automatically add the plugin to Hyprland's plugin list.
        When disabled, you need to manually add the plugin path to your Hyprland config.
      '';
    };

    generateKeybindings = mkOption {
      type = types.bool;
      default = true;
      description = ''
        Automatically generate Hyprland keybindings from the keybindings configuration.
        When disabled, you need to manually add keybindings to your Hyprland config.
      '';
    };

    extraConfig = mkOption {
      type = types.lines;
      default = "";
      example = ''
        # Custom carousel settings
        bind = SUPER_ALT, TAB, exec, hyprctl dispatch carousel:toggle
      '';
      description = ''
        Extra configuration to add to the Hyprland config file.
        This is useful for custom keybindings or additional plugin settings.
      '';
    };
  };

  config = mkIf cfg.enable {
    # Assertions for configuration validation
    assertions = [
      {
        assertion = cfg.radius >= 200 && cfg.radius <= 2000;
        message = "hypr-carousel: radius must be between 200 and 2000 pixels";
      }
      {
        assertion = cfg.spacing >= 0.5 && cfg.spacing <= 3.0;
        message = "hypr-carousel: spacing must be between 0.5 and 3.0";
      }
      {
        assertion = cfg.transparencyGradient >= 0.0 && cfg.transparencyGradient <= 1.0;
        message = "hypr-carousel: transparencyGradient must be between 0.0 and 1.0";
      }
      {
        assertion = cfg.animationDuration >= 50 && cfg.animationDuration <= 1000;
        message = "hypr-carousel: animationDuration must be between 50 and 1000 milliseconds";
      }
      {
        assertion = cfg.maxWorkspaces >= 3 && cfg.maxWorkspaces <= 20;
        message = "hypr-carousel: maxWorkspaces must be between 3 and 20";
      }
    ];

    # Add plugin to system packages
    environment.systemPackages = [ cfg.package ];

    # Generate Hyprland configuration
    programs.hyprland = mkIf config.programs.hyprland.enable {
      # Add plugin to Hyprland's plugin list
      plugins = mkIf cfg.autoLoadPlugin [ cfg.package ];
      
      # Generate configuration file content
      extraConfig = mkMerge [
        # Plugin configuration section
        configSection
        
        # Keybindings (if enabled)
        (mkIf cfg.generateKeybindings ''
          # 3D Carousel Plugin Keybindings
          bind = ${cfg.keybindings.toggle}, exec, hyprctl dispatch carousel:toggle
          bind = ${cfg.keybindings.next}, exec, hyprctl dispatch carousel:next
          bind = ${cfg.keybindings.previous}, exec, hyprctl dispatch carousel:prev
          bind = ${cfg.keybindings.select}, exec, hyprctl dispatch carousel:select
          bind = ${cfg.keybindings.exit}, exec, hyprctl dispatch carousel:exit
          
          # Debug keybinding (useful for troubleshooting)
          bind = SUPER_CTRL, T, exec, hyprctl dispatch carousel:test
        '')
        
        # Extra user configuration
        cfg.extraConfig
      ];
    };

    # Warnings for common misconfigurations
    warnings = mkMerge [
      (mkIf (!config.programs.hyprland.enable) [
        "hypr-carousel is enabled but programs.hyprland.enable is false. The plugin will not be loaded automatically."
      ])
      
      (mkIf (cfg.enableDepthBlur && cfg.radius > 1000) [
        "hypr-carousel: Large radius (${toString cfg.radius}) with depth blur enabled may impact performance."
      ])
      
      (mkIf (cfg.maxWorkspaces > 15) [
        "hypr-carousel: High maxWorkspaces value (${toString cfg.maxWorkspaces}) may impact performance."
      ])
    ];
  };

  # Module metadata
  meta = {
    maintainers = with lib.maintainers; [ ]; # Add your maintainer info here
    doc = ./README.md;
    buildDocsInSandbox = false;
  };
}