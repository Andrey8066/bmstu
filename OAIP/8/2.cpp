#include <iostream>
#include <memory>

int main()
{
    std::shared_ptr<int> ptr = std::make_shared<int>(42);
    std::weak_ptr<int> weakPtr(ptr) ;
    std::cout << weakPtr.expired();
    std::cout << *weakPtr.lock();
    return 0;
}