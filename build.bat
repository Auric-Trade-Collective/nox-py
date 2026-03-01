@echo off

FOR /F "tokens=*" %%i IN ('python -c "import sys; print(sys.base_prefix)"') DO SET PYTHON_BASE=%%i
FOR /F "tokens=*" %%i IN ('python -c "import sys; print(f'{sys.version_info.major}{sys.version_info.minor}')"') DO SET PYTHON_VER=%%i

gcc -shared -o libapi.dll noxpy.c -I"%PYTHON_BASE%\include" -L"%PYTHON_BASE%\libs" -lpython%PYTHON_VER%

echo Done