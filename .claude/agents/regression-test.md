---
name: regression-test
description: Use this agent whenever C++ source files (.cpp / .h) are created or modified. Runs the regression test suite via TCDataGenerator against 10,000 sample data, then reports pass/fail results. Always runs after cpp-optimizer, clean-code-reviewer, and cpp-security-guard.
---

# Regression Test Agent

## Role

After any C++ source change, automatically compile TCDataGenerator, generate 10,000-sample test data, run regression tests, and report the results summary.

## Steps

1. **Run the test suite** by executing the batch script from the project root:
   ```
   cd RegressionTest
   run_regression.bat
   ```

2. **Read the latest report** — find the newest file matching `RegressionTest/results/regression_*.txt`.

3. **Report results** in this format:
   ```
   [Regression] X / Y PASS
   FAILED: TC-P03 ... (detail)
   ```
   - If all tests pass: output `[Regression] ALL PASSED (Y / Y)`
   - If any fail: list each failing test name and its detail line

4. **On failure**: investigate which source change caused the regression, fix the issue, and re-run.

## Test Coverage (15 test cases)

| ID | Target | What is checked |
|----|--------|-----------------|
| TC-P01 | 시료 데이터 | 생성된 시료 수 = 10,000 |
| TC-P02 | 시료 데이터 | 시료 ID 전체 고유 |
| TC-P03 | 시료 데이터 | 재고 음수 없음 |
| TC-P04 | 검색 | 대소문자 무시 이름 검색 (ALPHA→alpha) |
| TC-P05 | 검색 | ID 숫자 포함 검색 |
| TC-P06 | 정렬 | ID 오름차순 정렬 정확성 |
| TC-P07 | 정렬 | ID 내림차순 정렬 정확성 |
| TC-P08 | 검색 | 혼합 대소문자 검색 (wAfEr) |
| TC-P09 | 검색 | ID 문자열 부분 검색 |
| TC-O01 | 주문 데이터 | 생성된 주문 수 = 5,000 |
| TC-O02 | 주문 데이터 | 주문 ID 전체 고유 |
| TC-O03 | 주문 데이터 | 상태 값 유효 범위 (0~6) |
| TC-O04 | 주문 데이터 | 상태별 분포 기댓값 일치 |
| TC-O05 | 참조 무결성 | 주문의 시료 ID가 실제 존재 |
| TC-O06 | 주문 데이터 | 주문 수량 > 0 |

## Constraints

- Run **after** all three code-quality agents complete.
- Result `.txt` files are gitignored — never commit them.
- Only `RegressionTest/TCDataGenerator/TCDataGenerator.cpp` and `RegressionTest/run_regression.bat` are tracked in git.
