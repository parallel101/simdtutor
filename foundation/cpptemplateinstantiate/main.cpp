#include "other.h"
#include "printer.h"

int main() {
    add_object(1);
    add_object(3.14);
    add_object("Hello, World!");
    add_object("Hello, World!");
    add_object("Hello, World!");
    add_object("Hello, World!");
    add_object(3.14);
    add_object("Hello, World!");
    add_object("Hello, World!");
    print_objects();
    printer() , 1 , 3.14 , "Hello, World!" , "Hello, World!" , "Hello, World!" , "Hello, World!" , 3.14 , "Hello, World!" , "Hello, World!";
    return 0;
}
