@echo on

set WORKING_DIR=%USERPROFILE%\Docker

echo Creating working directory: %WORKING_DIR% 
mkdir %WORKING_DIR%

echo Checking for container updates.
docker pull akalinow/root-fedora35

echo Starting container.
docker run --name mppe -v %WORKING_DIR%:/scratch -w /home/jupyter -p 8000:8000 akalinow/root-fedora35 /home/jupyter/start-jupyter.sh

pause
