@echo off
chcp 65001 >nul
title Podmiana plików z polskimi znakami w Adafruit_GFX_Library i WiFi - Seba99317
color 0A
mode con: cols=100 lines=30

echo ====================================================================================
echo                                Podmiana plików z polskimi znakami
echo                                Biblioteki: Adafruit_GFX_Library, WiFi
echo                                Autor: Seba99317
echo ====================================================================================
echo.

setlocal

:: ===== Plik 1: glcdfont.c =====
set "SOURCE1=%~dp0glcdfont.c"
set "DEST1=%USERPROFILE%\Documents\Arduino\libraries\Adafruit_GFX_Library\glcdfont.c"

if not exist "%SOURCE1%" (
    echo [BŁĄD] Brak pliku źródłowego: %SOURCE1%
    pause
    exit /b
)

copy /Y "%SOURCE1%" "%DEST1%" >nul

if exist "%DEST1%" (
    echo [OK] Plik glcdfont.c został podmieniony: %DEST1%
) else (
    echo [BŁĄD] Nie udało się podmienić pliku glcdfont.c!
)

echo.

:: ===== Plik 2: WiFiSTA.cpp =====
set "SOURCE2=%~dp0WiFiSTA.cpp"
set "DEST2=%LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\3.3.0\libraries\WiFi\src\WiFiSTA.cpp"

if not exist "%SOURCE2%" (
    echo [BŁĄD] Brak pliku źródłowego: %SOURCE2%
    pause
    exit /b
)

copy /Y "%SOURCE2%" "%DEST2%" >nul

if exist "%DEST2%" (
    echo [OK] Plik WiFiSTA.cpp został podmieniony: %DEST2%
) else (
    echo [BŁĄD] Nie udało się podmienić pliku WiFiSTA.cpp!
)

echo.
echo ====================================================================================
echo                                Operacja zakończona
echo ====================================================================================
pause
endlocal
