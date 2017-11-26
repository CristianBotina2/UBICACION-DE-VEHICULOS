#!/bin/bash

make clean
make

scp ./build/ch.bin root@omega-41ef.local:
