# 구현 계획 (PLAN)

## 프로젝트 정보

- **프로젝트명**: SampleOrderSystem — 반도체 시료 생산주문관리
- **담당자**: KyusungPark (사번: 19017482)
- **언어**: C++ (C++20)

---

## 역할별 흐름

고객 시료 요청 → 주문담당자 주문서 등록·관리 → 생산담당자 승인/거절 → (승인 시) 생산 후 출고

---

## 시스템 개요

**운영 방식**: 콘솔 기반, 담당자가 직접 명령 입력

**생산 라인**
- 공장에서 시료 하나를 생산하는 설비 흐름
- 하나의 생산 라인은 시료를 하나씩 순차 생산
- 주문이 들어온 시료에 대해서만 생산 진행

**주요 기능**
- 시료(상품) 등록·관리
- 주문 접수·처리 (승인/거절/취소)

---

## 주문 상태 흐름

```
RESERVED → PENDING → CONFIRMED → PRODUCING → RELEASE
                   → REJECTED
         → CANCELLED  (RESERVED / PENDING 단계에서만 가능)
```

---

## 클래스 설계

### OrderStatus (주문 상태 enum)

```cpp
enum class OrderStatus {
    RESERVED,   // 고객 예약, 접수 대기
    PENDING,    // 주문 담당자 접수 완료, 승인 대기
    CONFIRMED,  // 생산 담당자 승인, 생산 큐 대기
    PRODUCING,  // 생산 라인 진행 중
    RELEASE,    // 출고 완료
    REJECTED,   // 생산 담당자 거절
    CANCELLED   // 주문 취소
};
```

### Product (시료)

```cpp
class Product {
    int id;
    std::string name;
    int stock;
};
```

### Order (주문)

```cpp
class Order {
    int id;
    std::string customerName;
    int productId;
    int quantity;
    OrderStatus status;
};
```

### ProductManager (시료 관리자)

```cpp
class ProductManager {
    std::vector<Product> products;
public:
    void addProduct(...);
    void listProducts() const;
    std::vector<Product> searchByName(const std::string& keyword) const;
    bool updateStock(int id, int delta);
    Product* findById(int id);
};
```

### OrderManager (주문 관리자)

```cpp
class OrderManager {
    std::vector<Order> orders;
    ProductManager& productManager;
public:
    bool reserveOrder(...);              // RESERVED 생성
    bool acceptOrder(int id);           // RESERVED → PENDING
    bool approveOrder(int id);          // PENDING  → CONFIRMED
    bool rejectOrder(int id);           // PENDING  → REJECTED
    bool cancelOrder(int id);           // RESERVED / PENDING → CANCELLED
    void listOrders() const;
    void listOrdersByStatus(OrderStatus status) const;
};
```

### ProductionLine (생산 라인)

```cpp
class ProductionLine {
    std::queue<int> waitingQueue;       // CONFIRMED 주문 ID 대기 큐
    std::optional<int> producingOrderId;// 현재 생산 중인 주문 ID
    OrderManager& orderManager;
public:
    void enqueue(int orderId);          // CONFIRMED → 큐 추가
    void startNext();                   // 큐 선두 → PRODUCING
    void completeProducing();           // PRODUCING → RELEASE
    void showStatus() const;
};
```

### App (진입점 및 메뉴)

```cpp
class App {
    ProductManager productManager;
    OrderManager orderManager;
    ProductionLine productionLine;
public:
    void run();
    void showMainMenu();
};
```

---

## 파일 구조

```
SampleOrderSystem-KyusungPark-19017482/
├── SampleOrderSystem-KyusungPark-19017482/
│   ├── main.cpp
│   ├── Product.h / Product.cpp
│   ├── Order.h / Order.cpp
│   ├── ProductManager.h / ProductManager.cpp
│   ├── OrderManager.h / OrderManager.cpp
│   ├── ProductionLine.h / ProductionLine.cpp
│   ├── App.h / App.cpp
│   └── FileRepository.h / FileRepository.cpp   # 데이터 영속성
└── tests/
    ├── test_product.cpp
    ├── test_order.cpp
    ├── test_production.cpp
    └── test_scenario.cpp
```

---

## 개발 Phase 구성

> **규칙**: 각 Phase는 리뷰 승인 후 다음 Phase로 진행한다.
> Phase 완료 조건 — 구현 + 테스트 스크립트 통과 + 에이전트 리뷰 완료

---

### Phase 1 — 프로젝트 기반 구조

**목표**: 빌드 가능한 뼈대 + 핵심 데이터 모델 완성

- [ ] 소스 디렉터리 및 파일 구조 생성
- [ ] `OrderStatus` enum 구현
- [ ] `Product` 클래스 구현 (멤버 변수, getter/setter)
- [ ] `Order` 클래스 구현 (멤버 변수, getter/setter)
- [ ] `main.cpp` 진입점 작성 (빌드 통과 확인)
- [ ] 테스트 하네스 기본 틀 (`tests/` 디렉터리 생성)

**리뷰 기준**: 빌드 성공 / Product·Order 객체 생성 및 출력 확인

---

### Phase 2 — 시료 관리 (ProductManager)

**목표**: 시료 등록·조회·검색 기능 완성

- [ ] `ProductManager` 구현: 시료 등록, 전체 목록 조회, 이름 검색
- [ ] 콘솔 UI: 시료 관리 서브메뉴 연결
- [ ] 테스트 스크립트: 등록→조회→이름 검색 시나리오 PASS 확인

**리뷰 기준**: 시료 등록·목록·검색 기능이 PRD 명세와 일치

---

### Phase 3 — 시료 주문 (예약 · 접수 · 취소)

**목표**: 주문 생명주기 전반부(RESERVED → PENDING → CANCELLED) 완성

- [ ] `OrderManager` 구현: `reserveOrder`, `acceptOrder`, `cancelOrder`
- [ ] 콘솔 UI: 주문 예약·접수·취소 서브메뉴 연결
- [ ] 테스트 스크립트: 예약→접수 / 예약→취소 / 접수→취소 시나리오 PASS 확인

**리뷰 기준**: 상태 전환이 PRD 흐름과 정확히 일치, 잘못된 상태에서 취소 시 오류 처리 확인

---

### Phase 4 — 주문 승인 / 거절

**목표**: 생산담당자 승인·거절 처리 완성

- [ ] `OrderManager` 구현: `approveOrder`(PENDING → CONFIRMED), `rejectOrder`(PENDING → REJECTED)
- [ ] 승인 시 `ProductionLine` 대기 큐에 자동 추가
- [ ] 콘솔 UI: 승인·거절 서브메뉴 연결
- [ ] 테스트 스크립트: 승인→큐 추가 / 거절→목록 제외 시나리오 PASS 확인

**리뷰 기준**: PENDING 이외 상태에서 승인·거절 시 거부 처리 확인

---

### Phase 5 — 생산 라인 (ProductionLine)

**목표**: 순차 생산 처리 완성

- [ ] `ProductionLine` 구현: 대기 큐, `startNext`(CONFIRMED → PRODUCING), `completeProducing`(PRODUCING → RELEASE)
- [ ] 콘솔 UI: 생산 라인 현황 화면 (현재 생산 중 / 대기 큐 목록)
- [ ] 테스트 스크립트: 큐 순서 보장 / 하나씩 처리 시나리오 PASS 확인

**리뷰 기준**: 생산 라인이 동시에 하나의 주문만 처리함을 확인

---

### Phase 6 — 출고 처리

**목표**: 출고 실행 및 재고 차감 완성

- [ ] 출고 실행: `CONFIRMED` 상태 주문 선택 → `RELEASE` 전환
- [ ] 출고 시 시료 재고 차감 (`ProductManager.updateStock`)
- [ ] 콘솔 UI: 출고 처리 서브메뉴 연결
- [ ] 테스트 스크립트: 출고 후 재고 감소 / 재고 부족 시 출고 거부 시나리오 PASS 확인

**리뷰 기준**: 재고 충분 여부 검증 후 출고 처리, 상태 전환 정확성 확인

---

### Phase 7 — 모니터링

**목표**: 주문 현황 및 재고 상태 대시보드 완성

- [ ] 상태별 주문 수 집계 출력 (RESERVED / CONFIRMED / PRODUCING / RELEASE, REJECTED 제외)
- [ ] 시료별 재고 현황 출력: 여유 / 부족 / 고갈 상태 표기
- [ ] 콘솔 UI: 모니터링 서브메뉴 연결
- [ ] 테스트 스크립트: 집계 수치 정확성 검증 시나리오 PASS 확인

**리뷰 기준**: PRD 모니터링 명세의 모든 항목이 화면에 출력됨을 확인

---

### Phase 8 — 데이터 영속성

**목표**: 프로그램 재시작 후에도 데이터 유지

- [ ] `FileRepository` 구현: 시료·주문·생산 큐 데이터를 파일(CSV 또는 텍스트)로 저장·로드
- [ ] 프로그램 시작 시 자동 로드, 종료 시 자동 저장
- [ ] 테스트 스크립트: 저장→재실행→데이터 일치 확인 시나리오 PASS

**리뷰 기준**: 종료 후 재실행해도 이전 상태가 완전히 복원됨을 확인

---

### Phase 9 — 콘솔 UI 통합 (App)

**목표**: 전체 메뉴 통합 및 메인 화면 완성

- [ ] `App` 클래스: 메인 루프, 전체 서브메뉴 연결
- [ ] 메인 화면 상단 요약 정보 (등록 시료 수 / 상태별 주문 수)
- [ ] 잘못된 입력(범위 외 번호, 비숫자 입력) 처리
- [ ] 통합 테스트: 예약→접수→승인→생산→출고 전체 흐름 E2E 시나리오 PASS

**리뷰 기준**: PRD 메인 메뉴 구조와 일치, 모든 메뉴 진입·복귀 정상 동작 확인

---

### Phase 10 — 최종 검증 및 정리

**목표**: 코드 품질 최종 점검 및 제출 준비

- [ ] `cpp-optimizer` 에이전트 전체 소스 리뷰 및 개선 적용
- [ ] `clean-code-reviewer` 에이전트 전체 소스 리뷰 및 개선 적용
- [ ] `cpp-security-guard` 에이전트 전체 소스 리뷰 및 개선 적용
- [ ] 엣지 케이스 최종 확인 (재고 0, 빈 큐, 잘못된 ID 입력 등)
- [ ] 커밋 이력 정리 및 문서(CLAUDE.md·PRD) 최종 업데이트

**리뷰 기준**: 모든 에이전트 리뷰 이슈 해결 완료, 빌드 경고 0건

---

## 주요 제약사항

- STL(`vector`, `queue`, `string`, `algorithm`, `optional`)만 사용, 외부 라이브러리 금지
- 전역 변수 사용 금지, 모든 상태는 클래스 내부에서 관리
- 메모리 관리: 스마트 포인터(`unique_ptr`) 또는 값 타입 우선 사용
- 데이터 영속성: 파일 기반(CSV 또는 텍스트), 외부 DB 사용 금지
