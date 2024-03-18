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
        ready.push_front(h);
        cv.notify_one();
    }

    void batched_add(std::ranges::input_range auto &&hs) {
        std::unique_lock lck(mtx);
        for (auto const &h: hs) {
            ready.push_front(h);
        }
        cv.notify_all();
    }

    void run_until_done(std::coroutine_handle<> h) {
        add(h);
        while (!h.done()) {
            run();
        }
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

struct base_promise {
    std::coroutine_handle<> last;
    std::atomic_size_t *counter;
    std::size_t index;
};

template <class T = void>
struct basic_promise;

template <>
struct basic_promise<void> : base_promise {
    auto initial_suspend() noexcept {
        return std::suspend_always{};
    }

    struct final_awaiter {
        bool await_ready() noexcept {
            return false;
        }

        void await_resume() noexcept {}

        std::coroutine_handle<> await_suspend(std::coroutine_handle<basic_promise> curr) noexcept {
            auto &p = curr.promise();
            bool jump = true;
            if (p.counter) {
                if (!p.index) {
                    jump = p.counter->fetch_sub(1, std::memory_order_acq_rel) == 1;
                } else {
                    std::size_t expect = 0;
                    jump = p.counter->compare_exchange_strong(expect, p.index, std::memory_order_acq_rel);
                }
            }
            if (jump && p.last)
                return p.last;
            else
                return std::noop_coroutine();
        }
    };

    auto final_suspend() noexcept {
        return final_awaiter{};
    }

    void unhandled_exception() {
        throw;
    }

    void return_void() {
    }

    std::coroutine_handle<basic_promise> get_return_object() {
        return std::coroutine_handle<basic_promise>::from_promise(*this);
    }
};

template <class T> requires (!std::same_as<void, T>)
struct basic_promise<T> : base_promise {
    auto initial_suspend() noexcept {
        return std::suspend_always{};
    }

    struct final_awaiter {
        bool await_ready() noexcept {
            return false;
        }

        void await_resume() noexcept {}

        std::coroutine_handle<> await_suspend(std::coroutine_handle<basic_promise> curr) noexcept {
            auto &p = curr.promise();
            bool jump = true;
            if (p.counter) {
                if (!p.index) {
                    jump = p.counter->fetch_sub(1, std::memory_order_acq_rel) == 1;
                } else {
                    std::size_t expect = 0;
                    jump = p.counter->compare_exchange_strong(expect, p.index, std::memory_order_acq_rel);
                }
            }
            if (jump && p.last)
                return p.last;
            else
                return std::noop_coroutine();
        }
    };

    auto final_suspend() noexcept {
        return final_awaiter{};
    }

    void unhandled_exception() {
        throw;
    }

    void return_value(T value) {
        result.emplace(std::move(value));
    }

    std::coroutine_handle<basic_promise> get_return_object() {
        return std::coroutine_handle<basic_promise>::from_promise(*this);
    }

    T get_result() {
        return std::move(result.value());
    }

    std::optional<T> result;
};

template <class P = void>
struct basic_task;

template <>
struct basic_task<void> {
    basic_task() noexcept : curr(nullptr) {}

    basic_task(std::coroutine_handle<> curr) noexcept : curr(curr) {}

    basic_task(basic_task &&that) noexcept : curr(that.curr) {
        that.curr = nullptr;
    }

    basic_task &operator=(basic_task &&that) noexcept {
        if (curr)
            curr.destroy();
        curr = that.curr;
        that.curr = nullptr;
        return *this;
    }

    ~basic_task() {
        if (curr)
            curr.destroy();
    }

    std::coroutine_handle<> get() const noexcept {
        return curr;
    }

    operator std::coroutine_handle<>() const noexcept {
        return curr;
    }

private:
    std::coroutine_handle<> curr;
};

template <class P> requires (!std::same_as<void, P>)
struct basic_task<P> {
    using promise_type = P;

    basic_task() noexcept : curr(nullptr) {}

    basic_task(std::coroutine_handle<promise_type> curr) noexcept : curr(curr) {}

    basic_task(basic_task &&that) noexcept : curr(that.curr) {
        that.curr = nullptr;
    }

    basic_task &operator=(basic_task &&that) noexcept {
        if (curr)
            curr.destroy();
        curr = that.curr;
        that.curr = nullptr;
        return *this;
    }

    ~basic_task() {
        if (curr)
            curr.destroy();
    }

    operator basic_task<void>() const noexcept {
        return std::coroutine_handle<>{get()};
    }

    bool await_ready() const {
        return false;
    }

    auto await_resume() const {
        if constexpr (!std::same_as<void, P> && !std::same_as<basic_promise<>, P>) {
            return get().promise().get_result();
        }
    }

    std::coroutine_handle<promise_type> await_suspend(std::coroutine_handle<> last) const {
        curr.promise().last = last;
        return curr;
    }

    std::coroutine_handle<promise_type> get() const noexcept {
        return curr;
    }

    operator std::coroutine_handle<>() const noexcept {
        return curr;
    }

private:
    std::coroutine_handle<promise_type> curr;
};

struct awaiter_duration {
    awaiter_duration(std::chrono::system_clock::duration d) : duration(d) {}

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

template <class T = void>
struct task : basic_task<basic_promise<T>> {
    using basic_task<basic_promise<T>>::basic_task;
};

template <class Rep, class Period>
task<> sleep_for(std::chrono::duration<Rep, Period> duration) {
    co_await awaiter_duration(std::move(duration));
}

struct awaiter_count {
    awaiter_count(std::span<std::coroutine_handle<basic_promise<>> const> ts)
        : awaiter_count(std::move(ts), ts.size())
    {}

    awaiter_count(std::span<std::coroutine_handle<basic_promise<>> const> ts, std::size_t count)
        : counter{count}, ts(std::move(ts))
    {}

    bool await_ready() {
        return counter.load(std::memory_order_acquire) == 0;
    }

    void await_resume() {
    }

    void await_suspend(std::coroutine_handle<> last) {
        for (auto &h: ts) {
            h.promise().last = last;
            h.promise().counter = &counter;
            h.promise().index = 0;
            get_scheduler().add(h);
        }
    }

private:
    std::atomic_size_t counter;
    std::span<std::coroutine_handle<basic_promise<>> const> ts;
};

struct awaiter_index {
    awaiter_index(std::span<std::coroutine_handle<basic_promise<>> const> ts)
        : counter{0}, ts(std::move(ts))
    {}

    bool await_ready() {
        return counter.load(std::memory_order_acquire) != 0;
    }

    std::size_t await_resume() {
        return counter.load(std::memory_order_relaxed) - 1;
    }

    void await_suspend(std::coroutine_handle<> last) {
        std::size_t index = 1;
        for (auto &h: ts) {
            h.promise().last = last;
            h.promise().counter = &counter;
            h.promise().index = index++;
            get_scheduler().add(h);
        }
    }

private:
    std::atomic_size_t counter;
    std::span<std::coroutine_handle<basic_promise<>> const> ts;
};

template <class ...Ts> requires (sizeof...(Ts) != 0 && !(std::same_as<Ts, void> || ...))
task<std::tuple<Ts...>> when_all(task<Ts> const &...ts) {
    using array_type = std::array<std::coroutine_handle<basic_promise<>>, sizeof...(Ts)>;
    array_type tasks{ts.get()...};
    co_await awaiter_count(tasks);
    std::tuple<basic_promise<Ts> *...> promises(&ts.get().promise()...);
    co_return [&]<std::size_t ...Is> (std::index_sequence<Is...>) {
        return std::tuple<Ts...>(std::get<Is>(promises)->get_result()...);
    }(std::make_index_sequence<sizeof...(Ts)>{});
}

template <class ...Ts> requires (sizeof...(Ts) != 0 && (std::same_as<Ts, void> || ...))
task<> when_all(task<Ts> const &...ts) {
    using array_type = std::array<std::coroutine_handle<basic_promise<>>, sizeof...(Ts)>;
    array_type tasks{ts.get()...};
    co_await awaiter_count(tasks);
}

template <class ...Ts> requires (sizeof...(Ts) != 0 && !(std::same_as<Ts, void> || ...))
task<std::variant<Ts...>> when_any(task<Ts> const &...ts) {
    using array_type = std::array<std::coroutine_handle<basic_promise<>>, sizeof...(Ts)>;
    array_type tasks{ts.get()...};
    auto i = co_await awaiter_index(tasks);
    std::tuple<basic_promise<Ts> *...> promises(&ts.get().promise()...);
    co_return [&]<std::size_t ...Is> (std::index_sequence<Is...>) {
        std::optional<std::variant<Ts...>> res;
        ([&] {
            if (i == Is) {
                res = std::variant<Ts...>(std::in_place_index<Is>, std::get<Is>(promises)->get_result());
            }
        }, ...);
        return res.value();
    }(std::make_index_sequence<sizeof...(Ts)>{});
}

template <class ...Ts> requires (sizeof...(Ts) != 0 && (std::same_as<Ts, void> || ...))
task<std::size_t> when_any(task<Ts> const &...ts) {
    using array_type = std::array<std::coroutine_handle<basic_promise<>>, sizeof...(Ts)>;
    array_type tasks{ts.get()...};
    auto i = co_await awaiter_index(tasks);
    co_return i;
}

task<> test1() {
    debug(), "test1";
    co_await sleep_for(1s);
    debug(), "test1 done";
    co_return;
}

task<> test2() {
    debug(), "test2";
    co_await when_any(sleep_for(2s), sleep_for(10s));
    debug(), "test2 done";
    co_return;
}

task<> test() {
    debug(), "test";
    co_await when_all(test1(), test2());
    co_return;
}

int main() {
    auto t = test();
    get_scheduler().run_until_done(t);
    return 0;
}
