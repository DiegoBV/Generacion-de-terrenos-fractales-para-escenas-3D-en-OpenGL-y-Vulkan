@echo off
set shaderName=%1
set extension=%2

echo Processing files...
set CURRENT_FOLDER=%~dp0
for %%a IN ("%CURRENT_FOLDER:~0,-1%") do SET grandparent=%%~dpa
for %%a IN ("%grandparent:~0,-1%") do SET grandgrandparent=%%~dpa
set SHADER_FOLDER=%grandgrandparent%Shaders\
set EXE_FOLDER=%PARENT_FOLDER%compiler\

%EXE_FOLDER%glslc.exe "%SHADER_FOLDER%%shaderName%%extension%" -o %shaderName%.spv

if exist "%CURRENT_FOLDER%%shaderName%.spv" (
    echo %shaderName%.spv processed succesfully
    move "%CURRENT_FOLDER%%shaderName%.spv" "%SHADER_FOLDER%"
    echo "Done. %shaderName%.spv was generated and moved at original shader location --> %SHADER_FOLDER%"
) else (
    echo %shaderName%.spv processed unsuccesfully. No files were generated
)

echo ----END----
