//
//  MyUtil.hpp
//  cpp-network-http
//
//  Created by ChrisLam on 09/01/2024.
//

#ifndef MyUtil_hpp
#define MyUtil_hpp
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include <exception>
#include <algorithm>
#include <filesystem>

#define FMT_HEADER_ONLY 1
#include "../fmt/include/fmt/core.h"
#include "../fmt/include/fmt/color.h"

#define MY_ASSERT(x) my_assert(x, #x, __LINE__, __FILE__)

#define TEST(EXPR)                                                                  \
do                                                                              \
{                                                                               \
printf("[%s] Line %3d: %s\n", ((EXPR) ? " OK " : "FAIL"), __LINE__, #EXPR); \
} while (0) //-------------------

#define dump_var(X)                        \
do                                     \
{                                      \
std::cout << #X ": " << X << "\n"; \
} while (false)

inline void my_assert(bool x, const char* msg, int lineNumber, const char* filename) {
    assert(x);
    if (!x) printf("%s", msg);
}


class NonCopyable {
private:
    NonCopyable(const NonCopyable&) = delete;
    void operator=(const NonCopyable&) = delete;
public:
    NonCopyable() = default;
    NonCopyable(NonCopyable&&) = default;
    void operator=(NonCopyable&&) {};
};

class MyError : std::exception {
public:
    MyError(const char* msg) {
        printf("[ERR][%s]: %s\n", msg, strerror(errno));
    }
};


class MyUtil {
public:
//    static std::string getLocalCurrentWorkingDir() {
//        return std::filesystem::current_path().string();
//    }
    template<class T>
    static void appendVector(std::vector<T>& v0, const std::vector<T>& v1) {
        v0.insert(v0.end(), v1.begin(), v1.end());
    }
    
    static void toRelativeUrl(std::string& url) {
        const char* p = strstr(url.data(), "://");
        if (!p) return;
        p = strstr(p + 3, "/");
        url.assign(p);
    }
    
    static bool isStartsWith(const char* str, const char* substr){
        auto* p = strstr(str, substr);
        return p == str;
    }
    
    static const char* trim(const char* c_str){
        if (!c_str) return nullptr;
        auto* p = c_str;
        
        // trim sp or tab
        while (*p == ' ' || *p == '\t') p++;
        return p;
    }
    
    static const char* getToken(std::string& tokenBuff, const char* inputStr, char sep){
        if (!inputStr) return nullptr;
        
        tokenBuff.clear();
        
        const char* p = inputStr;
        
        // trim space
        p = trim(p);
        
        const char* q = p;
        
        while (true) {
            if (*q && *q == sep) {
                tokenBuff.assign(p, q);
                return q;
            }
            q++;
        }
        return nullptr;
    }
    
    static const char* getLine(std::string& outBuff, const char* inputStr, const char* lineEndToken = "\r\n") {
        outBuff.clear();
        auto* p = strstr(inputStr, lineEndToken);
        
        if (!p) return nullptr;
        
        outBuff.assign(inputStr, p);
        
        return p + strlen(lineEndToken);
        
    }
    
    template<typename T>
    static void remove_unorder(std::vector<T>& vec, T& item){
        
        if (vec.size() <= 0){            
            MY_ASSERT(false);
            return;
        }
        
        if (&item != &vec.back()) {
            std::swap(item, vec.back());
        }
        vec.resize(vec.size() - 1);
    }
    
    template<class InputIterator, class T>
    static T* find_value(InputIterator first, InputIterator last, const T& val)
    {
        while (first != last) {
            if (*first == val)
                return &*first;
            ++first;
        }
        return nullptr;
    }
    
};
#endif /* MyUtil_hpp */
