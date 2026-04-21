#!/usr/bin/env bash

set -eux

jbig2dec -o "$2" "$1"
