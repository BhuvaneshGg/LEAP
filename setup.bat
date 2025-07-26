@echo off
setlocal EnableDelayedExpansion

:: === CONFIGURATION ===
set "REPO_URL=https://github.com/BhuvaneshGg/LEAP.git"
set "ROOT_FOLDER=LEAP"
set "SUB_FOLDER=leap"
set "APP_FILE=app.py"
set "ENV_FILE=.env"

:: === CHECK REQUIRED TOOLS ===
where git >nul 2>&1 || (echo Git is not installed. & pause & exit /b)
where python >nul 2>&1 || (echo Python is not installed. & pause & exit /b)

:: === CLONE REPO IF NEEDED ===
if not exist "%ROOT_FOLDER%" (
    echo Cloning the LEAP repo...
    git clone %REPO_URL%
)

:: === ENTER leap/ DIRECTORY ===
cd "%ROOT_FOLDER%\%SUB_FOLDER%"

:: === CREATE VIRTUAL ENVIRONMENT IF NEEDED ===
if not exist "venv" (
    echo Creating Python virtual environment...
    python -m venv venv
)

:: === ACTIVATE VENV ===
call venv\Scripts\activate.bat

:: === INSTALL PYTHON MODULES ===
echo Installing required Python modules...
pip install --upgrade pip
pip install flask python-dotenv requests twilio

:: === GET LOCAL IP (ESP32 will connect to this) ===
for /f "tokens=2 delims=:" %%A in ('ipconfig ^| findstr /R "IPv4.*"') do (
    set IP=%%A
    set IP=!IP: =!
    goto :done
)
:done

if not defined IP (
    echo Failed to get local IP.
    pause
    exit /b
)

:: === CREATE .ENV FILE WITH REQUIRED SETTINGS ===
(
    echo ESP32_IP=http://!IP!          
    echo HEART_RATE_LIMIT=200
    echo TEMP_LIMIT=38
    echo TWILIO_SID=your_twilio_sid
    echo TWILIO_AUTH=your_twilio_auth_token
    echo TWILIO_FROM=+1xxxxxxxxxx
    echo TWILIO_TO=+91xxxxxxxxxx
    echo FLASK_HOST=0.0.0.0
    echo FLASK_PORT=5000
    echo HOST_IP=!IP!
    echo SERVER_URL=http://!IP!:5000
) > %ENV_FILE%

echo Generated .env with local IP: !IP!

:: === LAUNCH FLASK APP IN NEW TERMINAL WINDOW ===
start "Flask Server" cmd /k "cd /d %cd% && call venv\Scripts\activate && python %APP_FILE%"

:: === OPEN IN BROWSER ===
timeout /t 5 >nul
start "" "http://!IP!:5000"

echo Done. Flask app launched and browser opened.
pause
