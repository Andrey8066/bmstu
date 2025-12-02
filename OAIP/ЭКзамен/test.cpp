#include <iostream>
#include <memory>
#include <pqxx/pqxx>
#include <string>

using namespace std;
using namespace pqxx;
class DatabaseConnection {
protected:
  shared_ptr<connection> c;
  shared_ptr<work> w;
  int status;

public:
  DatabaseConnection(string conn) { c = make_shared<connection>(conn); 
  cout << c->connection_string();}
  ~DatabaseConnection() {
    c->close();
  }
  void beginTransaction() {
    cout << 1;
    w = make_shared<work>(*c);

    status = 1;
  }
  void commitTransaction() {
    w.get()->commit();
    status = 2;
  }
  void rollbackTransaction() {
    w.get()->abort();
    status = 3;
    delete w.get();
  }
  void executeNonQuery(string request) { 
    cout << 1;
    w.get()->exec(request); }
  void executeQuery(string request) {
    result r = w.get()->exec(request);
    for (int rownum = 0; rownum < r.size(); ++rownum) {
            for (int colnum = 0; colnum < r.size(); ++colnum) {
                cout << r[rownum][colnum] << "  ";
            }
        cout << endl;
    }
  }
  string getTransactionStatus() {
    switch (status) {
    case 1:
      return "beginned";
    case 2:
      return "commited";
    case 3:
      return "rolledback";
    }
  }
};


int main(){
     std::string conn =
        "dbname=exam user=postgres password=123456 hostaddr=127.0.0.1 "
        "port=5432";

    shared_ptr<DatabaseConnection >d = make_shared<DatabaseConnection>(conn);
    d.get()->beginTransaction();
    d.get()->executeQuery("SELECT * FROM products");
    d.get()->commitTransaction();
}