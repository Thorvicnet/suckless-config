#/bin/bash
paru -Qe | awk -F ' ' '{print $1}' >$HOSTNAME"-explicitly-installed.txt"
