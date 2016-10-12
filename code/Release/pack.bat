::@echo off
@pushd "%~dps0"

@set d=%date:~0,4%-%date:~5,2%-%date:~8,2%
@set markFile=%~dps0\libxl.dll
@svn update %markFile% 1>nul 2>nul
@for /f "tokens=1,2" %%i in ('svn info %markFile% ^| find ^"Revision^"') do @set d=(V%%j)%date:~0,4%-%date:~5,2%-%date:~8,2%

@set /p a=输入Svn版本信息及生成日期，留空则为%d%：
@if {%a%}=={} set a=%d%

set publish=SclCheckPub_%a%

md %publish% 2>nul
md %publish%\pdbs 2>nul
md %publish%\xsd 2>nul
md %publish%\x64 2>nul
md %publish%\cfg 2>nul
md %publish%\cfg\private 2>nul
md %publish%\Templates 2>nul

copy /y *.pdb %publish%\pdbs
copy /y ..\*.xml %publish%\cfg\
copy /y ScdIcdCheckService.exe %publish%\
copy /y ScdIcdCheckService64.exe %publish%\
copy /y ScdIcdCheckServiceTest.exe %publish%\
copy /y ScdIcdCheckServiceTest64.exe %publish%\
copy /y ScdIcdCheckService.cfg %publish%\

copy /y ScdIcdCheckTool.exe %publish%\
copy /y ScdIcdCheckTool64.exe %publish%\x64

copy /y libxl.dll %publish%\
copy /y libxl_x64.dll %publish%\x64\libxl.dll

copy /y msvcp100.dll %publish%\
copy /y msvcr100.dll %publish%\

copy /y ScdChecker.exe %publish%\
copy /y ScdChecker64.exe %publish%\x64\
copy /y IcdChecker.exe %publish%\
copy /y IcdChecker64.exe %publish%\x64\
copy /y ScdCrcCalculator.dll %publish%\
copy /y ScdCrcCalculator64.dll %publish%\x64\
copy /y ScdCrcCalculator2.dll %publish%\
copy /y ScdCrcCalculator264.dll %publish%\x64\
copy /y Xml2Xsl.exe %publish%\
copy /y Xml2Xsl64.exe %publish%\x64\
copy /y xerces-c_3_1.dll %publish%\
copy /y xerces-c_3_1x64.dll %publish%\x64\xerces-c_3_1.dll
copy /y 部署说明.txt %publish%\
copy /y ..\xsd\* %publish%\xsd\
copy /y ..\Templates\* %publish%\Templates\

@echo.
@echo.
@echo.
@echo.
@set /p t=按回车自动打开打包目录：
@start explorer /n,/select,"%publish%\x64"
