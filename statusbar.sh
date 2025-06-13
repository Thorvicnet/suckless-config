#!/bin/sh

bluetooth() {
  static_bluetooth_status="/tmp/bluetooth_status"
  current_time=$(date +%s)

  if [ -f "$static_bluetooth_status" ]; then
    last_update=$(stat -c %Y "$static_bluetooth_status")
  else
    last_update=0
  fi

  if [ $((current_time - last_update)) -ge 60 ]; then
    powered=$(bluetoothctl show | grep "Powered:" | awk '{ print $2 }')
    if [ "$powered" = "yes" ]; then
      connected=$(bluetoothctl devices Connected | wc -l)
      if [ "$connected" -gt 0 ]; then
        echo "󰂯" >"$static_bluetooth_status"
      else
        echo "" >"$static_bluetooth_status"
      fi
    else
      echo "" >"$static_bluetooth_status"
    fi
  fi

  cat "$static_bluetooth_status"
}

battery() {
  capacity=$(cat /sys/class/power_supply/BAT0/capacity 2>/dev/null)
  status=$(cat /sys/class/power_supply/BAT0/status 2>/dev/null)

  if [ "$status" = "Charging" ]; then
    icon="󰂄"
  elif [ "$capacity" -ge 80 ]; then
    icon="󰂁"
  elif [ "$capacity" -ge 60 ]; then
    icon="󰁿"
  elif [ "$capacity" -ge 40 ]; then
    icon="󰁽"
  elif [ "$capacity" -ge 20 ]; then
    icon="󰁻"
  else
    icon="󰂎"
  fi

  echo "$icon $capacity%"
}

usbmon() {
  usb1=$(lsblk -la | awk '/sdc1/ { print $1 }')
  usb1mounted=$(lsblk -la | awk '/sdc1/ { print $7 }')

  if [ "$usb1" ]; then
    if [ -z "$usb1mounted" ]; then
      echo " |"
    else
      echo " $usb1 |"
    fi
  fi
}

fsmon() {
  ROOTPART=$(df -h | awk '/\/$/ { print $3}')
  HOMEPART=$(df -h | awk '/\/home/ { print $3}')
  SWAPPART=$(cat /proc/swaps | awk '/\// { print $4 }')

  echo "   $ROOTPART    $HOMEPART    $SWAPPART"
}

ram() {
  mem=$(free -h | awk '/Mem:/ { print $3 }' | cut -f1 -d 'i')
  echo "  $mem"
}

cpu() {
  read -r cpu a b c previdle rest </proc/stat
  prevtotal=$((a + b + c + previdle))
  sleep 0.5
  read -r cpu a b c idle rest </proc/stat
  total=$((a + b + c + idle))
  cpu=$((100 * ((total - prevtotal) - (idle - previdle)) / (total - prevtotal)))
  echo  "$cpu"%
}

network() {
  conntype=$(ip route | awk '/default/ { print substr($5,1,1) }')

  if [ -z "$conntype" ]; then
    echo "󰤭 down"
  elif [ "$conntype" = "e" ]; then
    echo "? up"
  elif [ "$conntype" = "w" ]; then
    echo "  up"
  fi
}

volume_pa() {
  muted=$(pactl list sinks | awk '/Mute:/ { print $2 }')
  vol=$(pactl list sinks | grep Volume: | awk 'FNR == 1 { print $5 }' | cut -f1 -d '%')

  if [ "$muted" = "yes" ]; then
    echo "󰝟 muted"
  else
    if [ "$vol" -ge 65 ]; then
      echo " $vol%"
    elif [ "$vol" -ge 40 ]; then
      echo " $vol%"
    elif [ "$vol" -ge 0 ]; then
      echo " $vol%"
    fi
  fi

}

volume_alsa() {

  mono=$(amixer -M sget Master | grep Mono: | awk '{ print $2 }')

  if [ -z "$mono" ]; then
    muted=$(amixer -M sget Master | awk 'FNR == 6 { print $7 }' | sed 's/[][]//g')
    vol=$(amixer -M sget Master | awk 'FNR == 6 { print $5 }' | sed 's/[][]//g; s/%//g')
  else
    muted=$(amixer -M sget Master | awk 'FNR == 5 { print $6 }' | sed 's/[][]//g')
    vol=$(amixer -M sget Master | awk 'FNR == 5 { print $4 }' | sed 's/[][]//g; s/%//g')
  fi

  if [ "$muted" = "off" ]; then
    echo "󰝟 muted"
  else
    if [ "$vol" -ge 65 ]; then
      echo " $vol%"
    elif [ "$vol" -ge 40 ]; then
      echo " $vol%"
    elif [ "$vol" -ge 0 ]; then
      echo " $vol%"
    fi
  fi
}

clock() {
  dte=$(date -I)
  time=$(date +"%H:%M")

  echo " $dte  $time"
}

main() {
  while true; do
    xsetroot -name " $(usbmon) $(bluetooth) $(ram) | $(cpu) | $(network) | $(volume_pa) | $(battery) | $(clock)"
    sleep 1
  done
}

main
