# 반도체 시료 생산주문관리 시스템

가상의 반도체 회사 **S-Semi**의 시료(Sample) 생산 주문을 처음부터 끝까지 관리하는 콘솔 애플리케이션입니다.
주문 접수부터 생산, 출고까지의 전체 흐름을 한 곳에서 처리합니다.

---

## 왜 이 시스템이 필요한가요?

엑셀과 메모장으로 주문을 관리하다 보니 이런 문제가 생겼습니다.

> "어, 이 주문 처리됐나요?"  
> "공정 예약을 했는데, 언제 완성되는지 모르겠어요."  
> "재고가 충분한데 왜 추가 공정이 돌아가고 있나요?"

이 시스템은 시료 재고, 주문 상태, 생산 라인을 **한 화면에서 파악**할 수 있도록 만들어졌습니다.

---

## 주요 기능

| 기능 | 설명 |
|------|------|
| 시료 관리 | 시료 등록·목록 조회·이름 검색 |
| 시료 주문 | 고객 주문 예약·접수·취소 |
| 주문 승인/거절 | 생산담당자가 대기 주문을 검토하여 승인 또는 거절 |
| 생산 라인 | 순차 생산(FIFO) 현황 및 대기 큐 확인 |
| 모니터링 | 주문 상태별 현황 + 시료별 재고 상태(여유/부족/고갈) |
| 출고 처리 | 출고 대기 주문을 선택하여 출고 완료 처리 |

---

## 주문 흐름

```
고객 예약(RESERVED)
    ↓ 주문담당자 접수
PENDING
    ├─→ REJECTED   (생산담당자 거절 — 모니터링 제외)
    └─→ CONFIRMED  (생산담당자 승인)
            ↓
        PRODUCING  (생산 라인 진행 중)
            ↓
         RELEASE   (출고 완료)

※ RESERVED / PENDING 단계에서 취소 → CANCELLED
```

---

## 메인 화면

```
============================================
  반도체 시료 생산주문관리 시스템
============================================
[시료 요약]
  등록 시료: 5개  |  대기 주문: 3건  |  확정 주문: 2건  |  출고 완료: 7건

--------------------------------------------
1. 시료 관리
2. 주문 (접수 / 승인 / 거절)
3. 모니터링
4. 출고 처리
5. 생산 라인
0. 종료
--------------------------------------------
선택 >
```

---

## 빌드 방법

**Visual Studio에서 열기 (권장)**
1. `SampleOrderSystem-KyusungPark-19017482.slnx` 파일을 Visual Studio 2022로 엽니다.
2. 빌드 구성을 `Release | x64`로 설정합니다.
3. 상단 메뉴 → **빌드 → 솔루션 다시 빌드** (단축키: `Ctrl + Alt + F7`)

**MSBuild 명령어**
```
msbuild SampleOrderSystem-KyusungPark-19017482.slnx /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal
```

빌드 출력 위치: `x64\Release\SampleOrderSystem-KyusungPark-19017482.exe`

---

## 실행 방법

**Visual Studio에서 실행**
- `Ctrl+F5` (실행)
- 작업 디렉터리가 솔루션 루트로 자동 설정되어 `Database/` 폴더를 정상 인식

**직접 실행**
```
x64\Release\SampleOrderSystem-KyusungPark-19017482.exe
```

> **데이터 영속성**: 프로그램 종료(메뉴 `0`) 시 자동 저장되며, 재실행 시 이전 데이터가 그대로 복원됩니다.

---

## 프로젝트 구조

```
SampleOrderSystem-KyusungPark-19017482/          ← 솔루션 루트 (실행 기준 디렉터리)
├── SampleOrderSystem-KyusungPark-19017482/       ← 소스 코드
│   ├── main.cpp                                  ← 진입점
│   ├── App.h / App.cpp                           ← 메인 메뉴 및 루프
│   ├── Product.h / Product.cpp                   ← 시료 데이터 모델
│   ├── Order.h / Order.cpp                       ← 주문 데이터 모델
│   ├── OrderStatus.h                             ← 주문 상태 enum
│   ├── ProductManager.h / ProductManager.cpp     ← 시료 관리 비즈니스 로직
│   ├── OrderManager.h / OrderManager.cpp         ← 주문 관리 비즈니스 로직
│   ├── ProductionLine.h / ProductionLine.cpp     ← 생산 라인 (FIFO 큐)
│   ├── Monitor.h / Monitor.cpp                   ← 모니터링 대시보드
│   ├── JsonRepository.h / JsonRepository.cpp     ← JSON 기반 데이터 영속성
│   └── PasswordGuard.h                           ← 비밀번호 보호
├── Database/                                     ← 런타임 데이터 (로컬 전용, git 제외)
│   ├── products.json                             ← 시료 목록
│   ├── orders.json                               ← 주문 목록
│   └── production.json                           ← 생산 라인 상태
├── x64/Release/                                  ← 빌드 출력
│   └── SampleOrderSystem-KyusungPark-19017482.exe
├── RegressionTest/                               ← 회귀 테스트
│   └── TCDataGenerator/TCDataGenerator.cpp
├── docs/
│   ├── PRD.md                                    ← 요구사항 문서 인덱스
│   └── PRD_*.md                                  ← 기능별 상세 요구사항
└── README.md
```

---

## 품질 보증

코드 변경이 발생할 때마다 `RegressionTest/TCDataGenerator`가 10,000개의 시료와 5,000개의 주문 데이터를 자동 생성하고, 데이터 무결성·검색 정확성·정렬 정확성을 포함한 15개 항목의 회귀 테스트를 수행합니다.
테스트 결과는 타임스탬프 형식의 리포트로 로컬에만 저장되며, GitHub에는 테스트 소스 코드(`TCDataGenerator.cpp`, `run_regression.bat`)만 관리됩니다.

---

## 기술 스택

- **언어**: C++20
- **플랫폼**: Windows Console Application
- **빌드**: MSBuild (Visual Studio 2022, 툴셋 v145, Release | x64)
- **외부 라이브러리**: 없음 (STL만 사용)

---

## 담당자

- **이름**: KyusungPark (박규성)
- **사번**: 19017482
