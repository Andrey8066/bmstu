#include <iostream>
#include <memory>

int main()
{
    std::shared_ptr<int> ptr = std::make_shared<int>(42);
    std::shared_ptr<int> str = ptr;
    std::cout << ptr.use_count();
    return 0;
}