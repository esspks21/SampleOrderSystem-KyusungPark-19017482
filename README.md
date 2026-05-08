# 반도체 시료 생산주문관리 시스템

가상의 반도체 회사 **S-Semi**의 시료(Sample) 생산 주문을 처음부터 끝까지 관리하는 콘솔 애플리케이션입니다.

---

## 개발 배경

엑셀과 메모장으로 주문을 관리하다 보니 이런 문제가 생겼습니다.

> "어, 이 주문 처리됐나요?"  
> "공정 예약을 했는데, 언제 완성되는지 모르겠어요."  
> "재고가 충분한데 왜 추가 공정이 돌아가고 있나요?"

S-Semi는 시료 재고·주문 상태·생산 라인을 **한 화면에서 파악**하기 위해 이 시스템을 도입했습니다.

---

## 주요 기능

| 메뉴 | 기능 |
|------|------|
| 시료 관리 | 시료 등록·목록 조회(정렬)·이름/ID 검색·수정·삭제 |
| 주문 관리 | 고객 예약(RESERVED) → 접수(PENDING) → 승인/거절 |
| 모니터링 | 주문 상태·재고 현황 실시간 대시보드 (3초 자동 갱신) |
| 출고 내역 | 출고 완료(RELEASE) 목록 조회 |
| 생산 라인 | 생산 현황·대기 큐·진행률 실시간 표시 (3초 자동 갱신) |
| 데이터 초기화 | 관리자 비밀번호 인증 후 전체 초기화 |

---

## 주문·생산 자동화 흐름

```
고객 예약(RESERVED)
    ↓ 주문담당자 접수
PENDING
    ├─→ REJECTED  (생산담당자 거절 — 모니터링 제외)
    └─→ 재고 충분 → 자동 출고 (RELEASE)
        재고 부족 → 생산 라인 자동 등록
                        ↓ [실시간 생산]
                    PRODUCING  →  생산 완료 → 자동 출고 (RELEASE)

※ 생산 공식: 실생산량 = ceil(부족분 / (수율 × 0.9))
※ 총생산시간 = 평균생산시간(min) × 실생산량
※ 프로그램 재시작 시 오프라인 경과 시간만큼 생산 자동 반영
```

---

## 시작하기 (Clone 후 설정)

```bash
# 1. 저장소 clone
git clone https://github.com/esspks21/SampleOrderSystem-KyusungPark-19017482.git

# 2. 로컬 DB 데이터가 git에 추적되지 않도록 설정 (권장)
git update-index --skip-worktree Database/products.csv Database/orders.csv
```

> `Database/products.csv`, `Database/orders.csv`는 빈 초기 파일로 포함됩니다.  
> 프로그램 실행 시 개인 데이터가 로컬에만 누적되며 GitHub에 반영되지 않습니다.

---

## 빌드 방법

**Visual Studio 2022 (권장)**
1. `SampleOrderSystem-KyusungPark-19017482.slnx` 파일을 엽니다.
2. 빌드 구성: `Release | x64`
3. `Ctrl + Shift + B` (솔루션 빌드)

**MSBuild 명령어**
```
msbuild SampleOrderSystem-KyusungPark-19017482.slnx /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal
```

빌드 출력: `x64\Release\SampleOrderSystem-KyusungPark-19017482.exe`

---

## 실행 방법

**Visual Studio에서 실행**  
`Ctrl+F5` — 작업 디렉터리가 솔루션 루트로 자동 설정됩니다.

**직접 실행**  
솔루션 루트(`SampleOrderSystem-KyusungPark-19017482/` 폴더)에서 실행해야 `Database/` 경로가 정상 인식됩니다.
```
x64\Release\SampleOrderSystem-KyusungPark-19017482.exe
```

> **데이터 자동 저장**: 모든 상태 변경(예약·접수·승인 등)마다 즉시 저장됩니다.  
> **재시작 복원**: 프로그램 재시작 시 이전 상태가 완전히 복원되며, 오프라인 중 완료된 생산도 자동 반영됩니다.

---

## 메인 화면

```
  ███   ██   █   █  ███  █   █ █   █  ████
 █     █  █  ██ ██ █     █   █ ██  █ █
  ███  ████  █ █ █  ███  █   █ █ █ █ █  ██
     █ █  █  █   █     █ █   █ █  ██ █   █
  ███  █  █  █   █  ███   ███  █   █  ████

       반도체 시료 생산주문관리 시스템
  ⏱  현재시각 :  2026-05-08  15:00:00  (KST)
─────────────────────────────────────────────
 [시스템 현황]
  시료 : 1000   예약 : 3   대기 : 1   확정 : 0   생산중 : 2   출고 : 47

  1.  시료 관리
  2.  주문 관리
  3.  모니터링
  4.  출고 내역
  5.  생산 라인
  9.  데이터 초기화  (관리자)
  0.  종료
```

---

## 프로젝트 구조

```
SampleOrderSystem-KyusungPark-19017482/        ← 솔루션 루트
├── SampleOrderSystem-KyusungPark-19017482/    ← 소스 코드
│   ├── main.cpp                               ← 진입점
│   ├── App.h / App.cpp                        ← 메인 루프·메뉴
│   ├── Product.h / Product.cpp                ← 시료 모델
│   ├── Order.h / Order.cpp                    ← 주문 모델
│   ├── OrderStatus.h                          ← 주문 상태 enum
│   ├── ProductManager.h / ProductManager.cpp  ← 시료 CRUD
│   ├── OrderManager.h / OrderManager.cpp      ← 주문 처리·생산 동기화
│   ├── ProductionLine.h / ProductionLine.cpp  ← 생산 라인 현황
│   ├── Monitor.h / Monitor.cpp                ← 모니터링 대시보드
│   ├── FileRepository.h / FileRepository.cpp  ← CSV 영속성
│   └── PasswordGuard.h                        ← 관리자 인증
├── Database/                                  ← 런타임 데이터 (로컬 전용)
│   ├── products.csv                           ← 시료 목록
│   └── orders.csv                             ← 주문 목록
├── RegressionTest/                            ← 회귀 테스트
│   ├── TCDataGenerator/TCDataGenerator.cpp   ← 10,000건 테스트 데이터 생성·검증
│   └── run_regression.bat                     ← 테스트 실행 스크립트
├── docs/PRD.md                                ← 요구사항 인덱스
├── .claude/agents/                            ← 코드 품질 에이전트
└── README.md
```

---

## 품질 보증

코드 변경 시마다 `TCDataGenerator`가 10,000개 시료·5,000개 주문 데이터를 자동 생성하고 15개 항목(데이터 무결성·검색 정확도·정렬·참조 무결성 등)의 회귀 테스트를 수행합니다.  
테스트 결과는 타임스탬프 리포트로 로컬에만 저장되며, GitHub에는 테스트 소스 코드만 포함됩니다.

---

## 기술 스택

- **언어**: C++20
- **플랫폼**: Windows Console Application
- **빌드**: MSBuild (Visual Studio 2022, 툴셋 v145, Release | x64)
- **영속성**: CSV 파일 기반 (`Database/products.csv`, `Database/orders.csv`)
- **외부 라이브러리**: 없음 (STL만 사용)

---

## 담당자

- **이름**: KyusungPark (박규성)
- **사번**: 19017482
