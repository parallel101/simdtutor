#include <concepts>
#include <cstdio>

struct Dog {
    void speak() {
        puts("汪汪");
    }
};

struct Cat {
    void speak() {
        puts("喵喵");
    }
};

struct Human {
    void speak() {
        puts("我是人类");
    }

    void learn() {
        puts("正在播放小彭老师课程");
    }
};

struct Robot {
    void speak() {
        puts("我是机器人");
    }

    void learn() {
        puts("我是一只智能机器人，我具有机器学习功能");
    }
};

void educate(Human animal) {
    animal.speak();
    animal.learn();
}

void educate(Robot animal) {
    animal.speak();
    animal.learn();
}

void educate(Dog animal) {
    animal.speak();
    puts("该动物不支持学习");
}

void educate(Cat animal) {
    animal.speak();
    puts("该动物不支持学习");
}

int main() {
    Human student;
    Dog petDog;
    Robot smartRobot;
    puts("人类：");
    educate(student);
    puts("狗狗：");
    educate(petDog);
    puts("机器人：");
    educate(smartRobot);
    return 0;
}
