#include <mutex>
template <typename T> class Account {
public:
  T balance;
  std::mutex mtx;

private:
  Account(T balance);
  T get_balance();
  T deposit(T amount);
  T withdraw(T amount);
};

template <typename T> T Account<T>::get_balance() { return this->balance; }
template <typename T> T Account<T>::deposit(T amount) {
  mtx.lock();
  balance += amount;
  mtx.unlock();
  return this->balance;
}
template <typename T> T Account<T>::withdraw(T amount) {
  mtx.lock();
  balance -= amount;
  mtx.unlock();
  return this->balance;
}

template <typename T> Account<T>::Account(T balance) {
  this->balance = balance;
}