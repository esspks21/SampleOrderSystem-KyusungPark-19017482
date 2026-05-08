@echo off
setlocal EnableDelayedExpansion
cd /d "%~dp0"

echo =====================================================
echo  SampleOrderSystem Regression Test Runner
echo =====================================================

:: ── 1. Find cl.exe (Visual Studio 2022 / 2019) ────────
set "CL_EXE="
for %%E in (Community Professional Enterprise) do (
  for %%Y in (2022 2019) do (
    set "BASE=C:\Program Files\Microsoft Visual Studio\%%Y\%%E\VC\Tools\MSVC"
    if exist "!BASE!" (
      for /d %%V in ("!BASE!\*") do (
        if exist "%%~V\bin\Hostx64\x64\cl.exe" (
          set "CL_EXE=%%~V\bin\Hostx64\x64\cl.exe"
          goto :found_cl
        )
      )
    )
  )
)
:: Fallback: x86 hosted
for %%E in (Community Professional Enterprise) do (
  for %%Y in (2022 2019) do (
    set "BASE=C:\Program Files (x86)\Microsoft Visual Studio\%%Y\%%E\VC\Tools\MSVC"
    if exist "!BASE!" (
      for /d %%V in ("!BASE!\*") do (
        if exist "%%~V\bin\Hostx86\x64\cl.exe" (
          set "CL_EXE=%%~V\bin\Hostx86\x64\cl.exe"
          goto :found_cl
        )
      )
    )
  )
)

echo [ERROR] cl.exe 를 찾지 못했습니다. Visual Studio 가 설치되어 있는지 확인하세요.
exit /b 1

:found_cl
echo [INFO] Compiler : !CL_EXE!

:: ── 2. Compile TCDataGenerator ────────────────────────
set "SRC=TCDataGenerator\TCDataGenerator.cpp"
set "EXE=TCDataGenerator.exe"

echo [INFO] TCDataGenerator 컴파일 중...
"!CL_EXE!" /std:c++20 /utf-8 /EHsc /O2 /nologo ^
  "%SRC%" /Fe:"%EXE%" /Fo:"%~dp0" > compile_log.txt 2>&1

if %ERRORLEVEL% neq 0 (
  echo [ERROR] 컴파일 실패. compile_log.txt 를 확인하세요.
  type compile_log.txt
  exit /b 1
)
del /q compile_log.txt 2>nul
del /q *.obj           2>nul
echo [INFO] 컴파일 완료.

:: ── 3. Generate test data ──────────────────────────────
echo [INFO] 테스트 데이터 생성 중 (10,000 시료 / 5,000 주문)...
"%EXE%" --generate
if %ERRORLEVEL% neq 0 (
  echo [ERROR] 데이터 생성 실패.
  exit /b 1
)

:: ── 4. Run regression tests ────────────────────────────
echo [INFO] Regression 테스트 실행 중...
"%EXE%" --test
set "RESULT=%ERRORLEVEL%"

echo.
if %RESULT% equ 0 (
  echo [PASS] 모든 테스트를 통과했습니다.
) else if %RESULT% equ 1 (
  echo [FAIL] 일부 테스트가 실패했습니다. RegressionTest\results\ 를 확인하세요.
) else (
  echo [ERROR] 테스트 실행 중 오류가 발생했습니다.
)

echo =====================================================
exit /b %RESULT%
