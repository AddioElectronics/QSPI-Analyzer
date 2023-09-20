# QSPI Analyzer for Saleae Logic/KingstVIS
---
#### `V1.0.0`

Analyzer for low level debugging of the QSPI Protocol, for use with Saleae and Kingst Logic Analyzers.

##### Features
- Command, Address, Dummy and Data blocks.
- Supports SIO, DIO, and QIO for Command, Address, and data. Dummy is SIO.
- Endianess, CPOL/CPHA, CS Idle State, Extra CS Pre and Post cycles
- All settings simulated.

##### Images
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


##### Future Plans
- Octal IO
- Operation without chip select
- DDR



## Usage
---
#### KingstVIS (Windows)
- Download and install [Kingst Virtual Instruments Studio][king]
- Place `QSpiAnalyzer.dll` into your `\KingstVIS\Analyzer` folder. *(Default C:\Program Files\KingstVIS\Analyzer\)*
- Open KingstVIS and add "QSPI" to Analyzer list
- Set the channels and settings to match your QSPI master device.
- Capture data from Logic Analyzer **or** press play without device connected, which will simulate data.

#### Saleae Logic 1 & 2
- Download and install [Saleae Logic 1][logic1] **or** Download and install [Saleae Logic 2][logic2]
- [Custom Analyzer Install Instructions][logictutorial]
- Add "QSPI" to Analyzer list
- Set the channels and settings to match your QSPI master device.
- Capture data from Logic Analyzer **or** press play without device connected to simulate data.

## Building/Debugging
---
**Note:** Saleae users must also run `git submodule update --init --recursive` to download the [Saleae Analyzer SDK][asdk].
#### Windows

**Building**
1. Open Project in Visual Studio with Administrative rights ***1**
2. Select either the "KingstVIS" or "Saleae project" depending on which software you are using.
3. Hit Play, and your software will launch with a debugger attached.

**The following steps are "optionally required" if any of the following are true**
- Your software was **not** installed to default location
- Your system is **32 bit**
- You are using Saleae Logic **1**

1. Edit Project `Properties->Debugging->Command` to target your software's executable
2. Edit Project `Properties->Build Events->Post-Build Event->Command Line` to make sure the **copy target** is your software's **Analyzers folder.**



***1** *Only required to run as admin when your software is installed in a protected folder like Program Files, and if you wish to use post-build event to copy DLL directly to your analyzers folder*

#### Linux/Mac
---
**Note:** I am not very familiar with CMAKE or Linux/Mac build systems, so all I can provide are basic instructions.
- You must add these preprocessor definitions. *(or define inside QSpiAnalyzer.h)*
    - SALEAE=1 
    - KINGSTVIS=2
    - ANALYZER_VERSION=SALEAE **or** ANALYZER_VERSION=KINGSTVIS
- Add `src` to your include directories.

**KingstVIS**
I edited the make files that were included with the SDK to target the source, includes and library. 
All that should need to be done is the preprocessor definitions.

**Saleae**
- Add `SaleaeAnalyzerSDK\include` to your include directories.
- Add `SaleaeAnalyzerSDK\lib_x86_64` to your libraries directories

Another option would be to build following Saleae's [Sample Analyzer's][sampleanalyzer] build instructions.
I'm not sure what is generated for Linux/Mac, so I suggest [renaming to "QSPI"](https://github.com/saleae/SampleAnalyzer/tree/master#renaming-your-analyzer) before copying pasting the source files.
Also make sure inside `QSpiAnalyer.h` and `QSpiAnalyer.cpp`, `ANALYZER_VERSION` gets renamed to `Analyzer2`.

### Included Libraries
---
They are both almost identical, but unfortunately are not compatible.
The only difference I noticed at first glance was the "Analyzers SDK" has/uses the `Analyzer2` class which derives from `Analyzer`.
- [Analyzer SDK (Saleae)][asdk] 
- [Custom Protocol Analyzers SDK (Kingst)][kingsdk]

## License
---
- [QSPI Analyzer](https://github.com/AddioElectronics/QSPI-Analyzer/LICENSE)
- [Analyzer SDK](https://github.com/AddioElectronics/QSPI-Analyzer/SDK_LICENSE)
## Author
---
- Author : Addio
- Website : www.Addio.io

[asdk]:https://github.com/saleae/AnalyzerSDK
[king]:http://www.qdkingst.com/en/download
[kingsdk]:http://www.qdkingst.com/download/vis_sdk
[logic1]:https://support.saleae.com/logic-software/legacy-software/older-software-releases#logic-1-x-download-links
[logic2]:https://www.saleae.com/downloads/
[logictutorial]:https://support.saleae.com/faq/technical-faq/setting-up-developer-directory
[sampleanalyzer]:https://github.com/saleae/SampleAnalyzer/tree/master
