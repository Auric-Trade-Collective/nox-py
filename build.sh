#!/bin/bash

echo "Building..."
gcc -shared -fPIC -Wall -o libapi.so -fPIC src/*.c -Isrc/include $(python3-config --cflags --ldflags --embed)
echo "Done."