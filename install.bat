@echo off
set LIBS_DIR=libs

if not exist %LIBS_DIR% (
    mkdir %LIBS_DIR%
)

echo Downloading required dependencies into %LIBS_DIR%...

:: Download GLFW
echo Downloading GLFW...
powershell -Command "& {Invoke-WebRequest -Uri 'https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip' -OutFile '%LIBS_DIR%\glfw.zip'}"
powershell -Command "& {Expand-Archive -Path '%LIBS_DIR%\glfw.zip' -DestinationPath '%LIBS_DIR%'}"
move %LIBS_DIR%\glfw-3.3.8\include\GLFW %LIBS_DIR% >nul
rmdir /s /q %LIBS_DIR%\glfw-3.3.8
del %LIBS_DIR%\glfw.zip
echo GLFW downloaded.

:: Download tiny_obj_loader
echo Downloading tiny_obj_loader...
powershell -Command "& {Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/tinyobjloader/tinyobjloader/release/tiny_obj_loader.h' -OutFile '%LIBS_DIR%\tiny_obj_loader.h'}"
echo tiny_obj_loader downloaded.

:: Download GLM
echo Downloading GLM...
powershell -Command "& {Invoke-WebRequest -Uri 'https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip' -OutFile '%LIBS_DIR%\glm.zip'}"
powershell -Command "& {Expand-Archive -Path '%LIBS_DIR%\glm.zip' -DestinationPath '%LIBS_DIR%'}"
rename "%LIBS_DIR%\glm" glm_temp
move "%LIBS_DIR%\glm_temp\glm" "%LIBS_DIR%" >nul
rmdir /s /q "%LIBS_DIR%\glm_temp"
del "%LIBS_DIR%\glm.zip"
echo GLM downloaded.

:: Download stb_image
echo Downloading stb_image...
powershell -Command "& {Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/nothings/stb/master/stb_image.h' -OutFile '%LIBS_DIR%\stb_image.h'}"
echo stb_image downloaded.

echo All dependencies downloaded successfully!
