@echo off

pushd ..

echo "\nBuilding Saleae win-x64" 
python build.py -a x64 -p win -s saleae -c Release --vs-index 0 --skip --build

echo "\nBuilding Saleae win-arm64" 
python build.py -a arm64 -p win -s saleae -c Release --vs-index 0 --skip --build

echo "\nBuilding Saleae linux-x64" 
python build.py -a x64 -p linux -s saleae -c Release --distro-index 0 --skip --build

echo "\nBuilding Saleae linux-arm64" 
python build.py -a arm64 -p linux -s saleae -c Release --distro-index 0 --skip --build

popd