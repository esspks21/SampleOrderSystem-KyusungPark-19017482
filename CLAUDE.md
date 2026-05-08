# SampleOrderSystem-KyusungPark-19017482

@PLAN.md
@docs/PRD.md

## 프로젝트 개요

**과제 주제**: 반도체 시료 생산주문관리 개인과제

반도체 시료의 생산 주문을 등록·조회·취소·관리하는 콘솔 애플리케이션입니다. Visual Studio C++ 기반으로 구현합니다.

> **이 프로젝트는 C++ 언어로 작성됩니다.** 모든 소스 코드는 C++ (C++20 표준)로 구현하며, 다른 언어는 사용하지 않습니다.

- **사번**: 19017482
- **이름**: KyusungPark (박규성)

## 기술 스택

- **언어**: C++ (C++20 표준)
- **플랫폼**: Windows (Win32 / x64)
- **빌드 시스템**: MSBuild (Visual Studio 2019/2022, 툴셋 v145)
- **문자셋**: Unicode
- **출력 타입**: Console Application

## 빌드 구성

| 구성 | 플랫폼 | 설명 |
|------|--------|------|
| Debug | Win32 / x64 | 디버그 정보 포함, 최적화 없음 |
| Release | Win32 / x64 | 전체 프로그램 최적화, 함수 수준 링킹 |

## 프로젝트 구조

```
SampleOrderSystem-KyusungPark-19017482/
├── CLAUDE.md
├── SampleOrderSystem-KyusungPark-19017482.slnx   # Visual Studio 솔루션 파일
└── SampleOrderSystem-KyusungPark-19017482/
    ├── SampleOrderSystem-KyusungPark-19017482.vcxproj          # 프로젝트 파일
    └── SampleOrderSystem-KyusungPark-19017482.vcxproj.filters  # 소스 필터 정의
```

소스 파일 필터 구성:
- **소스 파일**: `.cpp`, `.c`, `.cxx` 등
- **헤더 파일**: `.h`, `.hpp`, `.hxx` 등
- **리소스 파일**: `.rc`, `.ico`, `.bmp` 등

## 빌드 방법

Visual Studio에서 `.slnx` 파일을 열고 빌드하거나, MSBuild를 사용합니다.

```
msbuild SampleOrderSystem-KyusungPark-19017482.slnx /p:Configuration=Debug /p:Platform=x64
```

## 코딩 규칙

- C++20 표준 기능 사용 가능
- SDL 보안 검사 활성화 (SDLCheck)
- 경고 수준 Level3 이상 유지
- 한글 문자열 처리 시 Unicode 설정 활용

## 에이전트

### cpp-optimizer

`.cpp` 또는 `.h` 파일을 생성하거나 수정할 때마다 **반드시** `cpp-optimizer` 에이전트를 실행한다.

- 수정된 코드의 시간복잡도(Big-O)를 분석하고 개선점을 제시한다.
- 런타임 성능(불필요한 복사, 루프 내 반복 연산, 컨테이너 선택 등)을 검토한다.
- 문제가 발견되면 즉시 수정하여 코드 품질을 향상시킨다.
- 동작(로직)은 변경하지 않고 성능만 개선한다.

## Git 무시 항목

- `.vs/` — Visual Studio 로컬 캐시
- `*.user` — 사용자별 로컬 설정
- `*.db` — IntelliSense 데이터베이스
- `x64/`, `Win32/` — 빌드 출력 디렉터리
