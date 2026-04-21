#!/usr/bin/env bash

cd "$(dirname "$0")/.."

if [ -n "$1" ]; then
  src="$1"
  if ! [ -e "$src" ]; then
    echo "error: no such file: $src"
    exit 1
  fi
  set -x
else
  set -x
  magick rose: -scale 1600% test.png
  src=test.png
fi

./tools/compress-group4.sh "$src" test.group4.tiff
./build/tiff_cli test.group4.tiff -o test.group4.tiff.bmp

./tools/compress-jpeg.sh "$src" test.jpeg.tiff
echo this should fail
./build/tiff_cli test.jpeg.tiff

# JBIG2 is out of scope
if false; then
  ./tools/compress-jbig2.sh "$src" test.jbig2
  ./tools/decompress-jbig2.sh test.jbig2 test.jbig2.pbm
fi
