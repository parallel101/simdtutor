#include "debug.hpp"
#include <string>

struct English {
};

struct Chinese {
};

struct Teacher {
    std::string name;

    Teacher(English) {
        name = "peng sir";
    }

    Teacher(Chinese) {
        name = "小彭老师";
    }
};

int main() {
    auto engTeacher = Teacher(English());
    auto chnTeacher = Teacher(Chinese());
}
