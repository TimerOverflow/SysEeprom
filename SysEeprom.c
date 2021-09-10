/*********************************************************************************/
/*
 * Author : Jeong Hyun Gu
 * File name : SysEeprom.c
*/
/*********************************************************************************/
#include <string.h>
#include "SysEeprom.h"
/*********************************************************************************/
#if(SYS_EEPROM_REVISION_DATE != 20190905)
#error wrong include file. (SysEeprom.h)
#endif
/*********************************************************************************/
/** Global variable **/

/*********************************************************************************/
static inline tU8 CheckAllOfInit(tag_EepControl *Eep)
{
	return (Eep->Bit.InitGeneral) ? true : false;
}
/*********************************************************************************/
tU8 InitEepCommonConfig(tag_EepCommonConfig *EepConfig, tU16 LastAddr, tU8 (*EepromWrite)(tU16 Addr, tU8 Data), tU8 (*EepromRead)(tU16 Addr, tU8 *pData))
{
	tU16 *pLastAddr = (tU16 *) &EepConfig->LastAddr;
	static tag_EepControl EepSignature = { .Bit.InitGeneral = false };
	volatile static tU8 StrSignature[3];

	/*
		1) 인수
			- EepConfig : EepConfig 인스턴스의 주소
			- EepromWrite : 사용자가 구현한 EepromWrite 함수 포인터 주소
			- EepromRead : 사용자가 구현한 EepromRead 함수 포인터 주소

		2) 반환
			- 초기화 성공 여부

		3) 설명
			- tag_EepControl::Config 구조체의 필요한 정보를 초기화 한다.
			- EEPROM의 크기, HAL 구현 함수등을 연결.
	*/

	*pLastAddr = LastAddr;
	EepConfig->EepromWrite = EepromWrite;
	EepConfig->EepromRead = EepromRead;
	EepConfig->Bit.Init = true;
	
	memset((void *) StrSignature, 0, sizeof(StrSignature));
	
	InitEepControl(&EepSignature, (const tU8 *) StrSignature, sizeof(StrSignature), EepConfig);
	DoEepReadControl(&EepSignature);
	if((StrSignature[0] != 'J') || (StrSignature[1] != 'H') || (StrSignature[2] != 'G'))
	{
		DoEepReadControl(&EepSignature);
		if((StrSignature[0] != 'J') || (StrSignature[1] != 'H') || (StrSignature[2] != 'G'))
		{
			StrSignature[0] = 'J'; StrSignature[1] = 'H'; StrSignature[2] = 'G';
			SetEepWriteEnable(&EepSignature);
			while(DoEepWriteControl(&EepSignature) == true)
			{
				EepConfig->Bit.FirstExecute = true;
			}
		}
	}
	
	return EepConfig->Bit.Init;
}
/*********************************************************************************/
tU8 InitEepControl(tag_EepControl *Eep, const tU8 *DataBase, tU16 Length, tag_EepCommonConfig *Config)
{
	const tU8 GapOfAnotherSector = 10;
	tU8 **pDataBase = (tU8 **) &Eep->DataBase;
	tU16 *pEepBase = (tU16 *) &Eep->EepBase;
	tU16 *pLength = (tU16 *) &Eep->Length;
	tag_EepBitField *pBit = (tag_EepBitField *) &Eep->Bit;
	tag_EepCommonConfig **pConfig = (tag_EepCommonConfig **) &Eep->Config;

	/*
		1) 인수
			- Eep : tag_EepControl 인스턴스의 주소
			- DataBase : eeprom을 통해 관리할 대상의 시작주소
			- Length : 관리할 대상의 크기
			- Config : Eeprom에 대한 크기 정보, HAL 함수 정보를 가지고 있는 tag_EepCommonConfig 구조체 인스턴스의 주소.

		2) 반환
			- 초기화 성공 여부

		3) 설명
			- tag_EepControl 인스턴스를 초기화한다.
			- tag_EepControl 인스턴스에 eeprom으로 부터 읽거나 쓸 대상의 주소와, 할당할 eeprom의 시작과 길이를 초기화한다.
	*/

	if(Config->Bit.Init == false)
	{
		return false;
	}

	*pConfig = (tag_EepCommonConfig *) Config;
	//Link with "Eep" struct.

	if((DataBase != 0) && (Length != 0) && ((Config->AllocEepAddr + Length + GapOfAnotherSector) < Config->LastAddr))
	{
		*pLength = Length;
		*pDataBase = (tU8 *) DataBase;
		*pEepBase = Config->AllocEepAddr;
		Config->AllocEepAddr += (Length + GapOfAnotherSector);

		pBit->InitGeneral = true;
	}

	pBit->InitComplete = CheckAllOfInit(Eep);

	return pBit->InitGeneral;
}
/*********************************************************************************/
void DoEepReadControl(tag_EepControl *Eep)
{
	tU8 *pDataBase = (tU8 *) Eep->DataBase;
	tU16 Index = 0;
	tU8 Data;

	/*
		1) 인수
			- Eep : tag_EepControl 인스턴스의 주소

		2) 반환
			- 없음.

		3) 설명
			- 해당 인스턴스의 eeprom으로 부터 데이터를 읽어와 대상 버퍼에 값을 대입.
	*/

	if(Eep->Bit.InitComplete == false)
	{
		return;
		/* error or disabled */
	}

	do
	{
		Eep->Config->EepromRead(Eep->EepBase + (Index), &Data);
		pDataBase[Index] = Data;
	}while(++Index < Eep->Length);
}
/*********************************************************************************/
tU8 DoEepWriteControl(tag_EepControl *Eep)
{
	tU16 *pIndex = (tU16 *) &Eep->Index;
	tag_EepBitField *pBit;
	tU8 Data = 0;

	/*
		1) 인수
			- Eep : tag_EepControl 인스턴스의 주소

		2) 반환
			- 쓰기동작 중일 때 true, 그외 상황일 때 false.

		3) 설명
			- 해당 인스턴스의 eeprom 영역에 관리할 대상 데이터를 쓰기를 진행한다.
			- SetEepWriteEnable() 함수를 통해 'Eep.Bit.Write'를 활성화 시켜야 실행된다.
			- 쓰기전 해당주소의 값이 쓰려고 하는 값과 일치하지 않을 경우에만 유효하다고 판단하며,
				유효한 데이터만 쓰기를 진행하고 유효하지 않을 경우 다음 주소로 건너뛴다.
			- 값이 유효하여 쓰기를 진행하거나 해당영역의 마지막에 도달할 경우에만 while loop를 탈출한다.
	*/

	if((Eep->Bit.InitComplete == false) || (Eep->Bit.Write == false))
	{
		return false;
		/* error or disabled */
	}

	while(true)
	{
		Eep->Config->EepromRead(Eep->EepBase + (*pIndex), &Data);
		if(Data != Eep->DataBase[*pIndex])
		{
			Eep->Config->EepromWrite(Eep->EepBase + (*pIndex), Eep->DataBase[*pIndex]);
			return true;
			/* check valid data */
		}

		if(++(*pIndex) >= Eep->Length)
		{
			pBit = (tag_EepBitField *) &Eep->Bit;
			pBit->Write = false;
			return false;
			/* end of sector */
		}
	}
}
/*********************************************************************************/
void SetEepWriteEnable(tag_EepControl *Eep)
{
	/*
		1) 인수
			- Eep : tag_EepControl 인스턴스의 주소

		2) 반환
			- 없음.

		3) 설명
			- 해당 인스턴스에 쓰기 동작을 활성화한다.
			- 'Eep.Bit.Write' 상태가 'true'일 때 DoEepWriteControl()함수가 쓰기 동작을 실행한다.
	*/

	if(Eep->Bit.InitComplete == false)
	{
		return;
		/* error or disabled */
	}

	tag_EepBitField *pBit = (tag_EepBitField *) &Eep->Bit;
	tU16 *pIndex = (tU16 *) &Eep->Index;

	pBit->Write = true;
	*pIndex = 0;
}
/*********************************************************************************/
