@echo off

cd /D "%~dp0"

IF exist "P:\GravityGun\" (
	echo Removing existing link P:\GravityGun
	rmdir "P:\GravityGun\"
)

echo Creating link P:\GravityGun
mklink /J "P:\GravityGun\" "%cd%\GravityGun\"

echo Done