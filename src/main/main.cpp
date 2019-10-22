#include <iostream>
#include "test.h"

int main() {
    std::string testString = helloWorldMessage();
    
    std::cout << "Hello World!\n";
    std::cout << "A custom hello world message: " << testString;
    return 0;
}
