# Basic NixOS Configuration Example
# Add this to your NixOS configuration

{ config, pkgs, ... }:

{
  # Enable Hyprland with 3D Carousel Plugin
  programs.hyprland = {
    enable = true;
    xwayland.enable = true;
  };

  # Configure 3D Carousel Plugin
  programs.hypr-carousel = {
    enable = true;
    
    # Basic settings - good for most users
    radius = 800;
    spacing = 1.2;
    transparencyGradient = 0.3;
    animationDuration = 300;
    
    # Use default keybindings
    keybindings = {
      toggle = "SUPER, TAB";
      next = "SUPER, right";
      previous = "SUPER, left";
      select = "SUPER, RETURN";
      exit = "SUPER, ESCAPE";
    };
  };

  # Optional: Install additional Hyprland tools
  environment.systemPackages = with pkgs; [
    hyprpaper    # Wallpaper daemon
    hyprpicker   # Color picker
    hypridle     # Idle daemon
    hyprlock     # Screen locker
  ];
}