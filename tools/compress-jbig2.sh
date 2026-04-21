#!/usr/bin/env bash

set -eux

jbig2 "$1" >"$2"
