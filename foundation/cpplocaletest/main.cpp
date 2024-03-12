#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <ranges>

void hexdump(std::ranges::input_range auto const &range) {
    using T = std::ranges::range_value_t<decltype(range)>;
    bool add_space = false;
    std::wstring s;
    for (auto c: range) {
        if (add_space) {
            std::wcout << ' ';
        } else {
            add_space = true;
        }
        std::wcout << std::right << std::hex << std::setw(2 * sizeof(T)) << std::setfill(L'0');
        std::wcout << (std::uint64_t)(std::make_unsigned_t<T>)c;
        s.push_back((wchar_t)(std::make_unsigned_t<T>)c);
    }
    std::wcout << "\t|";
    std::wcout << s;
    std::wcout << "|\n";
}

int main() {
    std::locale::global(std::locale("fr_FR.ISO-8859-1"));
    hexdump(L"好"); // 0000597D = 好 (UTF-32)
    hexdump("å¥½"); // E5 A5 BD = 好 (UTF-8)
    hexdump("¥¥¥"); // A5 A5 A5 = ??? (UTF-8)
    hexdump(u8"å¥½"); // C3 A5 C2 A5 C2 BD = å¥½ (UTF-8)
    return 0;
}
