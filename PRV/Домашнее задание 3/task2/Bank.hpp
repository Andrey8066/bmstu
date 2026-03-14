#include "Account.hpp"
#include <condition_variable>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

template <typename T> class Bank {
public:
  std::vector<Account<T>> accounts;
  std::condition_variable cv;
  std::mutex cvMutex;

private:
  Bank(size_t n);
  bool transfer(int from, int to, T amount);
  inline bool doTransfer(int from, int to, T amount);
};

template <typename T>
inline bool Bank<T>::doTransfer(int from, int to, T amount) {
  Account<T> fromAccount = &accounts.at(from);
  Account<T> toAccount = &accounts.at(to);

  std::unique_lock<std::mutex> cvLock(cvMutex);

  cv.wait(cvLock, [&] { return fromAccount.get_balance >= amount; });

  fromAccount.withdraw(amount);
  toAccount.deposit(amount);
}

template <typename T> bool Bank<T>::transfer(int from, int to, T amount) {
  std::thread t(&Bank<T>::doTransfer, this, from, to, amount);

  t.detach();
}

template <typename T> Bank<T>::Bank(size_t n) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<T> dist(1, 10000000);

  for (int i = 0; i < n; i++) {
    accounts.emplace_back(Account<T>(dist));
  }
}