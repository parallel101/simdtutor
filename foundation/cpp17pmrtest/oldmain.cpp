#include "ticktock.h"
#include "memory_resource_inspector.h"
#include <memory_resource>
#include <list>


static char g_buf[65536 * 30];

// newdelete < sync < unsync < monot

int main() {
    std::pmr::unsynchronized_pool_resource mem{};
    {
        std::vector<char> a;
        TICK(vector);
        for (int i = 0; i < 65536; i++) {
            a.push_back(42);
        }
        TOCK(vector);
    }
    {
        std::pmr::vector<char> a{&mem};
        TICK(pmr_unsync_vector);
        for (int i = 0; i < 65536; i++) {
            a.push_back(42);
        }
        TOCK(pmr_unsync_vector);
    }
    {
        std::pmr::monotonic_buffer_resource lmem{&mem};
        std::pmr::vector<char> a{&lmem};
        // prev8 next8 char1 padding7 = node24
        TICK(pmr_monotonic_vector);
        for (int i = 0; i < 65536; i++) {
            a.push_back(42);
        }
        TOCK(pmr_monotonic_vector);
    }
    {
        std::list<char> a;
        // prev8 next8 char1 padding7 = node24
        TICK(list);
        for (int i = 0; i < 65536; i++) {
            a.push_back(42);
        }
        TOCK(list);
    }
    {
        std::pmr::list<char> a{&mem};
        // prev8 next8 char1 padding7 = node24
        TICK(pmr_unsync_list);
        for (int i = 0; i < 65536; i++) {
            a.push_back(42);
        }
        TOCK(pmr_unsync_list);
    }
    {
        std::pmr::monotonic_buffer_resource lmem{&mem};
        std::pmr::list<char> a{&lmem};
        // prev8 next8 char1 padding7 = node24
        TICK(pmr_monotonic_list);
        for (int i = 0; i < 65536; i++) {
            a.push_back(42);
        }
        TOCK(pmr_monotonic_list);
    }













    /* { // C */
    /*     char *p = (char *)malloc(4); */
    /*     free(p); */
    /* } */
    /* { // C++ 没有 allocator */
    /*     char *p = new char[4]; */
    /*     delete[] p; */
    /* } */
    /* { // C++ 有 allocator */
    /*     std::allocator<char> alloc; */
    /*     char *p = alloc.allocate(4); */
    /*     alloc.deallocate(p, 4); */
    /* } */
    /* { // C++ 有 allocator */
    /*     struct my_custom_allocator { */
    /*         char local_buf[1024]; */
    /*  */
    /*         char *allocate(size_t n) { */
    /*             return local_buf; */
    /*         } */
    /*  */
    /*         void deallocate(char *p, size_t n) { */
    /*             // 什么都不做 */
    /*         } */
    /*     }; */
    /*     my_custom_allocator alloc; */
    /*     char *p = alloc.allocate(4); */
    /*     alloc.deallocate(p, 4); */
    /* } */

    return 0;
}


#if 0
static char g_buf[65536 * 30];

struct memory_resource {
    virtual char *do_allocate(size_t n, size_t align) = 0;
};

struct my_memory_resource : memory_resource {
    size_t m_watermark = 0;
    char *m_buf = g_buf;

    char *do_allocate(size_t n, size_t align) override {
        m_watermark = (m_watermark + align - 1) / align * align;
        char *p = m_buf + m_watermark;
        m_watermark += n;
        return p;
    }
};

template <class T>
struct my_custom_allocator {
    memory_resource *m_resource{};

    using value_type = T;

    my_custom_allocator(memory_resource *resource)
    : m_resource(resource) {}

    T *allocate(size_t n) {
        char *p = m_resource->do_allocate(n * sizeof(T), alignof(T));
        return (T *)p;
    }

    void deallocate(T *p, size_t n) {
        // 什么都不做
    }

    my_custom_allocator() = default;

    template <class U>
    constexpr my_custom_allocator(my_custom_allocator<U> const &other) noexcept : m_resource(other.m_resource) {
    }

    constexpr bool operator==(my_custom_allocator const &other) const noexcept {
        return m_resource == other.m_resource;
    }
};
#endif
