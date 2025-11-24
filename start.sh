#!/bin/sh

export GTK_THEME=Adwaita-dark
export XDG_SESSION_TYPE=wayland
export XDG_CURRENT_DESKTOP=wlroots
export MOZ_ENABLE_WAYLAND=1
export QT_QPA_PLATFORM=wayland
export OZONE_PLATFORM=wayland
export QT_STYLE_OVERRIDE=Adwaita-dark

if [[ $(cat /etc/hostname) == "archnott" ]] && iwctl station wlan0 show | rg "(Personnels)|(Professeurs)" &>/dev/null; then
  export http_proxy="http://10.0.0.1:3128"
  export https_proxy="http://10.0.0.1:3128"
  export HTTP_PROXY="http://10.0.0.1:3128"
  export HTTPS_PROXY="http://10.0.0.1:3128"
fi

"$HOME/.config/suckless/status-daemon/status_daemon" &
exec dbus-run-session dwl -s '
  dbus-update-activation-environment --systemd WAYLAND_DISPLAY XDG_CURRENT_DESKTOP XDG_SESSION_TYPE
  wbg "$HOME/Pictures/background/wave2.png" &
  mako &
  foot --server &
  pass-secret-service &
  kdeconnectd &
  wl-paste --watch cliphist store &
  dwlb &
  xdg-desktop-portal-gtk &
  xdg-desktop-portal-wlr &
  xdg-desktop-portal &
'
