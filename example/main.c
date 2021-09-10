
#include "AvrEeprom.h"

typedef struct
{
  char Data1;
  int Data2;
  float Data3;
}tag_Var;
tag_Var Var;

unsigned char Array[10];

tag_EepControl EepVar, EepArray;
char SaveEepFlag;

void main( void )
{
  unsigned char i;
  
  InitEepControl(&EepVar, (unsigned char *) &Var, sizeof(Var));
  InitEepControl(&EepArray, Array, sizeof(Array));
  
  if((Eeprom_Read(EEPROM_SIZE - 10) != 'O') || (Eeprom_Read(EEPROM_SIZE - 9) != 'K'))   //check first init.
  {
    if((Eeprom_Read(EEPROM_SIZE - 10) != 'O') || (Eeprom_Read(EEPROM_SIZE - 9) != 'K'))
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
      
      Eeprom_Write(EEPROM_SIZE - 10, 'O'); Eeprom_Write(EEPROM_SIZE - 9, 'K');
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
