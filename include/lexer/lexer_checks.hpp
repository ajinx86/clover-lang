#pragma once

namespace checks {

    static inline bool isalpha(char ch) {
        return ((ch >= 'a' && ch <= 'z') ||
                (ch >= 'A' && ch <= 'Z'));
    }

    static inline bool isalnum(char ch) {
        return ((ch >= '0' && ch <= '9') ||
                (ch >= 'a' && ch <= 'z') ||
                (ch >= 'A' && ch <= 'Z'));
    }

    static inline bool isname(char ch) {
        return ((ch >= 'a' && ch <= 'z') ||
                (ch >= 'A' && ch <= 'Z') ||
                (ch == '_'));
    };

    static inline bool isdigit(char ch) {
        return (ch >= '0' && ch <= '9');
    }

    static inline bool isxdigit(char ch) {
        return ((ch >= '0' && ch <= '9') ||
                (ch >= 'a' && ch <= 'f') ||
                (ch >= 'A' && ch <= 'F'));
    };

}
