/*********************************************************************************************************
*
* File                : fsmc_nand.c
* Hardware Environment:
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  :
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "nandflash.h"
/* Private define ------------------------------------------------------------*/
#define NAND_FLASH_START_ADDR     ((uint32_t)0x70000000)
#define NAND_TIMEOUT ((uint8_t)0x01)
#define NAND_OK ((uint8_t)0x00)
#define ROW_ADDRESS (Address.Page + (Address.Block + (Address.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE)

/* Private variables ---------------------------------------------------------*/
static uint8_t TxBuffer [NAND_PAGE_SIZE];
static uint8_t RxBuffer [NAND_PAGE_SIZE];


uint8_t	NAND_ReadyWait(void);
/*******************************************************************************
* Function Name  : FSMC_NAND_Init
* Description    : Configures the FSMC and GPIOs to interface with the NAND memory.
*                  This function must be called before any write/read operation
*                  on the NAND.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void FMC_NAND_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  FMC_NANDInitTypeDef FMC_NANDInitStructure;
  FMC_NAND_PCCARDTimingInitTypeDef  p;

  /* Enable FSMC, GPIOD, GPIOE and AFIO clocks */
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);

  /*-- GPIO Configuration ------------------------------------------------------*/

  /* CLE, ALE, D0->D3, NOE, NWE and NCE2  NAND pin configuration  */
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_11 | // FMC_A16
		  	  	  	  	  	  	  GPIO_Pin_12 | // FMC_A17
		  	  	  	  	  	  	  GPIO_Pin_14 | // D0
								  GPIO_Pin_15 | // D1
								  GPIO_Pin_0 |	// D2
								  GPIO_Pin_1 |	// D3
								  GPIO_Pin_4 |	// FMC_NOE +
								  GPIO_Pin_5 |	// FMC_NWE +
								  GPIO_Pin_7 ;	// FMC_NE1/FMC_NCE2
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FMC);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);


  /* D4->D7 NAND pin configuration ------------------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | // D4
		  	  	  	  	  	  	GPIO_Pin_8 | // D5
								GPIO_Pin_9 | // D6
								GPIO_Pin_10; // D7
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FMC);
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* NWAIT NAND pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /*-- FSMC Configuration ------------------------------------------------------*/
  p.FMC_SetupTime = 0xf1;
  p.FMC_WaitSetupTime = 0xf2;
  p.FMC_HoldSetupTime = 0xf3;
  p.FMC_HiZSetupTime = 0xf1;

  FMC_NANDInitStructure.FMC_Bank = FMC_Bank2_NAND;
  FMC_NANDInitStructure.FMC_Waitfeature = FMC_Waitfeature_Enable;
  FMC_NANDInitStructure.FMC_MemoryDataWidth = FMC_MemoryDataWidth_8b;
//  FMC_NANDInitStructure.FMC_ECC = FMC_ECC_Enable;
//  FMC_NANDInitStructure.FMC_ECCPageSize = FMC_ECCPageSize_2048Bytes;
  FMC_NANDInitStructure.FMC_TCLRSetupTime = 0xa1;
  FMC_NANDInitStructure.FMC_TARSetupTime = 0x15;
  FMC_NANDInitStructure.FMC_CommonSpaceTimingStruct = &p;
  FMC_NANDInitStructure.FMC_AttributeSpaceTimingStruct = &p;

  FMC_NANDInit(&FMC_NANDInitStructure);

  /* FSMC NAND Bank Cmd Test */
  FMC_NANDCmd(FMC_Bank2_NAND, ENABLE);
//  FMC_NANDECCCmd(FMC_Bank2_NAND, ENABLE);
}


/******************************************************************************
* Function Name  : FSMC_NAND_ReadID
* Description    : Reads NAND memory's ID.
* Input          : - NAND_ID: pointer to a NAND_IDTypeDef structure which will hold
*                    the Manufacturer and Device ID.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
uint8_t FMC_NAND_ReadID(NAND_IDTypeDef* NAND_ID)
{
	uint32_t data = 0;
  /* Send Command to the command area */
  *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = 0x90;
  *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00;

   /* Sequence to read ID from NAND flash */
  uint32_t *iFLASH = (vu32 *)(NAND_FLASH_START_ADDR | DATA_AREA);
  data = *iFLASH;
   NAND_ID->Maker_ID   = ADDR_1st_CYCLE (data);
   NAND_ID->Device_ID  = ADDR_2nd_CYCLE (data);
   NAND_ID->Third_ID   = ADDR_3rd_CYCLE (data);
   NAND_ID->Fourth_ID  = ADDR_4th_CYCLE (data);
   return NAND_OK;
}

uint8_t NAND_ReadyWait()
{
	uint32_t timeouttemp=100000;

	while(!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6))
	{
	if (timeouttemp) {timeouttemp--;} else {return NAND_TIMEOUT;}
	}
	return NAND_OK;
}

/******************************************************************************
* Function Name  : FSMC_NAND_WriteSmallPage
* Description    : This routine is for writing one or several 512 Bytes Page size.
* Input          : - pBuffer: pointer on the Buffer containing data to be written
*                  - Address: First page address
*                  - NumPageToWrite: Number of page to write
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
*                  And the new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_WriteSmallPage(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumPageToWrite)
{
  uint32_t index = 0x00, numpagewritten = 0x00, addressstatus = NAND_VALID_ADDRESS;
  uint32_t status = NAND_READY, size = 0x00;

  while((NumPageToWrite != 0x00) && (addressstatus == NAND_VALID_ADDRESS) && (status == NAND_READY))
  {
    /* Page write command and address */
    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM;

    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00;
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0X00;
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);

    /* Calculate the size */
    size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpagewritten);

    /* Write data */
    for(; index < size; index++)
    {
      *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA) = pBuffer[index];
    }

    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM_TRUE;

    /* ¶ÁÃ¦½Å */
    while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );
//    while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );

    /* Check status for successful operation */
    status = FSMC_NAND_GetStatus();

    if(status == NAND_READY)
    {
      numpagewritten++;

      NumPageToWrite--;

      /* Calculate Next small page Address */
      addressstatus = FSMC_NAND_AddressIncrement(&Address);
    }
  }

  return (status | addressstatus);
}

/******************************************************************************
* Function Name  : FSMC_NAND_ReadSmallPage
* Description    : This routine is for sequential read from one or several
*                  512 Bytes Page size.
* Input          : - pBuffer: pointer on the Buffer to fill
*                  - Address: First page address
*                  - NumPageToRead: Number of page to read
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
*                  And the new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_ReadSmallPage(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumPageToRead)
{
  uint32_t index = 0x00, numpageread = 0x00, addressstatus = NAND_VALID_ADDRESS;
  uint32_t status = NAND_READY, size = 0x00;

  while((NumPageToRead != 0x0) && (addressstatus == NAND_VALID_ADDRESS))
  {
    /* Page Read command and page address */
    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_READ_1;

    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00;
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0X00;
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);

    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_READ_TRUE;

    /* ¶ÁÃ¦½Å */
//    while( GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0 );
//    while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );

    /* Calculate the size */
    size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpageread);

    /* Get Data into Buffer */
    for(; index < size; index++)
    {
      pBuffer[index]= *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA);
    }

    numpageread++;

    NumPageToRead--;

    /* Calculate page address */
    addressstatus = FSMC_NAND_AddressIncrement(&Address);
  }

  status = FSMC_NAND_GetStatus();

  return (status | addressstatus);
}

/******************************************************************************
* Function Name  : FSMC_NAND_WriteSpareArea
* Description    : This routine write the spare area information for the specified
*                  pages addresses.
* Input          : - pBuffer: pointer on the Buffer containing data to be written
*                  - Address: First page address
*                  - NumSpareAreaTowrite: Number of Spare Area to write
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
*                  And the new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_WriteSpareArea(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumSpareAreaTowrite)
{
  uint32_t index = 0x00, numsparesreawritten = 0x00, addressstatus = NAND_VALID_ADDRESS;
  uint32_t status = NAND_READY, size = 0x00;

  while((NumSpareAreaTowrite != 0x00) && (addressstatus == NAND_VALID_ADDRESS) && (status == NAND_READY))
  {
    /* Page write Spare area command and address */
    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM;

    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00;
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x08;
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);

    /* Calculate the size */
    size = NAND_SPARE_AREA_SIZE + (NAND_SPARE_AREA_SIZE * numsparesreawritten);

    /* Write the data */
    for(; index < size; index++)
    {
      *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA) = pBuffer[index];
    }

    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM_TRUE;

    /* ¶ÁÃ¦½Å */
//    while( GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0 );
//    while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );

    /* Check status for successful operation */
    status = FSMC_NAND_GetStatus();

    if(status == NAND_READY)
    {
      numsparesreawritten++;

      NumSpareAreaTowrite--;

      /* Calculate Next page Address */
      addressstatus = FSMC_NAND_AddressIncrement(&Address);
    }
  }

  return (status | addressstatus);
}

/******************************************************************************
* Function Name  : FSMC_NAND_ReadSpareArea
* Description    : This routine read the spare area information from the specified
*                  pages addresses.
* Input          : - pBuffer: pointer on the Buffer to fill
*                  - Address: First page address
*                  - NumSpareAreaToRead: Number of Spare Area to read
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
*                  And the new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_ReadSpareArea(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumSpareAreaToRead)
{
  uint32_t numsparearearead = 0x00, index = 0x00, addressstatus = NAND_VALID_ADDRESS;
  uint32_t status = NAND_READY, size = 0x00;

  while((NumSpareAreaToRead != 0x0) && (addressstatus == NAND_VALID_ADDRESS))
  {
    /* Page Read command and page address */
    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_READ_1;

    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00;
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x08;
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);

    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_READ_TRUE;

    /* ¶ÁÃ¦½Å */
//    while( GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0 );
//    while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );

    /* Data Read */
    size = NAND_SPARE_AREA_SIZE +  (NAND_SPARE_AREA_SIZE * numsparearearead);

    /* Get Data into Buffer */
    for ( ;index < size; index++)
    {
      pBuffer[index] = *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA);
    }

    numsparearearead++;

    NumSpareAreaToRead--;

    /* Calculate page address */
    addressstatus = FSMC_NAND_AddressIncrement(&Address);
  }

  status = FSMC_NAND_GetStatus();

  return (status | addressstatus);
}

/******************************************************************************
* Function Name  : FSMC_NAND_EraseBlock
* Description    : This routine erase complete block from NAND FLASH
* Input          : - Address: Any address into block to be erased
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_EraseBlock(NAND_ADDRESS Address)
{
  *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_ERASE0;

  *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
  *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);

  *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_ERASE1;


//  while( GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0 );
//  while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );

  return (FSMC_NAND_GetStatus());
}

/******************************************************************************
* Function Name  : FSMC_NAND_Reset
* Description    : This routine reset the NAND FLASH
* Input          : None
* Output         : None
* Return         : NAND_READY
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_Reset(void)
{
  *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_RESET;

  return (NAND_READY);
}

/******************************************************************************
* Function Name  : FSMC_NAND_GetStatus
* Description    : Get the NAND operation status
* Input          : None
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_GetStatus(void)
{
  uint32_t timeout = 0x1000000, status = NAND_READY;

  status = FSMC_NAND_ReadStatus();

  /* Wait for a NAND operation to complete or a TIMEOUT to occur */
  while ((status != NAND_READY) &&( timeout != 0x00))
  {
     status = FSMC_NAND_ReadStatus();
     timeout --;
  }

  if(timeout == 0x00)
  {
    status =  NAND_TIMEOUT_ERROR;
  }

  /* Return the operation status */
  return (status);
}
/******************************************************************************
* Function Name  : FSMC_NAND_ReadStatus
* Description    : Reads the NAND memory status using the Read status command
* Input          : None
* Output         : None
* Return         : The status of the NAND memory. This parameter can be:
*                   - NAND_BUSY: when memory is busy
*                   - NAND_READY: when memory is ready for the next operation
*                   - NAND_ERROR: when the previous operation gererates error
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_ReadStatus(void)
{
  uint32_t data = 0x00, status = NAND_BUSY;

  /* Read status operation ------------------------------------ */
  *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_STATUS;
  data = *(vu8 *)(NAND_FLASH_START_ADDR);

  if((data & NAND_ERROR) == NAND_ERROR)
  {
    status = NAND_ERROR;
  }
  else if((data & NAND_READY) == NAND_READY)
  {
    status = NAND_READY;
  }
  else
  {
    status = NAND_BUSY;
  }

  return (status);
}

/******************************************************************************
* Function Name  : NAND_AddressIncrement
* Description    : Increment the NAND memory address
* Input          : - Address: address to be incremented.
* Output         : None
* Return         : The new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_AddressIncrement(NAND_ADDRESS* Address)
{
  uint32_t status = NAND_VALID_ADDRESS;

  Address->Page++;

  if(Address->Page == NAND_BLOCK_SIZE)
  {
    Address->Page = 0;
    Address->Block++;

    if(Address->Block == NAND_ZONE_SIZE)
    {
      Address->Block = 0;
      Address->Zone++;

      if(Address->Zone == NAND_MAX_ZONE)
      {
        status = NAND_INVALID_ADDRESS;
      }
    }
  }

  return (status);
}

/******************************************************************************
* Function Name  : FSMC_NAND_Test
* Description    : NAND test
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void FMC_NAND_Test(void)
{
//  uint16_t index;
//  uint16_t j;
  NAND_IDTypeDef NAND_ID;
//  NAND_ADDRESS WriteReadAddr;

  FMC_NANDDeInit(FMC_Bank2_NAND);
  FMC_NAND_Init();
  printf("INIT\r\n");

  /* ¼ì²âNAND Flash */
  FMC_NAND_ReadID(&NAND_ID);
  printf("Nand Flash ID = %02X,%02X,%02X,%02X  ",NAND_ID.Maker_ID, NAND_ID.Device_ID,
		                                         NAND_ID.Third_ID, NAND_ID.Fourth_ID );
  if ((NAND_ID.Maker_ID == 0xEC) && (NAND_ID.Device_ID == 0xF1)
		&& (NAND_ID.Third_ID == 0x80) && (NAND_ID.Fourth_ID == 0x15))
  {
	 printf("Type = K9F1G08U0A\r\n");
  }
  else if ((NAND_ID.Maker_ID == 0xEC) && (NAND_ID.Device_ID == 0xF1)
		&& (NAND_ID.Third_ID == 0x00) && (NAND_ID.Fourth_ID == 0x95))
  {
	 printf("Type = K9F1G08U0B\r\n");
  }
  else if ((NAND_ID.Maker_ID == 0xAD) && (NAND_ID.Device_ID == 0xF1)
		&& (NAND_ID.Third_ID == 0x80) && (NAND_ID.Fourth_ID == 0x1D))
  {
	 printf("Type = HY27UF081G2A\r\n");
  }
  else
  {
	 printf("Type = Unknow\r\n");
  }

//  /* NAND memory address to write to */
//  WriteReadAddr.Zone = 0x00;
//  WriteReadAddr.Block = 0x00;
//  WriteReadAddr.Page = 0x00;
//
//
//  /* Erase the NAND first Block */
//  FSMC_NAND_EraseBlock(WriteReadAddr);
//
//  /* Write data to FSMC NOR memory */
//  /* Fill the buffer to send */
//  for (index = 0; index < NAND_PAGE_SIZE; index++ )
//  {
//     TxBuffer[index] = index;
//  }
//
//  FSMC_NAND_WriteSmallPage(TxBuffer, WriteReadAddr, 1);
//   printf("\r\nWritten to the number of£º \r\n");
//   for(j = 0; j < 100; j++)
//	printf("%x  \r",TxBuffer[j]);
//
//  /* Read back the written data */
//  FSMC_NAND_ReadSmallPage (RxBuffer, WriteReadAddr, 1);
//     printf("\r\nRead several£º \r\n");
//    for(j = 0; j < 100; j++)
//	printf("%x  \r",RxBuffer[j]);
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
