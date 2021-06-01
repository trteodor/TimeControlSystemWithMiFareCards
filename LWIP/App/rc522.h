/*
 * RC522 Simple Lib  -  Test
 *
 *  Created on: 20 ap. 2021
 *  trteodor@gmail.com
 *  	The Mit License
 *      Author: Teodor Rosolowski
 *      https://github.com/trteodor
 *
 */
#ifndef INC_RC522_H_
#define INC_RC522_H_

typedef enum{
	MFRC522_OK,
	MFRC522_Error,
	MFRC522_NOTAGERR,
}MFRC522_StatusHandle;

typedef struct __MFRC522_Handle{
	int i;
}MFRC522_Handle;

typedef	unsigned char uint8_t;
typedef	unsigned int uint;

#define BLOCK_MAX_LEN 16
#define UID_SIZE 5
// Registers the MF522
#define PCD_IDLE              0x00               //NO action; Huy bo lenh hien hanh
#define PCD_AUTHENT           0x0E               //Accomplish the KEY
#define PCD_RECEIVE           0x08               //Nhan du Lieu
#define PCD_TRANSMIT          0x04               //Gui of the place
#define PCD_TRANSCEIVE        0x0C               //Gui and receive data
#define PCD_RESETPHASE        0x0F               //Reset
#define PCD_CALCCRC           0x03               //Calculator CRC
// Registrar setting for Mifare_One
#define PICC_REQIDL           0x26               //Antennas are active, do not cover due to power
#define PICC_REQALL           0x52               //All types
#define PICC_ANTICOLL         0x93               //Chong va cham the
#define PICC_SElECTTAG        0x93               //But choose the
#define PICC_AUTHENT1A        0x60               //Accurate verification
#define PICC_AUTHENT1B        0x61               //Verify movie B
#define PICC_READ             0x30               //Reading
#define PICC_WRITE            0xA0               //Khoi viet
#define PICC_DECREMENT        0xC0               //Remember no
#define PICC_INCREMENT        0xC1               //Nap tien
#define PICC_RESTORE          0xC2               //Switch back to the night
#define PICC_TRANSFER         0xB0               //Included in the night of data
#define PICC_HALT             0x50               //Vao che do ngu

//Page 0:Command and Status
//#define     Reserved00            0x00
#define     CommandReg            0x01
#define     CommIEnReg            0x02
#define     DivlEnReg             0x03
#define     CommIrqReg            0x04
#define     DivIrqReg             0x05
#define     ErrorReg              0x06
#define     Status1Reg            0x07
#define     Status2Reg            0x08
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     Reserved01            0x0F
//Page 1:Command
//#define     Reserved10            0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     Reserved11            0x1A
#define     Reserved12            0x1B
#define     MifareReg             0x1C
#define     Reserved13            0x1D
#define     Reserved14            0x1E
#define     SerialSpeedReg        0x1F
//Page 2:CFG
//#define     Reserved20            0x20
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     Reserved21            0x23
#define     ModWidthReg           0x24
#define     Reserved22            0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsPReg	            0x28
#define     ModGsPReg             0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
//Page 3:TestRegister
//#define     Reserved30            0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39
#define     TestDAC2Reg           0x3A
#define     TestADCReg            0x3B
#define     Reserved31            0x3C
#define     Reserved32            0x3D
#define     Reserved33            0x3E
#define     Reserved34			  		0x3F
//-----------------------------------------------
// The desire to use
MFRC522_StatusHandle MFRC522_Init(void);
void ClearBitMask(uint8_t reg, uint8_t mask);
MFRC522_StatusHandle MFRC522_Request(uint8_t reqMode, uint8_t *TagType);
MFRC522_StatusHandle MFRC522_Anticoll(uint8_t *serNum);
MFRC522_StatusHandle MFRC522_SelectTag(uint8_t *serNum);
MFRC522_StatusHandle MFRC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey, uint8_t *serNum);
MFRC522_StatusHandle MFRC522_WriteBlock(uint8_t blockAddr, uint8_t *writeData);
MFRC522_StatusHandle MFRC522_ReadBlock(uint8_t blockAddr, uint8_t *recvData);
MFRC522_StatusHandle MFRC522_Halt(void);
MFRC522_StatusHandle MFRC522_DeAuth();


#endif /* INC_RC522_H_ */
