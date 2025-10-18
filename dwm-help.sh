#!/bin/sh

cat <<'EOF' | dmenu -l 25
Alt  + p              | run launcher (dmenu_run)
Alt  + Return         | open terminal (kitty)
Alt  + b              | toggle bar visibility
Alt  + j / k          | focus next / prev client
Alt  + i / d          | grow / shrink master client count
Alt  + h              | shrink master area
Alt  + l              | lock screen (slock)
Alt  + Shift + Return | promote to master (zoom)
Alt  + Tab            | previous tag
Alt  + Shift + c      | close focused window
Alt  + t / f / m      | tile / float / monocle layout
Alt  + Shift + f      | toggle fullscreen
Alt  + space          | next layout
Alt  + Shift + space  | toggle floating
Alt  + 0              | view all tags
Alt  + Shift + 0      | tag window on all tags
Alt  + , / .          | focus monitor left / right
Alt  + Shift + , /.   | send window to monitor left / right
Alt  + {1-9}          | view tag N
Alt  + Shift + {1-9}  | tag window with tag N
Alt  + s              | screenshot (Flameshot)
Alt  + v              | show clipboard history
Alt  + w              | select wifi network
Alt  + Shift + b      | open Brave
Alt  + Shift + v      | open Vesktop
Alt  + Shift + h      | show this help
Alt  + Shift + q      | quit dwm
Audio Fn keys         | volume
Brightness Fn keys    | backlight
EOF
