#pragma once

#include <string>

// android is super dumb

#if __ANDROID__ && !__clang__

namespace std {

string to_string(int val);
string to_string(unsigned val);
string to_string(long val);
string to_string(unsigned long val);
string to_string(long long val);
string to_string(unsigned long long val);
string to_string(float val);
string to_string(double val);
string to_string(long double val);

int                stoi  (const string& str, size_t* idx = 0, int base = 10);
long               stol  (const string& str, size_t* idx = 0, int base = 10);
unsigned long      stoul (const string& str, size_t* idx = 0, int base = 10);
long long          stoll (const string& str, size_t* idx = 0, int base = 10);
unsigned long long stoull(const string& str, size_t* idx = 0, int base = 10);

} // namespace std

#endif
