#include <fstream>
#include <iostream>
#include <limits>
#include <ostream>
#include <string>

using namespace std;

class Light { // Класс отвечающий за свет
protected:
  bool lightState = false;

public:
  void lightOn() { lightState = true; }
  void lightOff() { lightState = false; }
  bool getLightState() {
    cout << "Light: ";
    if (lightState)
      cout << "On" << endl;
    else
      cout << "Off" << endl;
    return lightState;
  }
};

class Term { // Класс отвечающий за термостат
protected:
  float temperature = 20;

public:
  void setTemperature(float t) {
    temperature = t;
    getTemperature();
  }
  float getTemperature() {
    cout << "Temperature: " << temperature << "C" << endl;
    return temperature;
  }
};

class SafeSystem { // Класс отвечающий за систему безопасности
protected:
  bool safeSystemState = false;

public:
  void safeSystemOn() { safeSystemState = true; }
  void safeSystemOff() { safeSystemState = false; }
  bool getSafeSystemState() {
    cout << "Security System: ";
    if (safeSystemState)
      cout << "Armed" << endl;
    else
      cout << "Disarmed" << endl;
    return safeSystemState;
  }
};

class Сurtains { // Класс отвечающий за систему безопасности
protected:
  bool curtainsState = false;

public:
  void curtainsOn() { curtainsState = true; }
  void curtainsOff() { curtainsState = false; }
  bool getCurtainsState() {
    cout << "Curtains: ";
    if (curtainsState)
      cout << "Raised" << endl;
    else
      cout << "Omitted" << endl;
    return curtainsState;
  }
};

class Conditioner { // Класс отвечающий за занавески
protected:
  bool conditionerState = false;

public:
  void conditionerOn() { conditionerState = true; }
  void conditionerOff() { conditionerState = false; }
  bool getConditionerState() {
    cout << "Air Conditioner: ";
    if (conditionerState)
      cout << "On" << endl;
    else
      cout << "Off" << endl;
    return conditionerState;
  }
};

class SmartHome { // Класс отвечающий за объединение классов
protected:
  Light ligthts;
  Term terms;
  Сurtains curt;
  SafeSystem safe;
  Conditioner conditioner;
  string StateFile = "./state.conf";

public:
  void controlLight(bool state) { // Управление светом
    if (state) {
      cout << state;
      ligthts.lightOn();
    } else
      ligthts.lightOff();
    ligthts.getLightState();
  }
  void controlCurtains(bool state) { // Управление занавесками
    if (state)
      curt.curtainsOn();
    else
      curt.curtainsOff();
    curt.getCurtainsState();
  }
  void controlSafe(bool state) { // Управление системой безопасности
    if (state)
      safe.safeSystemOn();
    else
      safe.safeSystemOff();
    safe.getSafeSystemState();
  }
  void controlConditioner(bool state) { // Управление кондиционером
    if (state)
      conditioner.conditionerOn();
    else
      conditioner.conditionerOff();
    conditioner.getConditionerState();
  }
  void setTemperature(float temperature) { terms.setTemperature(temperature); }  // Управление температурой
  void getTemp() {  // Получение температуры
    float temperature;
    cout << "Введите желаемую температуру: ";
    cin >> temperature;
    setTemperature(temperature);
  }
  void loadState() { // Загрузка состояний из файла
    ifstream statesFile(StateFile);
    string line;
    cout << "State loaded from file." << endl << "Current State:" << endl;
    getline(statesFile, line);
    int ptr = 0;
    while (line[ptr] != ':') {
      ptr++;
    }
    ptr++;
    controlLight(static_cast<bool>(line[ptr] - 48));
    getline(statesFile, line);
    ptr = 0;
    while (line[ptr] != ':') {
      ptr++;
    }
    ptr++;
    controlSafe(static_cast<bool>(line[ptr] - 48));
    getline(statesFile, line);
    ptr = 0;
    while (line[ptr] != ':') {
      ptr++;
    }
    ptr++;
    controlCurtains(static_cast<bool>(line[ptr] - 48));
    getline(statesFile, line);
    ptr = 0;
    while (line[ptr] != ':') {
      ptr++;
    }
    ptr++;
    controlConditioner(static_cast<bool>(line[ptr] - 48));
    getline(statesFile, line);
    ptr = 0;
    while (line[ptr] != ':') {
      ptr++;
    }
    ptr++;
    string condState;
    while (line[ptr] != ';') {
      condState += line[ptr];
      ptr++;
    }
    setTemperature(stof(condState));
  }
  void saveState() { //  // Сохранение состояния умного дома
    ofstream file(StateFile);
    file << "Состояние освещения:" << ligthts.getLightState() << ";" << endl
         << "Состояние системы безопасности:" << safe.getSafeSystemState()
         << ";" << endl
         << "Состояние занавесок:" << curt.getCurtainsState() << ";" << endl
         << "Состояние кондиционера:" << conditioner.getConditionerState()
         << ";" << endl
         << "Темпереатура в комнате:" << terms.getTemperature() << ";" << endl;
    exit(0);
  }
  void menu() { // Меню управления
    char c;
    cout << "Choose an action" << endl
         << "1. Turn on light" << endl
         << "2. Turn off light" << endl
         << "3. Set temperature" << endl
         << "4. Arm security system" << endl
         << "5. Disarm security system" << endl
         << "6. Raise curtains" << endl
         << "7. Lower curtains" << endl
         << "8. Turn on air conditioner" << endl
         << "9. Turn off air conditioner" << endl
         << "0. Exit" << endl;
    cin >> c;
    switch (c) {
    case '1':
      controlLight(1);
      break;
    case '2':
      controlLight(0);
      break;
    case '3':
      getTemp();
      break;
    case '4':
      controlSafe(1);
      break;
    case '5':
      controlSafe(0);
      break;
    case '6':
      controlCurtains(1);
      break;
    case '7':
      controlCurtains(0);
      break;
    case '8':
      controlConditioner(1);
      break;
    case '9':
      controlConditioner(0);
      break;
    case '0':
      saveState();
      break;
    }
  }
};

int main() {
  SmartHome s;
  s.loadState();
  while (true)
    s.menu();
  return 0;
}