#!/usr/bin/env bash

set -eux

exec magick "$1" -compress JPEG "$2"
