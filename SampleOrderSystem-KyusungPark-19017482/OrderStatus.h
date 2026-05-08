#pragma once
#include <string>
using namespace std;

enum class OrderStatus {
    RESERVED,   // 고객 예약, 접수 대기
    PENDING,    // 주문 담당자 접수 완료, 승인 대기
    CONFIRMED,  // 생산 담당자 승인, 생산 큐 대기
    PRODUCING,  // 생산 라인 진행 중
    RELEASE,    // 출고 완료
    REJECTED,   // 생산 담당자 거절
    CANCELLED   // 주문 취소
};

inline string statusToString(OrderStatus status) {
    switch (status) {
    case OrderStatus::RESERVED:  return "RESERVED";
    case OrderStatus::PENDING:   return "PENDING";
    case OrderStatus::CONFIRMED: return "CONFIRMED";
    case OrderStatus::PRODUCING: return "PRODUCING";
    case OrderStatus::RELEASE:   return "RELEASE";
    case OrderStatus::REJECTED:  return "REJECTED";
    case OrderStatus::CANCELLED: return "CANCELLED";
    default:                     return "UNKNOWN";
    }
}
