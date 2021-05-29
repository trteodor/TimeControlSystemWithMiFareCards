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



#include "rc522.h"

//Definitions pertaining to my device
#include "stm32h7xx_hal.h"
#define CS_PORT GPIOF
#define CS_PIN GPIO_PIN_3
extern SPI_HandleTypeDef hspi1;

//Only this 3 function below depend from your device
uint8_t RC522_SPI_Transfer(uint8_t data) //This  function depend from your device
{
	uint8_t rx_data;
	HAL_SPI_TransmitReceive(&hspi1,&data,&rx_data,1,100);
	return rx_data;
}


__attribute__((weak)) void MFRC522_Write(uint8_t addr, uint8_t val) //This  function depend from your device
{
		HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_RESET);
		RC522_SPI_Transfer((addr<<1)&0x7E);
		RC522_SPI_Transfer(val);
		HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_SET);

}


__attribute__((weak)) uint8_t MFRC522_Read(uint8_t addr) //This function depend from your device
{
		uint8_t val;
		HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_RESET);
		RC522_SPI_Transfer(((addr<<1)&0x7E) | 0x80);
		val = RC522_SPI_Transfer(0x00);
		HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_SET);
		return val;
}

MFRC522_StatusHandle MFRC522_SetBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    tmp = MFRC522_Read(reg);
    MFRC522_Write(reg, tmp | mask);  // set bit mask
    return MFRC522_OK;
}
MFRC522_StatusHandle MFRC522_ClearBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    tmp = MFRC522_Read(reg);
    MFRC522_Write(reg, tmp & (~mask));  // clear bit mask
    return MFRC522_OK;
} 

MFRC522_StatusHandle AntennaOn(void)
{
	MFRC522_Read(TxControlReg);
MFRC522_SetBitMask(TxControlReg, 0x03);
return MFRC522_OK;
}
MFRC522_StatusHandle MFRC522_AntennaOff(void)
{
	MFRC522_ClearBitMask(TxControlReg, 0x03);
	return MFRC522_OK;
}
MFRC522_StatusHandle MFRC522_Reset(void)
{
    MFRC522_Write(CommandReg, PCD_RESETPHASE);
    return MFRC522_OK;
}
MFRC522_StatusHandle MFRC522_Init(void)
{

	MFRC522_Reset();
	MFRC522_Write(TModeReg, 0x8D);		//auto=1; f(Timer) = 6.78MHz/TPreScaler
	MFRC522_Write(TPrescalerReg, 0x3E);	//TModeReg[3..0] + TPrescalerReg
	MFRC522_Write(TReloadRegL, 30);
	MFRC522_Write(TReloadRegH, 0);
	MFRC522_Write(TxAutoReg, 0x40);		//100%ASK
	MFRC522_Write(ModeReg, 0x3D);		//CRC Original value 0x6363	???

	MFRC522_ClearBitMask(Status2Reg, 0x08);		//MFCrypto1On=0   // USE this line after evry Auth

	//MFRC522_Write(RxSelReg, 0x86);		//RxWait = RxSelReg[5..0]
	//MFRC522_Write(RFCfgReg, 0x7F);   		//RxGain = 48dB
	AntennaOn();		//Mo Anten
    return MFRC522_OK;
}
MFRC522_StatusHandle MFRC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint *backLen)
{
    uint8_t status = MFRC522_Error;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint i;

    switch (command)
    {
        case PCD_AUTHENT:		//Acknowledging the liver
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE:	// FIFO data collection
		{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
			break;
    }
   
    MFRC522_Write(CommIEnReg, irqEn|0x80);	//Yeu cau ngat
    MFRC522_ClearBitMask(CommIrqReg, 0x80);			//Clear all the bits
    MFRC522_SetBitMask(FIFOLevelReg, 0x80);			//FlushBuffer=1, FIFO
    
	MFRC522_Write(CommandReg, PCD_IDLE);	//NO action; Huy bo lenh hien hanh	???

	// Record in FIFO
    for (i=0; i<sendLen; i++)
    {   
		MFRC522_Write(FIFODataReg, sendData[i]);
	}

	//chay
	MFRC522_Write(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {    
		MFRC522_SetBitMask(BitFramingReg, 0x80);		//StartSend=1,transmission of data starts
	}   
    
	//The team is allowed to be stored
	i = 2000;	//i tuy thuoc tan so thach anh, thoi gian toi da cho the M1 la 25ms
    do 
    {
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = MFRC522_Read(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    MFRC522_ClearBitMask(BitFramingReg, 0x80);			//StartSend=0
	
    if (i != 0)
    {    
        if(!(MFRC522_Read(ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MFRC522_OK;
            if (n & irqEn & 0x01)
            {   
				status = MFRC522_NOTAGERR;			//??
			}

            if (command == PCD_TRANSCEIVE)
            {
               	n = MFRC522_Read(FIFOLevelReg);
              	lastBits = MFRC522_Read(ControlReg) & 0x07;
                if (lastBits)
                {   
					*backLen = (n-1)*8 + lastBits;   
				}
                else
                {   
					*backLen = n*8;   
				}

                if (n == 0)
                {   
					n = 1;    
				}
                if (n > MAX_LEN)
                {   
					n = MAX_LEN;   
				}
				
				//FIFO doc in the received data
                for (i=0; i<n; i++)
                {   
					backData[i] = MFRC522_Read(FIFODataReg);
				}
            }
        }
        else
        {   
			status = MFRC522_Error;
		}
    }
    //MFRC522_Write(CommandReg, PCD_IDLE);
    return status;
}

MFRC522_StatusHandle MFRC522_Request(uint8_t reqMode, uint8_t *TagType)
{
	uint8_t status;  
	uint backBits;			//The bits are manipulated

	MFRC522_Write(BitFramingReg, 0x07);		//TxLastBists = BitFramingReg[2..0]	???
	
	TagType[0] = reqMode;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != MFRC522_OK) || (backBits != 0x10))
	{    
		status = MFRC522_Error;
	}
return status;
}


/*
 * Ten ham:MFRC522_Anticoll
 * Detect the collision, select and read the serial number
 * Input: serNum - Look up the serial the 4 byte, byte 5 is the ma checksum
 * Check: MFRC522_OK if successful
 */
MFRC522_StatusHandle MFRC522_Anticoll(uint8_t *serNum)
{
    uint8_t status;
    uint8_t i;
	uint8_t serNumCheck=0;
    uint unLen;

    //MFRC522_ClearBitMask(Status2Reg, 0x08);		//TempSensclear
    //MFRC522_ClearBitMask(CollReg,0x80);			//ValuesAfterColl
	MFRC522_Write(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]
     serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MFRC522_OK)
	{
		//Check the serial number
		for (i=0; i<4; i++)
		{   
		 	serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i])
		{   
			status = MFRC522_Error;
		}
    }
    //MFRC522_SetBitMask(CollReg, 0x80);		//ValuesAfterColl=1
    return status;
} 


/*
 * Ten Ham:CalulateCRC
 * MFRC522 is a formula of RC522
 * Input: pIndata - Data CRC into calculator, wool - Data data, pOutData - CRC calculation
 * Check: No
 */
MFRC522_StatusHandle CalulateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData)
{
    uint8_t i, n;

    MFRC522_ClearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
    MFRC522_SetBitMask(FIFOLevelReg, 0x80);			//Con tro FIFO
    //MFRC522_Write(CommandReg, PCD_IDLE);

	//Record in FIFO
    for (i=0; i<len; i++)
    {   
		MFRC522_Write(FIFODataReg, *(pIndata+i));
	}
    MFRC522_Write(CommandReg, PCD_CALCCRC);

	// Let the CRC computer complete
    i = 0xFF;
    do 
    {
        n = MFRC522_Read(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	//Doc results in CRC calculation
    pOutData[0] = MFRC522_Read(CRCResultRegL);
    pOutData[1] = MFRC522_Read(CRCResultRegM);
    return MFRC522_OK;
}

MFRC522_StatusHandle MFRC522_SelectTag(uint8_t *serNum)
{
	uint8_t i;
	uint8_t status;
	uint8_t size;
	uint recvBits;
	uint8_t buffer[9]; 

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
    	buffer[i+2] = *(serNum+i);
    }
	CalulateCRC(buffer, 7, &buffer[7]);		//??
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    
    if ((status == MFRC522_OK) && (recvBits == 0x18))
    {   
		size = buffer[0]; 
	}
    else
    {   
		size = 0;    
	}

    return size;
}

MFRC522_StatusHandle MFRC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey, uint8_t *serNum)
{
    uint8_t status;
    uint recvBits;
    uint8_t i;
	uint8_t buff[12]; 

	//Confirmation + Address + password + quick number
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {    
		buff[i+2] = *(Sectorkey+i);   
	}
    for (i=0; i<4; i++)
    {    
		buff[i+8] = *(serNum+i);   
	}
    status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MFRC522_OK) || (!(MFRC522_Read(Status2Reg) & 0x08)))
    {   
		status = MFRC522_Error;
	}
    
    return status;
}

MFRC522_StatusHandle MFRC522_ReadBlock(uint8_t blockAddr, uint8_t *recvData)
{
    uint8_t status;
    uint unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    CalulateCRC(recvData,2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MFRC522_OK) || (unLen != 0x90))
    {
        status = MFRC522_Error;
    }
    return status;
}
MFRC522_StatusHandle MFRC522_WriteBlock(uint8_t blockAddr, uint8_t *writeData)
{
    uint8_t status;
    uint recvBits;
    uint8_t i;
	uint8_t buff[18]; 
    
    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalulateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MFRC522_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {   
		status = MFRC522_Error;
	}
        
    if (status == MFRC522_OK)
    {
        for (i=0; i<16; i++)		//16 FIFO bytes recorded
        {    
        	buff[i] = *(writeData+i);   
        }
        CalulateCRC(buff, 16, &buff[16]);
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
        
		if ((status != MFRC522_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {   
			status = MFRC522_Error;
		}
    }
    
    return status;
}
MFRC522_StatusHandle MFRC522_Halt(void)
{
	uint unLen;
	uint8_t buff[4]; 

	buff[0] = PICC_HALT;
	buff[1] = 0;
	CalulateCRC(buff, 2, &buff[2]);
 
	MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
	return MFRC522_OK;
}

MFRC522_StatusHandle MFRC522_DeAuth()  //CallThisFunctionAfterEvry Authenticate
{
	MFRC522_ClearBitMask(Status2Reg, 0x08);
	return MFRC522_OK;
}




