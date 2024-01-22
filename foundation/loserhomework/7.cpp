#include <format>
#include <vector>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <string>
#include <atomic>

struct MyException : std::exception {
    const char *s;
    MyException(const char *s_) : s(s_) {
    }
    ~MyException() {
    }
    const char *what() const noexcept {
        return s;
    }
};

struct Error {
    virtual std::string What() const noexcept {
        return "Error";
    }
    virtual ~Error() = default;
};

struct HttpError : Error {
    virtual std::string What() const noexcept override {
        return "HTTP 404 ERROR";
    }
};

void test() {
    std::string s;
    throw new MyException("信息");
    throw HttpError();
    throw MyException("信息");
    throw "sss";
}

void catcher(auto test) {
    try {
        test();
    } catch (...) {
        puts("男主：电脑，硬盘，赶紧，rm -rf");
        std::rethrow_exception(std::current_exception());
    }
}

int main() {
    try {
        catcher(test);
    } catch (Error const &e) {
        puts(e.What().c_str());
    } catch (std::exception const &e) {
        puts(e.what());
    } catch (std::exception const *e) {
        puts(e->what());
        delete e;
    } catch (int i) {
        printf("%d\n", i);
    } catch (...) {
        printf("未知错误\n");
    }
    return 0;
}
