#!/bin/bash
armv5-linux-gcc -std=gnu99 -Wall -fPIC -g -I. -DDEBUG -Iqdecoder-12.0.4/src/  -I./ -D_GNU_SOURCE -c -o rssi.o rssi.c
armv5-linux-gcc -std=gnu99 -Wall -fPIC -g -I. -DDEBUG -Iqdecoder-12.0.4/src/  -I./ -D_GNU_SOURCE -o rssi rssi.o qdecoder-12.0.4/src/libqdecoder.a
