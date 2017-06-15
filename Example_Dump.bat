@echo off
CD /D "%~dp0"
echo Dump with default config (exefile.ini)
OffsetDumper.exe

echo Dump with custom config (Example_Define.ini)
OffsetDumper.exe Example_Define.ini

echo Dump with custom config (Example_Vars.ini)
OffsetDumper.exe Example_Vars.ini