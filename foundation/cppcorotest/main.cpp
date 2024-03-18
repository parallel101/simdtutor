#include <chrono>
#include <coroutine>
#include <cstdio>
#include <deque>
#include <functional>
#include <optional>
#include <span>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "debug.hpp"

using namespace std::literals;

auto repr(std::ostream &os, std::coroutine_handle<> h) {
    os << "coroutine@" << h.address();
}

struct scheduler {
    void add(std::coroutine_handle<> h) {
        std::unique_lock lck(mtx);
        ready.push_back(h);
        cv.notify_one();
    }

    void run() {
        std::unique_lock lck(mtx);
        cv.wait(lck, [this] { return !ready.empty(); });
        auto h = ready.front();
        ready.pop_front();
        lck.unlock();
        h.resume();
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    std::deque<std::coroutine_handle<>> ready;
};

scheduler &get_scheduler() {
    static scheduler inst;
    return inst;
}

struct task {
    struct promise_type {
        auto initial_suspend() noexcept {
            return std::suspend_always{};
        }

        auto final_suspend() noexcept {
            struct awaiter {
                bool await_ready() noexcept {
                    return false;
                }

                void await_resume() noexcept {}

                std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> curr) noexcept {
                    auto &p = curr.promise();
                    bool go_back = false;
                    if (p.counter)
                        go_back = p.counter->fetch_sub(1, std::memory_order_acq_rel) == 1;
                    if (go_back && p.last)
                        return p.last;
                    else
                        return std::noop_coroutine();
                }
            };
            return awaiter{};
        }

        void unhandled_exception() {
            throw;
        }

        void return_void() {
        }

        task get_return_object() {
            return task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::coroutine_handle<> last;
        std::atomic_size_t *counter;
    };

    task() noexcept : curr(nullptr) {}

    task(std::coroutine_handle<promise_type> curr) : curr(curr) {}

    task(task &&that) noexcept : curr(that.curr) {
        that.curr = nullptr;
    }

    task &operator=(task &&that) noexcept {
        if (curr)
            curr.destroy();
        curr = that.curr;
        that.curr = nullptr;
        return *this;
    }

    ~task() {
        if (curr)
            curr.destroy();
    }

    bool is_done() const noexcept {
        return curr.done();
    }

    std::coroutine_handle<promise_type> get() const noexcept {
        return curr;
    }

    operator std::coroutine_handle<>() const noexcept {
        return curr;
    }

    bool await_ready() const {
        return false;
    }

    void await_resume() const {
    }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> last) const {
        curr.promise().last = last;
        return curr;
    }

private:
    std::coroutine_handle<promise_type> curr;
};

struct sleep_for {
    sleep_for(std::chrono::system_clock::duration d) : duration(d) {}

    bool await_ready() {
        return duration.count() <= 0;
    }

    void await_resume() {
    }

    void await_suspend(std::coroutine_handle<> last) noexcept {
        std::thread([last, duration = duration] {
            std::this_thread::sleep_for(duration);
            get_scheduler().add(last);
        }).detach();
    }

private:
    std::chrono::system_clock::duration duration;
};

struct wait_all {
    wait_all(std::initializer_list<task> ts)
        : wait_all(std::span<task const>{ts.begin(), ts.size()})
    {}

    wait_all(std::span<task const> ts)
        : counter{ts.size()}, ts(ts)
    {}

    bool await_ready() {
        return false;
    }

    void await_resume() {
    }

    void await_suspend(std::coroutine_handle<> last) {
        for (auto &t: ts) {
            auto h = t.get();
            h.promise().counter = &counter;
            h.promise().last = last;
            get_scheduler().add(h);
        }
    }

private:
    std::atomic_size_t counter;
    std::span<task const> ts;
};


task test1() {
    debug(), "test1";
    co_await sleep_for(1s);
    debug(), "test1 done";
    co_return;
}

task test2() {
    debug(), "test2";
    co_await sleep_for(2s);
    debug(), "test2 done";
    co_return;
}

task test() {
    debug(), "test";
    co_await wait_all(std::array{test1(), test2()});
    co_return;
}

int main() {
    auto t = test();
    get_scheduler().add(t);
    while (!t.is_done())
        get_scheduler().run();
    return 0;
}
