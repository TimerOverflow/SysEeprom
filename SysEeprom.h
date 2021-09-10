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
#define SYS_EEPROM_REVISION_DATE		20200417
/*********************************************************************************/
/** REVISION HISTORY **/
/*
	2020. 04. 17.					- tag_EepCommonConfig의 시그니처는 연결된 모든 tag_EepControl 인스턴스들이
	Jeong Hyun Gu						write동작을 끝냈을 때 write하도록 변경.

	2020. 01. 15.					- 실제 Eeprom과의 정상 동작 여부 확인 기능 추가.
	Jeong Hyun Gu					- CheckEepromError() 함수 추가. 'tag_EepCommonConfig'에서 관리 하는 실제
													Eeprom의 정상 동작 여부 확인.

	2019. 10. 01.					- EraseEepCommonConfigSignature() 함수 추가. InitEepCommonConfig()에서
	Jeong Hyun Gu						최초 실행 여부를 확인하는데 사용하는 참조 값 Signature를 지워 다음 실행에서
													공장 초기화 실행 가능.

	2019. 09. 24.					- CheckEepromFirstExecuteSignature() 매크로 함수 추가. 직접 비트 필드에
	Jeong Hyun Gu						접근하지 않고 위 매크로 함수로 접근.

	2019. 09. 05.					- 프로그램 최초 실행 감지 루틴을 모듈 내 InitEepCommonConfig() 함수에 추가.
	Jeong Hyun Gu						이제 어플리케이션에서 시그니쳐 확인하여 초기화를 실행 여부를 판단할 필요없이
													tag_EepCommonConfig::Bit.FirstExecute 비트가 '1'이면 최초 실행으로 판단.
												- 이전 버전에서 업데이트 시 AVR-EESAVE 불가.

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
		tU8 FirstExecute		:		1;
		tU8 ReadFail				:		1;
		tU8 WriteFail				:		1;
	}Bit;

	const tU16 LastAddr;					// eeprom마지막 주소(크기)
	tU16 AllocEepAddr;
	tU8 SignatureWriteDelay;

	tU8 (*HalEepromWrite)(tU16 Addr, tU8 Data);
	tU8 (*HalEepromRead)(tU16 Addr, tU8 *pData);
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

tU8 InitEepCommonConfig(tag_EepCommonConfig *EepConfig, tU16 LastAddr, tU8 (*HalEepromWrite)(tU16 Addr, tU8 Data), tU8 (*HalEepromRead)(tU16 Addr, tU8 *pData));
tU8 InitEepControl(tag_EepControl *Eep, const tU8 *DataBase, tU16 Length, tag_EepCommonConfig *EepConfig);

#define CheckEepromFirstExecuteSignature(Eepconfig)					((Eepconfig)->Bit.FirstExecute)
#define CheckEepromError(Eepconfig)					(((Eepconfig)->Bit.ReadFail) || ((Eepconfig)->Bit.WriteFail))
void EraseEepCommonConfigSignature(tag_EepCommonConfig *EepConfig);

void SetEepWriteEnable(tag_EepControl *Eep);
void DoEepReadControl(tag_EepControl *Eep);
tU8 DoEepWriteControl(tag_EepControl *Eep);

/*********************************************************************************/
#endif //__SYS_EEPROM_H__
