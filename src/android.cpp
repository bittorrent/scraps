#if __ANDROID__ && !__clang__

#include "scraps/android.h"

#include <cstdlib>
#include <cwchar>
#include <cerrno>
#include <limits>
#include <stdexcept>

namespace std {

// implementation from libc++

namespace {

template <typename T>
inline void throw_helper(const string& msg) {
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw T(msg);
#else
    printf("%s\n", msg.c_str());
    abort();
#endif
}

inline void throw_from_string_out_of_range(const string& func) { throw_helper<out_of_range>(func + ": out of range"); }

inline void throw_from_string_invalid_arg(const string& func) {
    throw_helper<invalid_argument>(func + ": no conversion");
}

// as_integer

template <typename V, typename S, typename F>
inline V as_integer_helper(const string& func, const S& str, size_t* idx, int base, F f) {
    typename S::value_type* ptr;
    const typename S::value_type* const p = str.c_str();
    typename remove_reference<decltype(errno)>::type errno_save = errno;
    errno = 0;
    V r = f(p, &ptr, base);
    swap(errno, errno_save);
    if (errno_save == ERANGE)
        throw_from_string_out_of_range(func);
    if (ptr == p)
        throw_from_string_invalid_arg(func);
    if (idx)
        *idx = static_cast<size_t>(ptr - p);
    return r;
}

template <typename V, typename S>
inline V as_integer(const string& func, const S& s, size_t* idx, int base);

// string
template <>
inline int as_integer(const string& func, const string& s, size_t* idx, int base) {
    // Use long as no Standard string to integer exists.
    long r = as_integer_helper<long>(func, s, idx, base, strtol);
    if (r < numeric_limits<int>::min() || numeric_limits<int>::max() < r)
        throw_from_string_out_of_range(func);
    return static_cast<int>(r);
}

template <>
inline long as_integer(const string& func, const string& s, size_t* idx, int base) {
    return as_integer_helper<long>(func, s, idx, base, strtol);
}

template <>
inline unsigned long as_integer(const string& func, const string& s, size_t* idx, int base) {
    return as_integer_helper<unsigned long>(func, s, idx, base, strtoul);
}

// wstring
template <>
inline int as_integer(const string& func, const wstring& s, size_t* idx, int base) {
    // Use long as no Stantard string to integer exists.
    long r = as_integer_helper<long>(func, s, idx, base, wcstol);
    if (r < numeric_limits<int>::min() || numeric_limits<int>::max() < r)
        throw_from_string_out_of_range(func);
    return static_cast<int>(r);
}

template <>
inline long as_integer(const string& func, const wstring& s, size_t* idx, int base) {
    return as_integer_helper<long>(func, s, idx, base, wcstol);
}

template <>
inline unsigned long as_integer(const string& func, const wstring& s, size_t* idx, int base) {
    return as_integer_helper<unsigned long>(func, s, idx, base, wcstoul);
}

// as_float

template <typename V, typename S, typename F>
inline V as_float_helper(const string& func, const S& str, size_t* idx, F f) {
    typename S::value_type* ptr;
    const typename S::value_type* const p = str.c_str();
    typename remove_reference<decltype(errno)>::type errno_save = errno;
    errno = 0;
    V r = f(p, &ptr);
    swap(errno, errno_save);
    if (errno_save == ERANGE)
        throw_from_string_out_of_range(func);
    if (ptr == p)
        throw_from_string_invalid_arg(func);
    if (idx)
        *idx = static_cast<size_t>(ptr - p);
    return r;
}

template <typename V, typename S>
inline V as_float(const string& func, const S& s, size_t* idx = nullptr);

} // unnamed namespace

int stoi(const string& str, size_t* idx, int base) { return as_integer<int>("stoi", str, idx, base); }

int stoi(const wstring& str, size_t* idx, int base) { return as_integer<int>("stoi", str, idx, base); }

long stol(const string& str, size_t* idx, int base) { return as_integer<long>("stol", str, idx, base); }

long stol(const wstring& str, size_t* idx, int base) { return as_integer<long>("stol", str, idx, base); }

unsigned long stoul(const string& str, size_t* idx, int base) {
    return as_integer<unsigned long>("stoul", str, idx, base);
}

unsigned long stoul(const wstring& str, size_t* idx, int base) {
    return as_integer<unsigned long>("stoul", str, idx, base);
}

// to_string

namespace {

// as_string

template <typename S, typename P, typename V>
inline S as_string(P sprintf_like, S s, const typename S::value_type* fmt, V a) {
    typedef typename S::size_type size_type;
    size_type available = s.size();
    while (true) {
        int status = sprintf_like(&s[0], available + 1, fmt, a);
        if (status >= 0) {
            size_type used = static_cast<size_type>(status);
            if (used <= available) {
                s.resize(used);
                break;
            }
            available = used; // Assume this is advice of how much space we need.
        } else
            available = available * 2 + 1;
        s.resize(available);
    }
    return s;
}

template <class S, class V, bool = is_floating_point<V>::value>
struct initial_string;

template <class V, bool b>
struct initial_string<string, V, b> {
    string operator()() const {
        string s;
        s.resize(s.capacity());
        return s;
    }
};

template <class V>
struct initial_string<wstring, V, false> {
    wstring operator()() const {
        const size_t n = (numeric_limits<unsigned long long>::digits / 3) +
                         ((numeric_limits<unsigned long long>::digits % 3) != 0) + 1;
        wstring s(n, wchar_t());
        s.resize(s.capacity());
        return s;
    }
};

template <class V>
struct initial_string<wstring, V, true> {
    wstring operator()() const {
        wstring s(20, wchar_t());
        s.resize(s.capacity());
        return s;
    }
};

typedef int (*wide_printf)(wchar_t* __restrict, size_t, const wchar_t* __restrict, ...);

inline wide_printf get_swprintf() {
#ifndef _LIBCPP_MSVCRT
    return swprintf;
#else
    return static_cast<int(__cdecl*)(wchar_t * __restrict, size_t, const wchar_t* __restrict, ...)>(swprintf);
#endif
}

} // unnamed namespace

string to_string(int val) { return as_string(snprintf, initial_string<string, int>()(), "%d", val); }

string to_string(unsigned val) { return as_string(snprintf, initial_string<string, unsigned>()(), "%u", val); }

string to_string(long val) { return as_string(snprintf, initial_string<string, long>()(), "%ld", val); }

string to_string(unsigned long val) {
    return as_string(snprintf, initial_string<string, unsigned long>()(), "%lu", val);
}

string to_string(float val) { return as_string(snprintf, initial_string<string, float>()(), "%f", val); }

string to_string(double val) { return as_string(snprintf, initial_string<string, double>()(), "%f", val); }

string to_string(long double val) { return as_string(snprintf, initial_string<string, long double>()(), "%Lf", val); }

wstring to_wstring(int val) { return as_string(get_swprintf(), initial_string<wstring, int>()(), L"%d", val); }

wstring to_wstring(unsigned val) {
    return as_string(get_swprintf(), initial_string<wstring, unsigned>()(), L"%u", val);
}

wstring to_wstring(long val) { return as_string(get_swprintf(), initial_string<wstring, long>()(), L"%ld", val); }

wstring to_wstring(unsigned long val) {
    return as_string(get_swprintf(), initial_string<wstring, unsigned long>()(), L"%lu", val);
}

} // namespace std

#endif // __ANDROID__
