#!/bin/bash

PROXY="http://10.0.0.1:3128/"
XINITRC="$HOME/.xinitrc"
PROXY_LINE="export http_proxy='$PROXY'; export https_proxy='$PROXY'; export HTTP_PROXY='$PROXY'; export HTTPS_PROXY='$PROXY';"

apply_proxy() {
  sed -i '/http_proxy/d; /https_proxy/d; /HTTP_PROXY/d; /HTTPS_PROXY/d' "$XINITRC" 2>/dev/null || true
  sed -i "1i$PROXY_LINE" "$XINITRC"
  export http_proxy="$PROXY"
  export https_proxy="$PROXY"
  export HTTP_PROXY="$PROXY"
  export HTTPS_PROXY="$PROXY"
  git config --global http.proxy "$PROXY"
  git config --global https.proxy "$PROXY"
  echo "Proxy applied."
}

remove_proxy() {
  sed -i '/http_proxy/d; /https_proxy/d; /HTTP_PROXY/d; /HTTPS_PROXY/d' "$XINITRC" 2>/dev/null || true
  unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY
  git config --global --unset http.proxy
  git config --global --unset https.proxy
  echo "Proxy removed."
}

case "$1" in
"on")
  apply_proxy
  ;;
"off")
  remove_proxy
  ;;
*)
  echo "Usage: $0 [on|off]"
  exit 1
  ;;
esac
