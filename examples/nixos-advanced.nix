# Advanced NixOS Configuration Example
# Comprehensive setup with performance tuning

{ config, pkgs, lib, ... }:

{
  programs.hyprland = {
    enable = true;
    xwayland.enable = true;
    
    # Additional configuration for better performance
    settings = {
      # Optimize for carousel plugin
      animations = {
        enabled = true;
        bezier = [
          "myBezier, 0.05, 0.9, 0.1, 1.05"
          "carouselBezier, 0.25, 0.46, 0.45, 0.94"
        ];
        animation = [
          "windows, 1, 7, myBezier"
          "windowsOut, 1, 7, default, popin 80%"
          "border, 1, 10, default"
          "borderangle, 1, 8, default"
          "fade, 1, 7, default"
          "workspaces, 1, 6, carouselBezier"
        ];
      };
      
      # Optimize rendering
      decoration = {
        rounding = 10;
        blur = {
          enabled = true;
          size = 8;
          passes = 1;
          new_optimizations = true;
        };
        drop_shadow = true;
        shadow_range = 4;
        shadow_render_power = 3;
      };
      
      # Performance settings
      misc = {
        force_default_wallpaper = 0;
        disable_hyprland_logo = true;
        vfr = true;  # Variable frame rate
      };
    };
  };

  # Advanced 3D Carousel Configuration
  programs.hypr-carousel = {
    enable = true;
    
    # High-performance settings
    radius = 1000;
    spacing = 1.5;
    transparencyGradient = 0.4;
    animationDuration = 250;
    enableDepthBlur = true;  # Enable for modern hardware
    maxWorkspaces = 12;
    
    # Custom keybindings with alternatives
    keybindings = {
      toggle = "SUPER, TAB";
      next = "SUPER, right";
      previous = "SUPER, left";
      select = "SUPER, RETURN";
      exit = "SUPER, ESCAPE";
    };
    
    # Additional custom configuration
    extraConfig = ''
      # Alternative keybindings
      bind = ALT, TAB, exec, hyprctl dispatch carousel:toggle
      bind = ALT, right, exec, hyprctl dispatch carousel:next
      bind = ALT, left, exec, hyprctl dispatch carousel:prev
      
      # Mouse bindings for carousel
      bind = SUPER, mouse_down, exec, hyprctl dispatch carousel:next
      bind = SUPER, mouse_up, exec, hyprctl dispatch carousel:prev
      
      # Quick workspace switching with carousel preview
      bind = SUPER, 1, exec, hyprctl dispatch carousel:toggle && sleep 0.1 && hyprctl dispatch workspace 1
      bind = SUPER, 2, exec, hyprctl dispatch carousel:toggle && sleep 0.1 && hyprctl dispatch workspace 2
      bind = SUPER, 3, exec, hyprctl dispatch carousel:toggle && sleep 0.1 && hyprctl dispatch workspace 3
      bind = SUPER, 4, exec, hyprctl dispatch carousel:toggle && sleep 0.1 && hyprctl dispatch workspace 4
      bind = SUPER, 5, exec, hyprctl dispatch carousel:toggle && sleep 0.1 && hyprctl dispatch workspace 5
    '';
  };

  # Hardware acceleration for better performance
  hardware.opengl = {
    enable = true;
    driSupport = true;
    driSupport32Bit = true;
    extraPackages = with pkgs; [
      intel-media-driver
      vaapiIntel
      vaapiVdpau
      libvdpau-va-gl
    ];
  };

  # Additional packages for enhanced experience
  environment.systemPackages = with pkgs; [
    # Hyprland ecosystem
    hyprpaper
    hyprpicker
    hypridle
    hyprlock
    
    # Performance monitoring
    htop
    nvtop
    glxinfo
    
    # Development tools (optional)
    gdb
    valgrind
  ];

  # System optimizations for smooth animations
  boot.kernel.sysctl = {
    "vm.swappiness" = 10;
    "vm.vfs_cache_pressure" = 50;
  };

  # User-specific optimizations
  users.users.yourusername = {
    extraGroups = [ "video" "render" ];
  };
}