#!/usr/bin/env bash
cd build

rm -rf server_out.fifo server_in.fifo
mkfifo server_out.fifo
mkfifo server_in.fifo

./robot server & ./robot
exit $?

