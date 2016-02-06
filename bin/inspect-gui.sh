#!/usr/bin/sh
./inspect-gui $@ & PID=$!
wait $PID
START=".WIP15_img_"
END="_*"
rm -f $START$PID$END
