# SampleOrderSystem-KyusungPark-19017482

@docs/PLAN.md
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

## Agentic Engineering

기능 명세(PRD)를 충족하는 고품질 코드를 개발하기 위해 Agentic Engineering 방식을 도입한다.
Claude가 코드를 작성·수정할 때 아래 5가지 주안점을 항상 준수한다.

### 1. 문서 관리 (CLAUDE.md · PRD.md)

- `CLAUDE.md`는 프로젝트 규칙·에이전트·코딩 규칙의 단일 진실 공급원(Single Source of Truth)이다.
- 기능 추가·변경 시 `docs/PRD.md`와 관련 문서를 먼저 확인하고, 문서와 구현이 일치하도록 유지한다.
- 구현 도중 요구사항이 바뀌면 코드와 함께 PRD도 즉시 업데이트한다.

### 2. Harness 도입

- 각 클래스·모듈은 독립 실행 가능한 테스트 하네스(간단한 `main` 드라이버 또는 별도 테스트 소스)를 함께 작성한다.
- 하네스는 실제 빌드 산출물과 분리하여 `tests/` 디렉터리에 배치한다.
- 하네스를 통해 단위 기능이 PRD 명세를 충족하는지 빌드·실행으로 직접 검증한다.

### 3. Test Script

- 기능 구현 후 시나리오 기반 테스트 스크립트를 작성한다.
- 정상 흐름(Happy Path)과 경계값·오류 입력(Edge Case) 시나리오를 모두 포함한다.
- 테스트 결과는 PASS / FAIL 형태로 콘솔에 출력한다.

### 4. Clean Code

- 코드 수정 시 `clean-code-reviewer` 에이전트와 `cpp-optimizer` 에이전트를 실행한다.
- 네이밍·단일 책임·매직 넘버 제거·중복 제거 원칙을 항상 적용한다.
- 시간복잡도와 런타임 성능을 검토하고 불필요한 복사·반복 연산을 제거한다.

### 5. Commit 이력

- 커밋 메시지는 **변경 이유(Why)** 중심으로 작성한다. 무엇을 바꿨는지(What)는 diff가 대신한다.
- 커밋 단위는 하나의 논리적 변경(기능 단위·버그 수정·리팩터링)으로 유지한다.
- 커밋 전 반드시 빌드가 성공하는 상태를 확인한다.
- 형식: `<type>: <제목>` — type은 `feat` / `fix` / `refactor` / `test` / `docs` / `chore` 중 선택

---

## 에이전트

### cpp-optimizer

`.cpp` 또는 `.h` 파일을 생성하거나 수정할 때마다 **반드시** `cpp-optimizer` 에이전트를 실행한다.

- 수정된 코드의 시간복잡도(Big-O)를 분석하고 개선점을 제시한다.
- 런타임 성능(불필요한 복사, 루프 내 반복 연산, 컨테이너 선택 등)을 검토한다.
- 문제가 발견되면 즉시 수정하여 코드 품질을 향상시킨다.
- 동작(로직)은 변경하지 않고 성능만 개선한다.

### clean-code-reviewer

`.cpp` 또는 `.h` 파일을 생성하거나 수정할 때마다 **반드시** `clean-code-reviewer` 에이전트를 실행한다.

- **네이밍**: 변수·함수·클래스 이름이 의도를 명확히 드러내는지 확인하고, 모호하거나 축약된 이름을 개선한다.
- **함수 설계(SRP)**: 함수가 단일 책임을 지키는지 확인하고, 20줄 초과 또는 "and"가 필요한 함수는 분리한다.
- **주석**: 코드로 설명 가능한 불필요한 주석을 제거하고, 반드시 필요한 경우에만 간결하게 남긴다.
- **매직 넘버**: 의미 없는 리터럴 숫자·문자열을 `constexpr` 상수 또는 enum class로 교체한다.
- **중복 제거(DRY)**: 2회 이상 반복되는 로직을 공통 함수 또는 템플릿으로 추출한다.
- **구조**: 중첩 깊이 3 초과 시 guard clause 또는 함수 추출로 개선한다.
- 이슈는 `[LOCATION] / [PRINCIPLE] / [ISSUE] / [FIX]` 형식으로 보고하며, 문제 발견 시 즉시 수정한다.
- 동작(로직)은 변경하지 않고 가독성과 구조만 개선한다.

### cpp-security-guard

`.cpp` 또는 `.h` 파일을 생성하거나 수정할 때마다 **반드시** `cpp-security-guard` 에이전트를 실행한다.

- **입력 검증**: 사용자 입력과 외부 데이터에 대해 범위·타입·길이 검증이 수행되는지 확인한다.
- **메모리 안전성**: 버퍼 오버플로우, use-after-free, double-free 등 메모리 취약점을 탐지하고 스마트 포인터로 개선한다.
- **정수 안전성**: 부호 불일치, 정수 오버플로우, 인덱스 연산 위험을 탐지한다.
- **데이터 무결성**: 공유 가변 상태의 불일치, 오류 경로에서의 리소스 누수·부분 쓰기 손상을 점검한다.
- **방어적 코딩**: 오류 무시, 하드코딩된 민감 정보, 내부 상태 노출 등을 탐지한다.
- 심각도(CRITICAL / HIGH / MEDIUM / LOW)를 함께 보고하며, 문제 발견 시 즉시 수정한다.
- 동작(로직)은 변경하지 않고 보안성과 안전성만 개선한다.

## Git 무시 항목

- `.vs/` — Visual Studio 로컬 캐시
- `*.user` — 사용자별 로컬 설정
- `*.db` — IntelliSense 데이터베이스
- `x64/`, `Win32/` — 빌드 출력 디렉터리
