#!/bin/sh

meson setup build --reconfigure
ninja -C build
