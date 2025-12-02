#include <iostream>
#include <memory>

int main()
{
    std::unique_ptr<int> ptr = std::make_unique<int>(42);
    std::unique_ptr<int> str = std::make_unique<int>(12);
    processValue(std::move(ptr));
    int mas[] = {1, 2, 3, 4, 5};
    std::shared_ptr<int> a = std::make_shared<int>(mas);
    std::shared_ptr<int> b = mas;
    std::shared_ptr<int> c = mas;
    while (*a != 5)
    {
        std::weak_ptr<int> weakPtr(mas) ;
        if (weakPtr.expired())
        {cout << *a;}
        a++;
    }
    return 0;
}