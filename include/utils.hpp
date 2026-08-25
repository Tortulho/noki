#ifndef UTILS
#define UTILS

inline bool isSkippable(const char& chr) {
    return (chr == ' ' || chr == '\n' || chr == '\t');
}

#endif