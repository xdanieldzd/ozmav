@echo off
set WIILOAD=tcp:wii
set WIILOAD=tcp:192.168.2.41

:start
echo About to compile...
set /p userinp="Transfer and run program after compiling? (Default=y) [y/n] "
set userinp=%userinp:~0,1%
if "%userinp%"=="y" goto yes
if "%userinp%"=="n" goto no
if "%userinp%"=="~0,1" goto yes
echo Invalid choice...
goto start

:yes
make test
goto end

:no
make
goto end

:end
echo Done.
pause
