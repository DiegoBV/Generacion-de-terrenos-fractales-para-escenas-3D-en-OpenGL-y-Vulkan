@echo off
set /p vertexShader=Enter Vertex Shader Name (without extension, only .vert extension supported): 
set /p fragmentShader=Enter Fragment Shader Name (without extension, only .frag extension supported): 

echo Processing files...
set CURRENT_FOLDER=%~dp0
for %%a IN ("%CURRENT_FOLDER:~0,-1%") do SET grandparent=%%~dpa
for %%a IN ("%grandparent:~0,-1%") do SET grandgrandparent=%%~dpa
set SHADER_FOLDER=%grandgrandparent%Shaders\
set EXE_FOLDER=%PARENT_FOLDER%compiler\

%EXE_FOLDER%glslc.exe "%SHADER_FOLDER%%vertexShader%.vert" -o %vertexShader%.spv
%EXE_FOLDER%glslc.exe "%SHADER_FOLDER%%fragmentShader%.frag" -o %fragmentShader%.spv

if exist "%CURRENT_FOLDER%%vertexShader%.spv" (
    echo %vertexShader%.spv processed succesfully
    move "%CURRENT_FOLDER%%vertexShader%.spv" "%SHADER_FOLDER%"
    echo "Done. %vertexShader%.spv was generated and moved at original vertex location --> %SHADER_FOLDER%"
) else (
    echo %vertexShader%.spv processed unsuccesfully. No files were generated
)

if exist "%CURRENT_FOLDER%%fragmentShader%.spv" (
    echo %fragmentShader%.spv processed succesfully
    move "%CURRENT_FOLDER%%fragmentShader%.spv" "%SHADER_FOLDER%"
    echo "Done. %fragmentShader%.spv was generated and moved at original vertex location --> %SHADER_FOLDER%"
) else (
    echo %fragmentShader%.spv processed unsuccesfully. No files were generated
)

echo ----END----
pause
