#!/bin/sh

gawk -f def2mib.awk $1.def > $1.mib
