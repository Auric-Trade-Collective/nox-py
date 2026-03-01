#!/bin/bash

gcc -shared -o libapi.so -fPIC noxpy.c $(python3-config --cflags --ldflags --embed)
echo "Done"