#include "ticktock.h"
#include <memory_resource>
#include "memory_resource_inspector.h"
#include <vector>
#include <list>
#include <deque>

// newdelete < sync < unsync < monot

int main() {
    memory_resource_inspector mem{std::pmr::new_delete_resource()};

    std::pmr::vector<int> s{&mem};
    for (int i = 0; i < 4096; i++) {
        s.push_back(i);
    }
}
