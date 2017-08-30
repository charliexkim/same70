#!/bin/sh
MAP=a.map
grep -A 1 \.bss\.$1 $MAP
