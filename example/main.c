
#include <iom64.h>
#include <ina90.h>
#include "SysEeprom.h"

#define __EEPROM_SIZE__     2048

typedef struct
{
  char Data1;
  int Data2;
  float Data3;
}tag_Var;
tag_Var Var;

unsigned char Array[10];

tag_EepCommonConfig EepConfig;
tag_EepControl EepVar, EepArray;
char SaveEepFlag;

tU8 HalEepromWrite(tU16 Addr, tU8 Data)
{
	char cSREG;

	_WDR();
	while(EECR & 0x02) _NOP();
	EEAR = Addr;
	EEDR = Data;

	cSREG = SREG;
	/* stored status register */

	SREG &= ~0x80;
	/* disable global interrupt */

	EECR |= 0x04;
	/* enable EEPROM Master Write EEMWE */

	EECR |= 0x02;
	/* enable EEPROM Write EEWE */

	SREG = cSREG;
	/* restored register */
	
	return true;
}

tU8 HalEepromRead(tU16 Addr, tU8 *pData)
{
	_WDR();
	while(EECR & 0x02) _NOP();
	EEAR = Addr;
	EECR |= 0x01;
	*pData = EEDR;
	
	return true;
}

void main( void )
{
  unsigned char i;
  tU8 Signature[2];
  
  InitEepCommonConfig(&EepConfig, __EEPROM_SIZE__, HalEepromWrite, HalEepromRead);
  InitEepControl(&EepVar, (unsigned char *) &Var, sizeof(Var), &EepConfig);
  InitEepControl(&EepArray, Array, sizeof(Array), &EepConfig);
  
  HalEepromRead(__EEPROM_SIZE__ - 10, &Signature[0]);
  HalEepromRead(__EEPROM_SIZE__ - 9, &Signature[1]);
  if((Signature[0] != 'O') || (Signature[1] != 'K'))   //check first init.
  {
    HalEepromRead(__EEPROM_SIZE__ - 10, &Signature[0]);
    HalEepromRead(__EEPROM_SIZE__ - 9, &Signature[1]);
    if((Signature[0] != 'O') || (Signature[1] != 'K'))
    {
      Var.Data1 = 10;
      Var.Data2 = 1000;
      Var.Data3 = 3.14;
      SetEepWriteEnable(&EepVar);
      while(DoEepWriteControl(&EepVar) == true);
      
      for(i = 0; i < 10; i++)
      {
        Array[i] = i;
      }
      SetEepWriteEnable(&EepArray);
      while(DoEepWriteControl(&EepArray) == true);
      
      HalEepromWrite(__EEPROM_SIZE__ - 10, 'O'); HalEepromWrite(__EEPROM_SIZE__ - 9, 'K');
    }
  }
  
  DoEepReadControl(&EepVar);
  DoEepReadControl(&EepArray);
  
  while(1)
  {
    if(SaveEepFlag)
    {
      SaveEepFlag = false;
      //write here some user code..
      SetEepWriteEnable(&EepVar);
      SetEepWriteEnable(&EepArray);
    }
    
    DoEepWriteControl(&EepVar);
    DoEepWriteControl(&EepArray);
  }
}
