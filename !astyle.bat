@echo off
set ASTYLEEXE=c:\bin\astyle.exe
set ASTYLEOPT=-n -Q --options=astyle-cpp-options
%ASTYLEEXE% %ASTYLEOPT% Util\*.h Util\*.cpp
%ASTYLEEXE% %ASTYLEOPT% *.h *.cpp
