#include <iostream>

int main() {
    std::string s = "我是中文";
    std::u32string u32s = U"我是中文";
    for (char c : s) {
        std::cout << std::hex << std::uppercase << (unsigned int)(unsigned char)c << '|';
    }
    std::cout << s << std::endl;
    for (char32_t c32 : u32s) {
        std::cout << std::hex << std::uppercase << (unsigned int)c32 << '|';
    }
    for (char32_t c32 : u32s) {
        if (0x0 <= c32 && c32 <= 0x7F) {
            std::cout << (char)c32;
        } else if (0x80 <= c32 && c32 <= 0x7FF) {
        }
    }
    std::cout << std::endl;
    return 0;
}
