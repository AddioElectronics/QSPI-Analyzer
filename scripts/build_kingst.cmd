@echo off

pushd ..

echo "\nBuilding Kingst win-x64" 
python build.py -p win -a x64 -s kingst -c Release --vs-index 0 --skip --build

echo "\nBuilding Kingst Win32" 
python build.py -p win -a Win32 -s kingst -p win -c Release --vs-index 0 --skip --build

echo "\nBuilding Kingst linux-x64" 
python build.py -p linux -a x64 -s kingst -c Release --distro-index 0 --skip --build

popd