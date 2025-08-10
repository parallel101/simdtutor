#pragma once

#include <atomic>
#include <cstddef>


template <class T, size_t N, bool AtomicWait = false>
struct alignas(64) spsc_ring
{
    static_assert(std::atomic<T *>::is_always_lock_free, "atomic pointer not lock-free");

    alignas(64) std::atomic<T *> m_write_pos;
    alignas(64) std::atomic<T *> m_read_pos;

    alignas(64) struct {
        T *m_write_pos_cached;
        T *m_read_pos_local;
    };

    alignas(64) struct {
        T *m_read_pos_cached;
        T *m_write_pos_local;
    };

    alignas(64) T m_ring_buffer[N];

    spsc_ring()
        : m_write_pos{m_ring_buffer}
        , m_read_pos{m_ring_buffer}
        , m_write_pos_cached{m_ring_buffer}
        , m_read_pos_local{m_ring_buffer}
        , m_read_pos_cached{m_ring_buffer}
        , m_write_pos_local{m_ring_buffer}
    {}

    void write(const T *input_first, const T *input_last)
    {
        T *write_pos_local = this->m_write_pos_local;
        T *read_pos_cached = this->m_read_pos_cached;
        while (input_first != input_last) {
            T *next_write_pos = write_pos_local;
            ++next_write_pos;
            if (next_write_pos == m_ring_buffer + N) {
                next_write_pos = m_ring_buffer;
            }
            if (next_write_pos == read_pos_cached) {
                while (true) {
                    read_pos_cached = this->m_read_pos.load(std::memory_order_acquire);
                    if (next_write_pos != read_pos_cached) {
                        break;
                    }
                    this->m_write_pos.store(write_pos_local, std::memory_order_release);
#if __cpp_lib_atomic_wait
                    if constexpr (AtomicWait) {
                        this->m_write_pos.notify_one();
                        this->m_read_pos.wait(read_pos_cached, std::memory_order_acquire);
                    }
#endif
                }
            }
            *write_pos_local = *input_first;
            ++input_first;
            write_pos_local = next_write_pos;
        }
        this->m_write_pos.store(write_pos_local, std::memory_order_release);
#if __cpp_lib_atomic_wait
        if constexpr (AtomicWait) {
            this->m_write_pos.notify_one();
        }
#endif
        this->m_write_pos_local = write_pos_local;
        this->m_read_pos_cached = read_pos_cached;
    }

    void read(T *output_first, T *output_last)
    {
        T *read_pos_local = this->m_read_pos_local;
        T *write_pos_cached = this->m_write_pos_cached;
        while (output_first != output_last) {
            if (read_pos_local == write_pos_cached) {
                while (true) {
                    write_pos_cached = this->m_write_pos.load(std::memory_order_acquire);
                    if (read_pos_local != write_pos_cached) {
                        break;
                    }
                    this->m_read_pos.store(read_pos_local, std::memory_order_release);
#if __cpp_lib_atomic_wait
                    if constexpr (AtomicWait) {
                        this->m_read_pos.notify_one();
                        this->m_write_pos.wait(write_pos_cached, std::memory_order_acquire);
                    }
#endif
                }
            }
            *output_first = *read_pos_local;
            ++output_first;
            ++read_pos_local;
            if (read_pos_local == m_ring_buffer + N) {
                read_pos_local = m_ring_buffer;
            }
        }
        this->m_read_pos.store(read_pos_local, std::memory_order_release);
#if __cpp_lib_atomic_wait
        if constexpr (AtomicWait) {
            this->m_read_pos.notify_one();
        }
#endif
        this->m_read_pos_local = read_pos_local;
        this->m_write_pos_cached = write_pos_cached;
    }

    const T *write_some(const T *input_first, const T *input_last)
    {
        T *write_pos_local = this->m_write_pos_local;
        T *read_pos_cached = this->m_read_pos_cached;
        while (input_first != input_last) {
            T *next_write_pos = write_pos_local;
            ++next_write_pos;
            if (next_write_pos == m_ring_buffer + N) {
                next_write_pos = m_ring_buffer;
            }
            if (next_write_pos == read_pos_cached) {
                read_pos_cached = this->m_read_pos.load(std::memory_order_acquire);
                if (next_write_pos != read_pos_cached) {
                    break;
                }
            }
            *write_pos_local = *input_first;
            ++input_first;
            write_pos_local = next_write_pos;
        }
        this->m_write_pos.store(write_pos_local, std::memory_order_release);
#if __cpp_lib_atomic_wait
        if constexpr (AtomicWait) {
            this->m_write_pos.notify_one();
        }
#endif
        this->m_write_pos_local = write_pos_local;
        this->m_read_pos_cached = read_pos_cached;
        return input_first;
    }

    T *read_some(T *output_first, T *output_last)
    {
        T *read_pos_local = this->m_read_pos_local;
        T *write_pos_cached = this->m_write_pos_cached;
        while (output_first != output_last) {
            if (read_pos_local == write_pos_cached) {
                write_pos_cached = this->m_write_pos.load(std::memory_order_acquire);
                if (read_pos_local != write_pos_cached) {
                    break;
                }
            }
            *output_first = *read_pos_local;
            ++output_first;
            ++read_pos_local;
            if (read_pos_local == m_ring_buffer + N) {
                read_pos_local = m_ring_buffer;
            }
        }
        this->m_read_pos.store(read_pos_local, std::memory_order_release);
#if __cpp_lib_atomic_wait
        if constexpr (AtomicWait) {
            this->m_read_pos.notify_one();
        }
#endif
        this->m_read_pos_local = read_pos_local;
        this->m_write_pos_cached = write_pos_cached;
        return output_first;
    }
};
