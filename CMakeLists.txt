cmake_minimum_required(VERSION 3.14...3.22)

project(
  QSPI_Analyzer
  VERSION 1.0
  LANGUAGES CXX
)

# Don't overwrite the AnalyzerSDK_DIR passed from command line
# Instead, append it to CMAKE_MODULE_PATH if it's set
if(DEFINED AnalyzerSDK_DIR)
  list(APPEND CMAKE_MODULE_PATH "${AnalyzerSDK_DIR}")
else()
  # Fallback for local builds
  set(AnalyzerSDK_DIR SaleaeAnalyzerSDK)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${AnalyzerSDK_DIR}")
endif()

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
