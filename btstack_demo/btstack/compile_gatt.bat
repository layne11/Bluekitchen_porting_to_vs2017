set PYTHON_COMPILE_PATH=.\tool\
set PYTHON_COMMAND_NAME=compile_gatt.py
set PYTHON_COMMAND=%PYTHON_COMPILE_PATH%%PYTHON_COMMAND_NAME%
set OUTPUT_PATH=%cd%

if exist %PYTHON_COMMAND% (
    echo python compiler found..
) else (
    echo python compiler not found, exiting...
	goto :eof
)

for /R .\ %%G in (*.gatt) do (

set filepath=%%~pG
set filename=%%~nG%%~xG

echo Compiling %%G
python %PYTHON_COMMAND% %%~dG%%~pG%%~nG%%~xG %%~dG%%~pG%%~nG.h
)

pause
