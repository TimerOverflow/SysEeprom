/*********************************************************************************/
/*
 * Author : Jeong Hyun Gu
 * File name : SysEeprom.h
*/
/*********************************************************************************/
#ifndef __SYS_EEPROM_H__
#define	__SYS_EEPROM_H__
/*********************************************************************************/
#include "SysTypedef.h"
/*********************************************************************************/
#define SYS_EEPROM_REVISION_DATE		20190723
/*********************************************************************************/
/** REVISION HISTORY **/
/*
	2019. 07. 23.					- Eeprom_Write(), Eeprom_Read() 삭제하고 HAL 적용.
	Jeong Hyun Gu					- 접두어 AVR -> SYS로 변경.
												- GetDataFromEeprom() 삭제.
												- "tag_EepCommonConfig" 추가.

	2019. 01. 14.					- CPU타입 __AVR_ATMEGA2560__ 추가.
	Jeong Hyun Gu

	2018. 09. 21.					- DoEepWriteControl() 함수에서 Length 보다 1바이트 더 쓰던 현상 수정.
	Jeong Hyun Gu

	2016. 12. 26.					- Eeprom_Write(), Eeprom_Read() 함수에서 WDT RESET 추가.
	Jeong Hyun Gu

	2016. 11. 30.					- DoEepReadControl(), GetDataFromEeprom() 함수에서 Length 보다 1바이트 더 읽던 현상 수정.
	Jeong Hyun Gu

	2016. 11. 08.					- revision valid check 추가.
	Jeong Hyun Gu

	2016. 10. 28.					- DoEepWriteControl() 반환값 수정.
	Jeong Hyun Gu					- 반환값 관련 주석 추가.

	2016. 10. 28.					- 초기버전.
	Jeong Hyun Gu
*/
/*********************************************************************************/
/**Define**/

#define	true		1
#define	false		0

/*********************************************************************************/
/**Enum**/


/*********************************************************************************/
/**Struct**/

typedef struct
{
	struct
	{
		tU8 Init						:		1;
	}Bit;

	const tU16 LastAddr;					// eeprom마지막 주소(크기)
	tU16 AllocEepAddr;

	tU8 (*EepromWrite)(tU16 Addr, tU8 Data);
	tU8 (*EepromRead)(tU16 Addr, tU8 *pData);
}tag_EepCommonConfig;

typedef struct
{
	tU8 InitGeneral			:		1;
	tU8 InitComplete		:		1;
	tU8 Write						:		1;
}tag_EepBitField;

typedef struct
{
	const tag_EepBitField Bit;
	const tag_EepCommonConfig *Config;

	const tU16 EepBase; 					// eeprom의 시작 주소
	const tU8 *DataBase;					// 관리할 데이터의 시작 주소

	const tU16 Index;	 					// 쓰기 인덱스
 	const tU16 Length; 					// 관리할 데이터의 길이
}tag_EepControl;

/*********************************************************************************/
/**Function**/

tU8 InitEepCommonConfig(tag_EepCommonConfig *EepConfig, tU16 LastAddr, tU8 (*EepromWrite)(tU16 Addr, tU8 Data), tU8 (*EepromRead)(tU16 Addr, tU8 *pData));
tU8 InitEepControl(tag_EepControl *Eep, const tU8 *DataBase, tU16 Length, tag_EepCommonConfig *EepConfig);

void SetEepWriteEnable(tag_EepControl *Eep);
void DoEepReadControl(tag_EepControl *Eep);
tU8 DoEepWriteControl(tag_EepControl *Eep);

/*********************************************************************************/
#endif //__SYS_EEPROM_H__
