/*********************************************************************************/
/*
 * Author : Jung Hyun Gu
 * File name : AvrEeprom.h
*/
/*********************************************************************************/
#ifndef __AVR_EEPROM_H__
#define	__AVR_EEPROM_H__
/*********************************************************************************/
#define AVR_EEPROM_REVISION_DATE		20161130
/*********************************************************************************/
/** REVISION HISTORY **/
/*
	2016. 11. 30.					- DoEepReadControl(), GetDataFromEeprom() 함수에서 Length 보다 1바이트 더 읽던 현상 수정.
	Jung Hyun Gu

	2016. 11. 08.					- revision valid check 추가.
	Jung Hyun Gu

	2016. 10. 28.					- DoEepWriteControl() 반환값 수정.
	Jung Hyun Gu					- 반환값 관련 주석 추가.

	2016. 10. 28.					- 초기버전.
	Jung Hyun Gu
*/
/*********************************************************************************/
/**Define**/

#define	true		1
#define	false		0


#define __AVR_ATMEGA64__


#ifdef __AVR_ATMEGA8__
#include <iom8.h>
#define	EEPROM_SIZE		512
/* Endurance: 100,000 Write/Erase */
#endif

#ifdef __AVR_ATMEGA16__
#include <iom16.h>
#define	EEPROM_SIZE		512
/* Endurance: 100,000 Write/Erase */
#endif

#ifdef __AVR_ATMEGA32__
#include <iom32.h>
#define	EEPROM_SIZE		1024
/* Endurance: 100,000 Write/Erase */
#endif

#ifdef __AVR_ATMEGA64__
#include <iom64.h>
#define	EEPROM_SIZE		2048
/* Endurance: 100,000 Write/Erase */
#endif

#ifdef __AVR_ATMEGA128__
#include <iom128.h>
#define	EEPROM_SIZE		4096
/* Endurance: 100,000 Write/Erase */
#endif

/*********************************************************************************/
/**Enum**/


/*********************************************************************************/
/**Struct**/

typedef struct
{
	char Init						:		1;
	char Write					:		1;
}tag_EepBitField;

typedef const struct
{
	char *DataBase;								// 관리할 데이터의 시작 주소
	unsigned int EepBase; 				// eeprom의 시작 주소
	unsigned int Index;	 					// 쓰기 인덱스 
	unsigned int Length; 					// 관리할 데이터의 길이
	
	tag_EepBitField Bit;
}tag_EepControl;

/*********************************************************************************/
/**Function**/

char InitEepControl(tag_EepControl *Eep, const unsigned char *DataBase, unsigned int Length);

void DoEepReadControl(tag_EepControl *Eep);
void GetDataFromEeprom(char* const Dest, const int EepBase, int Length);

char DoEepWriteControl(tag_EepControl *Eep);
void SetEepWriteEnable(tag_EepControl *Eep);

void Eeprom_Write(unsigned int Addr, char Data);
char Eeprom_Read(unsigned int Addr);

/*********************************************************************************/
#endif //__AVR_EEPROM_H__











