#!/usr/bin/env bash
set -e

cd ..

echo "\nBuilding Saleae x64"
python build.py -s saleae -a x64 -c Release --skip --build

echo "\nBuilding Saleae arm64"
python build.py -s saleae -a arm64 -c Release --skip --build

echo "\nBuilding kingst x64"
python build.py -s kingst -a x64 -c Release --skip --build