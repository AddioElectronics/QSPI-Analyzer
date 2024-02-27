cmake_minimum_required(VERSION 3.14...3.22)

project(
  QSPI_Analyzer
  VERSION 1.0
  LANGUAGES CXX
)

set(AnalyzerSDK_DIR SaleaeAnalyzerSDK)
find_package(AnalyzerSDK REQUIRED)

add_library(${PROJECT_NAME} SHARED
    src/QSpiAnalyzer.cpp
    src/QSpiAnalyzer.h
    src/QSpiAnalyzerResults.cpp
    src/QSpiAnalyzerResults.h
    src/QSpiAnalyzerSettings.cpp
    src/QSpiAnalyzerSettings.h
    src/QSpiDataChannelManager.cpp
    src/QSpiDataChannelManager.h
    src/QSpiSimulationDataGenerator.cpp
    src/QSpiSimulationDataGenerator.h
    src/QspiTypes.h
    src/QSpiUtil.h
)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} PUBLIC Saleae::AnalyzerSDK)
target_compile_definitions(${PROJECT_NAME} PRIVATE SALEAE_ANALYZER)