# QSPI Analyzer for Saleae Logic/KingstVIS

Analyzer for low level debugging of the QSPI Protocol, for use with Saleae and Kingst Logic Analyzers.

## Supported Platforms

| SDK    | Windows x86 | Windows x64 | Windows arm64 | Linux x64 | Linux arm64 | Mac x64 | Mac arm64 |
|--------|-------------|-------------|---------------|-----------|-------------|---------|-----------|
| Saleae | –           | ✓           | ⚠             | ⚠         | ⚠           | ⚠       | ⚠      |   |
| Kingst | ⚠           | ✓           | –             | ⚠         | –           | ⚠       | –      |   

*✓  Supported • ⚠  Not tested • – Not supported*

## Features
- Command, Address, Dummy and Data blocks.
- Supports SIO, DIO, and QIO for Command, Address, and data. Dummy is SIO.
- Endianess, CPOL/CPHA, CS Idle State, Extra CS Pre and Post cycles
- All settings simulated.

## Images
<details closed>
  <summary>KingstVIS</summary>
    <IMG src="https://github.com/AddioElectronics/QSPI-Analyzer/blob/master/Images/KingstVIS_Simulation.jpg?raw=true"/>
</details>
<details closed>
  <summary>Saleae Logic 1</summary>
  <IMG src="https://github.com/AddioElectronics/QSPI-Analyzer/blob/master/Images/SaleaeLogic_Simulation.jpg?raw=true"/>
</details>
<details closed>
  <summary>Saleae Logic 2</summary>
  <IMG src="https://github.com/AddioElectronics/QSPI-Analyzer/blob/master/Images/SaleaeLogic2_Simulation.jpg?raw=true"/>
</details>
<details closed>
  <summary>Saleae Logic 2 Settings</summary>
  <IMG src="https://github.com/AddioElectronics/QSPI-Analyzer/blob/master/Images/SaleaeLogic2_Settings.jpg?raw=true" />
</details>


##### Roadmap
- Octal IO
- Operation without chip select
- DDR


## Usage

#### Saleae Logic 1 & 2
- Download and install [Saleae Logic 1][logic1] or [Saleae Logic 2][logic2]
- [Custom Analyzer Install Instructions][logictutorial]
- Add "QSPI" to Analyzer list
- Set the channels and settings to match your QSPI master device.
- Capture data from Logic Analyzer **or** press play without device connected to simulate data.

#### KingstVIS (Windows)
- Download and install [Kingst Virtual Instruments Studio][king]
- Place `QSpiAnalyzer.dll` into your `\KingstVIS\Analyzer` folder. *(Default C:\Program Files\KingstVIS\Analyzer\)*
- Open KingstVIS and add "QSPI" to Analyzer list
- Set the channels and settings to match your QSPI master device.
- Capture data from Logic Analyzer **or** press play without device connected to simulate data.

## Building/Debugging

**Note:** Saleae users must also run `git submodule update --init --recursive` to download the [Saleae Analyzer SDK][asdk].

### Building and Debugging with Visual Studio:

1. Open Project in Visual Studio with Administrative rights ***1**
2. Select a configuration that is relative to your software _(Logic1, Logic2, or KingstVIS)_
3. Confirm debugger, and build event paths are correct.
4. Hit Play, and your software will launch with a debugger attached.

***1** Build event requires running VS with administrative rights, to copy the DLL to your analyzers folder.

**Debugger Path and Post Build Event**:

The commands will manually copy the DLL to your analyzers folder,  and start your software, before attaching a debugger when you hit _Start Debugging_ or _F5_.

`The following steps are required if any of the following are true`

 - [ ] Your software was **not** installed to default location
 - [ ] Your system is **32 bit**
 - [ ] You are using Logic 2 (_Logic 2 does not have a default folder for custom analyzers_)

1. Edit Project `Properties->Debugging->Command` to target your software's executable
2. Edit Project `Properties->Build Events->Post-Build Event->Command Line` to make sure the **copy target** is your software's **Analyzers folder.**

**Note**:
Logic 1 may throw an exception when debugger is auto-attached. If this happens, use _Debug->Start without debugging_ or _Ctrl+F5_. You may be able to attach the debugger after using _Debug->Attach To Process..._ or _Ctrl+Alt+P_. 
**Note2**:
Attach a debugger is not reliable, may only work when using _Attach To Process_, and that also has a possibility to fail.

### Building with CMake (Wizard Script)

> Untested on MacOS, please report any issues.

Run build.py, and it will guide you through selecting the SDK, platform, architecture, and build type automatically.

``` bash
python build.py
```
The wizard also supports invoking with arguments to skip prompts. Pass `--help` for more info.

### Building with CMake (Visual Studio)
Open Visual Studio → Continue Without Code → File → Open → CMake → select CMakeLists.txt.
Then choose configuration (Debug/Release, x64) and build.

### Building with CMake (Shell Commands)

**Targeting an SDK**:
Use `-DTARGET_SDK=[saleae|kingst]` option when configuring CMake.
_Will default to saleae if not included._


Example:

1) Windows
``` bash
cmake -G "Visual Studio 18 2026" -A x64 -DTARGET_SDK=saleae ..
cmake --build . --config Release
```

2) Linux
``` bash
mkdir build && cd build
cmake -DTARGET_SDK=saleae -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

3) macOS
``` bash
mkdir build && cd build
cmake -DTARGET_SDK=saleae -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

After building:
Copy the generated plugin to your analyzer’s plugin directory:

**Windows**: QSPI_Analyzer.dll
**Linux**: libQSPI_Analyzer.so
**macOS**: libQSPI_Analyzer.dylib

## SDKs

- [Analyzer SDK (Saleae)][asdk] 
- [Custom Protocol Analyzers SDK (Kingst)][kingsdk]

## License

- [QSPI Analyzer](https://github.com/AddioElectronics/QSPI-Analyzer/LICENSE)
- [Analyzer SDK](https://github.com/AddioElectronics/QSPI-Analyzer/SDK_LICENSE)

## Author

- Author : Addio
- Website : www.Addio.io

[asdk]:https://github.com/saleae/AnalyzerSDK
[king]:http://www.qdkingst.com/en/download
[kingsdk]:http://www.qdkingst.com/download/vis_sdk
[logic1]:https://support.saleae.com/logic-software/legacy-software/older-software-releases#logic-1-x-download-links
[logic2]:https://www.saleae.com/downloads/
[logictutorial]:https://support.saleae.com/faq/technical-faq/setting-up-developer-directory
[sampleanalyzer]:https://github.com/saleae/SampleAnalyzer/tree/master