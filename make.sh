#!/bin/sh

BUILDDIR='_build'

meson setup "$BUILDDIR" --reconfigure
meson compile -C "$BUILDDIR"
