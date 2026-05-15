@echo off

call build_saleae.cmd
if errorlevel 1 exit /b 1

call build_kingst.cmd
if errorlevel 1 exit /b 1