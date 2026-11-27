#include <avr/io.h>
#include <compat/twi.h>
#include <avr/interrupt.h>
#include "I2CSlave.h"





uint8_t dataFromGyro[14];

const uint8_t tytul[] = {"GRISHA"};
const uint8_t wartosc[] = {"BUNIN"};




#define I2C_ADDR 0x10 // адрес ведомого устройства

volatile uint8_t data;
uint8_t counter;

void I2C_received(uint8_t received_data)
{
  data = received_data;
  sprintf(dataFromGyro,"%3d",data);

	if(counter%2 ==1)
 		LCDGotoXY(8,1);
  	else
		LCDGotoXY(11,1);
		
	LCDstring(dataFromGyro,3);
	  counter++;

}

void I2C_requested()
{
  I2C_transmitByte(data);
}

void setup()
{

  I2C_setCallbacks(I2C_received, I2C_requested);


  // init I2C, тут посылаем адрес
  I2C_init(I2C_ADDR);
}


int main(void)
{

	setup();


	// Enable Global Interrupt\, тут настройка экрана, выводим на экран
	

	LCDinit();
	LCDcursorOFF();
	LCDGotoXY(0,0);
	LCDstring(tytul,6);
	LCDGotoXY(0,1);
	LCDstring(wartosc,5);

	sei();

	while (1) {

	}

}
