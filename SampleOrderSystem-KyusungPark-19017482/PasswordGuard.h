#pragma once
#include <string>
using namespace std;

// ─────────────────────────────────────────────────────────
// 비밀번호 검증 모듈
//   원본 비밀번호는 이 파일 어디에도 평문으로 저장되지 않습니다.
//   인코딩: token[i] = password[i] XOR key(i)
//           key(i)   = (i * 29 + 17) & 0xFF
// ─────────────────────────────────────────────────────────
namespace PasswordGuard {

    inline bool verify(const string& input) {
        static const uint8_t TOKEN[] = {0x23, 0x18, 0x7B, 0x5D, 0xB5, 0x94};
        constexpr int LEN = sizeof(TOKEN);
        if ((int)input.size() != LEN) return false;
        for (int i = 0; i < LEN; i++) {
            uint8_t key = static_cast<uint8_t>((i * 29 + 17) & 0xFF);
            if ((TOKEN[i] ^ key) != static_cast<uint8_t>(input[i]))
                return false;
        }
        return true;
    }

    inline const char* hint() {
        return "CRA 교육 시작날짜  (YYMMDD 6자리)";
    }
}
