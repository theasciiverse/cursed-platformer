#! /bin/sh
export LD_LIBRARY_PATH=./libs
if [ -x "$(command -v xterm)" ]; then
    xterm -xrm "xterm*faceName:monospace" -xrm "xterm*faceSize:18" -geometry 80x25 -e "./platformer_game_ncurses"
elif [ -x "$(command -v xfce4-terminal)" ]; then
    xfce4-terminal --geometry 80x25 -e "./platformer_game_ncurses"
elif [ -x "$(command -v gnome-terminal)" ]; then
    gnome-terminal --geometry=80x25 -e "./platformer_game_ncurses"
elif [ -x "$(command -v konsole)" ]; then
    konsole -e "./platformer_game_ncurses"
elif [ -x "$(command -v termite)" ]; then
    termite -e "./platformer_game_ncurses"
else
    x-terminal-emulator -geometry 80x25 -e "./platformer_game_ncurses"
fi
