#include "openssl/sha.h"
#include <climits>
#include <cstddef>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
string sha256(const string &input) { // Создание sha256 хэша по заданной строке
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256(reinterpret_cast<const unsigned char *>(input.c_str()), input.size(),
         hash);
  ostringstream oss;
  for (unsigned char c : hash) {
    oss << hex << setw(2) << setfill('0') << static_cast<int>(c);
  }
  return oss.str();
}

vector<string>
split(string line) { // Разделение записи на даннные для структуры
  vector<string> res;
  int i = 0;
  string data;
  while (line[i] != ';') {
    data += line[i];
    i++;
  }
  i++;
  res.push_back(data);
  data.clear();
  while (line[i] != ';') {
    data += line[i];
    i++;
  }
  i++;
  res.push_back(data);
  data.clear();
  while (line[i] != ';') {
    data += line[i];
    i++;
  }
  i++;
  res.push_back(data);
  data.clear();
  while (line[i] != ';') {
    data += line[i];
    i++;
  }
  i++;
  res.push_back(data);
  data.clear();
  while (line[i] != ';') {
    data += line[i];
    i++;
  }
  res.push_back(data);
  return res;
}

class Block { // Класс блока
protected:
  int index;              // Индекс блока
  string previousHash;    // Хэш предыдущего блока
  string transactionData; // Название транзакции
  time_t timeCreated;     // Время создания блока
  int nonce;         // Переменная для просчета хэшей
  int dificulty = 8; // Сложность вычисления блока
  string hash;       // Хэш блока

public:
  Block(int i, string tD, string pH) { // Коннструктор класса
    index = i;
    previousHash = pH;
    transactionData = tD;
    timeCreated = time(0);
    hash = calculateHash();
    mineBlock();
  }

  Block(vector<string> blockData) // Конструктор класса
      : index(stoi(blockData[0])), previousHash(blockData[1]),
        transactionData(blockData[2]), timeCreated((stol(blockData[3]))),
        hash(blockData[4]){};

  void mineBlock() { // Майнинг хэша данного блока
    string target(dificulty, '0');
    cout << "Майнинг хэша блока №" << index << endl;
    while (hash.substr(0, dificulty) != target) {
      nonce++;
      hash = calculateHash();
      cout << hash << endl;
    }
  }

  string calculateHash() { // Вычистление хэша
    timeCreated = time(0);
    string toHash = to_string(index) + transactionData + previousHash +
                    to_string(timeCreated) + to_string(nonce);
    return sha256(toHash);
  }
  // Get функции
  string get_hash() { return hash; }
  string get_previousHash() { return previousHash; }
  int get_index() { return index; }
  time_t get_timeCreated() { return timeCreated; }
  string get_blockData() {
    string res = to_string(index) + ";" + (previousHash) + ";" +
                 transactionData + ";" + to_string(timeCreated) + ";" + hash +
                 ";";
    return res;
  }
  
  void display_data() { // Метод для вывода данных о блоке
    cout << setw(3) << left << index << setw(10) << left << timeCreated
         << "    " << hash << endl;
  }
};

class BlockChain {
private:
  vector<Block> chain; // Вектор блоков

  Block createGenesisBlock() { return Block(0, "Genesis block", "0"); } // Метод для создания начального блока цепочки

public:
  BlockChain() { chain.push_back(createGenesisBlock()); } // Конструктор класс

  BlockChain(string path) { // Конструктор класса
    ifstream file(path);
    string line;
    vector<string> blockData;
    while (getline(file, line)) {
      blockData = split(line);
      chain.push_back(Block(blockData));
    }
  }

  void addBlock(string data) { // Метод для добавления блока в цепочку
    Block &lastBlock = chain.back();
    chain.push_back(Block(lastBlock.get_index() + 1, data, lastBlock.get_hash()));
  }

  bool isChainValid() { //  Метод для проверки правильности цепочки
    for (size_t i = 1; i < chain.size(); i++) {
      Block &currentBlock = chain[i];
      Block &previousBlock = chain[i - 1];
      if (currentBlock.get_previousHash() != previousBlock.get_hash()) {
        cout << currentBlock.get_previousHash() << "    "
             << previousBlock.get_hash() << endl;
        return false;
      }
    }
    return true;
  }

  void printBlocks() { // Метод для вывода данных о каждом блоке цепочки
    for (size_t i = 0; i < chain.size(); i++) {
      chain[i].display_data();
    }
  }

  void saveChain() { // Метод для сохранения цепочки
    ofstream chainFile(
        (sha256(to_string(chain[0].get_timeCreated())) + ".chain"));
    for (size_t i = 0; i < chain.size(); i++) {
      cout << chain[i].get_blockData() << endl;
      chainFile << chain[i].get_blockData() << endl;
    }
  }
};

int main() {
  BlockChain *chain = new BlockChain();

  while (1) {
    cout << "1. Создать цепочку\n2. Проверить цепочку\n3. Добавить блок\n4. "
            "Вывести всю цепочку\n5. Сохранить цепочку\n6. Открыть цепочку"
         << endl; // Вывод меню для работы с блокчейном
    char input;
    cin >> input;
    if (input == '1') {
      *chain = BlockChain();
    } else if (input == '2') {
      cout << "Проверка цепочки на Валидность" << endl;
      if (chain->isChainValid()) // Проверка правильности цепочки
        cout << "Цепочка верна" << endl;
      else
        cout << "Цепочка неверна" << endl;
    } else if (input == '3') {
      string data;
      cout << "Введите название блока" << endl; 
      cin.ignore();
      getline(cin, data);
      chain->addBlock(data); // Добавление нового блока в цепочку
    } else if (input == '4') {
      cout << "Блоки в данной цепочке" << endl;
      chain->printBlocks();
    } else if (input == '5') {
      chain->saveChain(); // Сохранение цепочки
      cout << "Цепочка сохранена" << endl;
    } else if (input == '6') {
      string path;
      cout << "Введите название цепочки" << endl;
      cin >> path;
      *chain = BlockChain(path); // Открытие сохраненной цепочки
    }
  }
  return 0;
}