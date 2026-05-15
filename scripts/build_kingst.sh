#!/usr/bin/env bash

set -e

cd ..

echo "\nBuilding kingst x64"
python build.py -s kingst -a x64 -c Release --skip --build