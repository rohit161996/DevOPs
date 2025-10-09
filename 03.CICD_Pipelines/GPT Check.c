/** @}
  * @addtogroup HAL_ETH_Functional_Test_Functions
  * @{
  */
/**
  * @brief HAL_ETH_MF_Test01: ETH Transmit Frame test
	* @par Test Description: In this test we do the following:
	* - start ETH MAC and DMA transmission
	* - enable transmission completed interrupt
	* - transmit a frame
	* - wait for transmission to be completed (5s timeout)
  * @par Called functions
	* - HAL_ETH_TransmitFrame()
	* - __HAL_ETH_DMA_ENABLE_IT()
  * @retval
  *  0: Test passed
  *  Value different from 0: Test failed
  */
uint32_t HAL_ETH_MF_Test01(void)
{
  uint32_t i=0;
  uint8_t macaddress[6]= { 0x0, 0x80, 0xE1, 0x01, 0x39, 0x61 };
  ETH_TxPacketConfigTypeDef TxConfig;
  ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT];
  HAL_StatusTypeDef status;

  printf("\n\r%s \r\n", __func__);

  /* Initialize the Tx_Complete and Rx_Complete Variables */
  Tx_Complete = 0;
  Rx_Complete = 0;

  /* Configure the Ethernet Handle(EthHandle)
   * 1. Instance = ETH1
   * 2. MAC Address = 0x0, 0x80, 0xE1, 0x01, 0x39, 0x61
   * 3. MODE = RGMII Mode
   * 4. DMA Descriptors = Rx and Tx Descriptors
   * 5. Rx Buffer Length = 1528
   */
  EthHandle.Instance = EthInstance;
  EthHandle.Init.MACAddr = macaddress;
#ifdef ETH_RMII_MODE_VALID
  EthHandle.Init.MediaInterface = HAL_ETH_RMII_MODE;
#endif
#ifdef ETH_RGMII_MODE_VALID
  EthHandle.Init.MediaInterface = HAL_ETH_RGMII_MODE;
#endif
#ifdef ETH_MII_MODE_VALID
  EthHandle.Init.MediaInterface = HAL_ETH_MII_MODE;
#endif

  EthHandle.Init.RxDesc = DMARxDscrTab;
  EthHandle.Init.TxDesc = DMATxDscrTab;
  EthHandle.Init.RxBuffLen = ETH_MAX_PACKET_SIZE;
	
  /* Configure Ethernet peripheral (GPIOs, clocks, MAC, DMA) */
//  HAL_ETH_DeInit(&EthHandle);
  /* Initialize the Ethernet Handle */
  status = HAL_ETH_Init(&EthHandle);
  if (status != HAL_OK){
    Error_Handler();
  }

  /* Init PHY */
  Eth_PhyInit(&EthHandle,PHY_ADDR);

#if defined(TEST_POLLING)
  status = HAL_ETH_Start(&EthHandle);
  if (status != HAL_OK)
    Error_Handler();
#endif
#if defined(TEST_IT)
  status = HAL_ETH_Start_IT(&EthHandle);
  if (status != HAL_OK)
    Error_Handler();
#endif
  
  Txbuffer[0].buffer = Tx_Buff[0];
  Txbuffer[0].len = 128;
  Txbuffer[0].next = NULL;
  Txbuffer[0].next = &Txbuffer[1];

  Txbuffer[1].buffer = Tx_Buff[1];
  Txbuffer[1].len = 400;
  Txbuffer[1].next = &Txbuffer[2];

  Txbuffer[2].buffer = Tx_Buff[2];
  Txbuffer[2].len = 400;
  Txbuffer[2].next = NULL;
  
  for(i=0; i < 400; i++)
  {
    Tx_Buff[0][i]= 0xFF;
    Tx_Buff[1][i]= 0xFF;
    Tx_Buff[2][i]= 0xFF;
  }
  
  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfigTypeDef));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_SAIC | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.SrcAddrCtrl = ETH_SRC_ADDR_REPLACE;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  TxConfig.Length = 128;
  TxConfig.TxBuffer = Txbuffer;  
  
#if defined(TEST_POLLING)
  status = HAL_ETH_Transmit(&EthHandle, &TxConfig, 1000);
  if (status != HAL_OK)
    Error_Handler();
#endif

#if defined(TEST_IT)
  printf("Starting TX ...\r\n");
  status = HAL_ETH_Transmit_IT(&EthHandle, &TxConfig);
  if (status != HAL_OK)
    while(1);

  while(Tx_Complete==0)
  {
    /* Add a timeout*/
  }
#endif
  printf("TX complete...\r\n");

  answer = 0;
  while ( (answer != ASCII_y) )
  {
    printf("\n\r*** Continue ? [y] ***\r\n");
    answer = Serial_Scanf(255);
  }

  /* If the Test passed */
  return 0;
}





void Eth_PhyInit(ETH_HandleTypeDef *Geth,uint32_t Phyaddr)
{
  uint32_t val = 0x0;;
  uint32_t val_cr = 0x0;
#if defined(USE_PHY_RTL8211F) || defined(USE_PHY_RTL8211E)
  // Phy reset
  printf("Phy reset...\n\r");
  /* Enable Phy reset */
  val_cr = 0x01 << 15;
  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x00,val_cr);
  HAL_Delay(20*PHY_DELAY);
  HAL_ETH_ReadPHYRegister(Geth,Phyaddr,0x00,&val);    // 0x1040  0001 0000 0100 0000  Giga Auto Nego

  // Auto-negociation
  /* Enable Auto negotiation */
  val_cr = val | (0x01 << 12);
  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x00,val_cr);
  HAL_ETH_ReadPHYRegister(Geth,Phyaddr,0x00,&val);  // 0x2100 0010 0001 0000 0000 Full duplex 100MBps

  /* BMSR */
  val = 0x0;
  while((val & 0x0020) != 0x0020)
  {
    /* Get PHY Status (BMSR) */
    HAL_ETH_ReadPHYRegister(Geth,Phyaddr,0x01,&val);
    HAL_Delay(PHY_DELAY);
  }
  printf("Auto-negociation ok - val=%#.2lx\n\r", val);

#if defined(USE_PHY_RTL8211F)

  /* Manage RX/TX Delay */
#if defined(USE_STM32MP257F_EV1) || defined(USE_STM32MP257CXX_VALID3)

  /* Set PHY Page */
  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x1F,0xd08);

//  /* Set PHY Tx Delay */
//  HAL_ETH_ReadPHYRegister(Geth,Phyaddr,0x11,&val);
//  val_cr = val | (0x1 << 8);
//  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x11,val_cr);
//  HAL_ETH_ReadPHYRegister(Geth,Phyaddr,0x11,&val);
//  printf("MIICR1 : TX Delay enabled - val=%#.2lx\n\r", val);

  /* Set PHY Rx Delay */
#if defined (ETH2_INSTANCE) || (defined (USE_STM32MP257CXX_VALID3) && defined(ETH1_INSTANCE))
  HAL_ETH_ReadPHYRegister(Geth,Phyaddr,0x15,&val);
  val_cr = val | (0x1 << 3);
  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x15, val_cr);
  HAL_ETH_ReadPHYRegister(Geth,Phyaddr,0x15,&val);
  printf("MIICR2 : RX Delay enabled - val=%#.2lx\n\r", val);
#endif
  /* Restore Default Page */
  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x1F,0x0);

#endif

//  /* Disable EEE (Energy-Efficient Ethernet) feature */
//  /* Set PHY Page */
//  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x1F,0xa43);
//  HAL_ETH_ReadPHYRegister(Geth,Phyaddr,0x19,&val);
//  val_cr = val & (~(0x1 << 5));
//  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x19,val_cr);
//  HAL_ETH_ReadPHYRegister(Geth,Phyaddr,0x19,&val);
//  printf("PHYCR2: Disable EEE - val=%#.2lx\n\r", val);
//  /* Restore Default Page */
//  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x1F,0x0);

  //#if defined(DISABLE_RTL8211F_EEE) && (DISABLE_RTL8211F_EEE==1)
  //    printf("Disable EEE...\n\r");
  //    /* Select page a4b */
  //    /* Set PHY Page */
  //    HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x1F,0xa4b);
  //    HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x11,0x1110);
  //    /* Select page 0 */
  //    HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x1F,0x0);
  //    /* Set address mode and MMD Device=7 */
  //    HAL_ETH_WritePHYRegister(Geth,Phyaddr,0xD,0x7);
  //    /*  Set address value */
  //    HAL_ETH_WritePHYRegister(Geth,Phyaddr,0xE,0x3c);
  //    /* Set data mode and MMD Device=7 */
  //    HAL_ETH_WritePHYRegister(Geth,Phyaddr,0xE,0x4007);
  //    /*  Set data */
  //    HAL_ETH_WritePHYRegister(Geth,Phyaddr,0xE,0x0);
  //  /* Restore Default Page */
  //  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x1F,0x0);
  //#endif

  // Check the link status
  /* Set PHY Page */
  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x1F,0xa43);

  /* Read IT status */
  HAL_ETH_ReadPHYRegister(Geth,Phyaddr,0x1D,&val);
  printf("Read IT status - val=%#.2lx\n\r", val);

  /* Restore Default Page */
  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x1F,0x0);

  val = 0x0;
  while((val & 0x0004) != 0x0004)
  {
    /* Get PHY Status (PHYSR1) */
    HAL_ETH_ReadPHYRegister(Geth,Phyaddr,0x1A,&val);
    HAL_Delay(PHY_DELAY);
  }
  printf("Link status ok - val=%#.2lx\n\r", val);

  /* Manage LEDs */
  /* Set PHY Page */
  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x1F,0xd04);
  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x10,0x617f);
  HAL_ETH_ReadPHYRegister(Geth,Phyaddr,0x10,&val);
  printf("LCR: LEDs enabled - val=%#.2lx\n\r", val);
  /* Restore Default Page */
  HAL_ETH_WritePHYRegister(Geth,Phyaddr,0x1F,0x0);

#endif /* USE_PHY_RTL8211F */

#else
#warning "Please add PHY init and PHY_ADDR value..."
#endif /* defined(USE_PHY_RTL8211F) || defined(USE_PHY_RTL8211E) */
}


/**
  * @brief  Initialize the Ethernet peripheral registers.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_Init(ETH_HandleTypeDef *heth)
{
  uint32_t tickstart;

  if (heth == NULL)
  {
    return HAL_ERROR;
  }
  if (heth->gState == HAL_ETH_STATE_RESET)
  {
    heth->gState = HAL_ETH_STATE_BUSY;

#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)

    ETH_InitCallbacksToDefault(heth);

    if (heth->MspInitCallback == NULL)
    {
      heth->MspInitCallback = HAL_ETH_MspInit;
    }

    /* Init the low level hardware */
    heth->MspInitCallback(heth);
#else
    /* Init the low level hardware : GPIO, CLOCK, NVIC. */
    HAL_ETH_MspInit(heth);

#endif /* (USE_HAL_ETH_REGISTER_CALLBACKS) */
  }


  if (heth->Init.MediaInterface == HAL_ETH_MII_MODE)
  {
    if (heth->Instance == ETH1)
    {
      HAL_SYSCFG_ETH1InterfaceSelect(SYSCFG_ETH_MII);
    }
    else
    {
      HAL_SYSCFG_ETH2InterfaceSelect(SYSCFG_ETH_MII);
    }
  }
  else if (heth->Init.MediaInterface == HAL_ETH_RGMII_MODE)
  {
    if (heth->Instance == ETH1)
    {
      HAL_SYSCFG_ETH1InterfaceSelect(SYSCFG_ETH_RGMII);
    }
    else
    {
      HAL_SYSCFG_ETH2InterfaceSelect(SYSCFG_ETH_RGMII);
    }
  }
  else if (heth->Init.MediaInterface == HAL_ETH_GMII_MODE)
  {
  }
  else
  {
    if (heth->Instance == ETH1)
    {
      HAL_SYSCFG_ETH1InterfaceSelect(SYSCFG_ETH_RMII);
    }
    else
    {
      HAL_SYSCFG_ETH2InterfaceSelect(SYSCFG_ETH_RMII);
    }
  }

  /* Dummy read to sync with ETH */
  (void)SYSCFG->ETH1CR;
  (void)SYSCFG->ETH2CR;

  /* Ethernet Software reset */
  /* Set the SWR bit: resets all MAC subsystem internal registers and logic */
  /* After reset all the registers holds their respective reset values */
  SET_BIT(heth->Instance->DMAMR, ETH_DMAMR_SWR);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait for software reset */
  while (READ_BIT(heth->Instance->DMAMR, ETH_DMAMR_SWR) > 0U)
  {
    if (((HAL_GetTick() - tickstart) > ETH_SWRESET_TIMEOUT))
    {
      /* Set Error Code */
      heth->ErrorCode = HAL_ETH_ERROR_TIMEOUT;
      /* Set State as Error */
      heth->gState = HAL_ETH_STATE_ERROR;
      /* Return Error */
      return HAL_ERROR;
    }
  }

  /*------------------ MDIO CSR Clock Range Configuration --------------------*/
  HAL_ETH_SetMDIOClockRange(heth);

  /*------------------ MAC LPI 1US Tic Counter Configuration --------------------*/
  WRITE_REG(heth->Instance->MAC1USTCR, (((uint32_t)HAL_RCC_GetFreq(RCC_CLOCKTYPE_ICN_LS_MCU) / ETH_MAC_US_TICK) - 1U));

  /*------------------ MAC, MTL and DMA default Configuration ----------------*/
  ETH_MACDMAConfig(heth);

  /* SET DSL to 64 bit */
  MODIFY_REG(heth->Instance->DMAC0CR, ETH_DMAC0CR_DSL, ETH_DMACCR_DSL_64BIT);

  /* Set Receive Buffers Length (must be a multiple of 4) */
  if ((heth->Init.RxBuffLen % 0x4U) != 0x0U)
  {
    /* Set Error Code */
    heth->ErrorCode = HAL_ETH_ERROR_PARAM;
    /* Set State as Error */
    heth->gState = HAL_ETH_STATE_ERROR;
    /* Return Error */
    return HAL_ERROR;
  }
  else
  {
    MODIFY_REG(heth->Instance->DMAC0RXCR, ETH_DMAC0RXCR_RBSZ, ((heth->Init.RxBuffLen) << 1));
  }

  /*------------------ DMA Tx Descriptors Configuration ----------------------*/
  ETH_DMATxDescListInit(heth);

  /*------------------ DMA Rx Descriptors Configuration ----------------------*/
  ETH_DMARxDescListInit(heth);

  /*--------------------- ETHERNET MAC Address Configuration ------------------*/
  /* Set MAC addr bits 32 to 47 */
  heth->Instance->MACA0HR = (((uint32_t)(heth->Init.MACAddr[5]) << 8) | (uint32_t)heth->Init.MACAddr[4]);
  /* Set MAC addr bits 0 to 31 */
  heth->Instance->MACA0LR = (((uint32_t)(heth->Init.MACAddr[3]) << 24) | ((uint32_t)(heth->Init.MACAddr[2]) << 16) |
                             ((uint32_t)(heth->Init.MACAddr[1]) << 8) | (uint32_t)heth->Init.MACAddr[0]);

  /* Disable Rx MMC Interrupts */
  SET_BIT(heth->Instance->MMCRXIMR, ETH_MMCRXIMR_RXLPITRCIM | ETH_MMCRXIMR_RXLPIUSCIM | \
          ETH_MMCRXIMR_RXUCGPIM | ETH_MMCRXIMR_RXALGNERPIM | ETH_MMCRXIMR_RXCRCERPIM);

  /* Disable Tx MMC Interrupts */
  SET_BIT(heth->Instance->MMCTXIMR, ETH_MMCTXIMR_TXLPITRCIM | ETH_MMCTXIMR_TXLPIUSCIM | \
          ETH_MMCTXIMR_TXGPKTIM | ETH_MMCTXIMR_TXMCOLGPIM | ETH_MMCTXIMR_TXSCOLGPIM);

  heth->ErrorCode = HAL_ETH_ERROR_NONE;
  heth->gState = HAL_ETH_STATE_READY;

  return HAL_OK;
}


/**
  * @brief  Enables Ethernet MAC and DMA reception/transmission in Interrupt mode
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_Start_IT(ETH_HandleTypeDef *heth)
{
  if (heth->gState == HAL_ETH_STATE_READY)
  {
    heth->gState = HAL_ETH_STATE_BUSY;

    /* save IT mode to ETH Handle */
    heth->RxDescList.ItMode = 1U;

    /* Set number of descriptors to build */
    heth->RxDescList.RxBuildDescCnt = ETH_RX_DESC_CNT;

    /* Build all descriptors */
    ETH_UpdateDescriptor(heth);

    /* Enable the DMA transmission */
    SET_BIT(heth->Instance->DMAC0TXCR, ETH_DMAC0TXCR_ST);

    /* Enable the DMA reception */
    SET_BIT(heth->Instance->DMAC0RXCR, ETH_DMAC0RXCR_SR);

    /* Clear Tx and Rx process stopped flags */
    heth->Instance->DMAC0SR |= (ETH_DMAC0SR_TPS | ETH_DMAC0SR_RPS);

    /* Set the Flush Transmit FIFO bit */
    SET_BIT(heth->Instance->MTLTXQ0OMR, ETH_MTLTXQ0OMR_FTQ);


    /* Enable the MAC transmission */
    SET_BIT(heth->Instance->MACCR, ETH_MACCR_TE);

    /* Enable the MAC reception */
    SET_BIT(heth->Instance->MACCR, ETH_MACCR_RE);

    /* Enable ETH DMA interrupts:
    - Tx complete interrupt
    - Rx complete interrupt
    - Fatal bus interrupt
    */
    __HAL_ETH_DMA_ENABLE_IT(heth, (ETH_DMAC0IER_NIE | ETH_DMAC0IER_RIE | ETH_DMAC0IER_TIE  |
                                   ETH_DMAC0IER_FBEE | ETH_DMAC0IER_AIE | ETH_DMAC0IER_RBUE));

    heth->gState = HAL_ETH_STATE_STARTED;
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }
}


/**
  * @brief  Sends an Ethernet Packet in interrupt mode.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  pTxConfig: Hold the configuration of packet to be transmitted
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_Transmit_IT(ETH_HandleTypeDef *heth, ETH_TxPacketConfigTypeDef *pTxConfig)
{
  if (pTxConfig == NULL)
  {
    heth->ErrorCode |= HAL_ETH_ERROR_PARAM;
    return HAL_ERROR;
  }

  if (heth->gState == HAL_ETH_STATE_STARTED)
  {
    /* Save the packet pointer to release.  */
    heth->TxDescList.CurrentPacketAddress = (uint32_t *)pTxConfig->pData;

    /* Config DMA Tx descriptor by Tx Packet info */
    if (ETH_Prepare_Tx_Descriptors(heth, pTxConfig, 1) != HAL_ETH_ERROR_NONE)
    {
      heth->ErrorCode |= HAL_ETH_ERROR_BUSY;
      return HAL_ERROR;
    }

    /* Ensure completion of descriptor preparation before transmission start */
    __DSB();

    /* Incr current tx desc index */
    INCR_TX_DESC_INDEX(heth->TxDescList.CurTxDesc, 1U);

    /* Start transmission */
    /* issue a poll command to Tx DMA by writing address of next immediate free descriptor */
    WRITE_REG(heth->Instance->DMAC0TXDTPR, (uint32_t)(heth->TxDescList.TxDesc[heth->TxDescList.CurTxDesc]));

    return HAL_OK;

  }
  else
  {
    return HAL_ERROR;
  }
}



/*
******************************************************************************
**
**  File        : LinkerScript.ld
**
**  Abstract    : Linker script for STM32MP1 series
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed �as is,� without any warranty
**                of any kind.
**
*****************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
*****************************************************************************
*/


/* Entry Point */
ENTRY(Reset_Handler)

_Min_Heap_Size = 0x200;     /* required amount of heap  */
_Min_Stack_Size = 0x400;    /* required amount of stack */

TTB_L1_SIZE = 16384;        /* MMU Level 1 Translation table size */
TTB_L2_SIZE = 1024;         /* MMU Level 2 Translation table size */

MEMORY {
    SYSRAM_BASE   (rx)  : ORIGIN = 0x30000000, LENGTH = 0x4000
    SRAM1  (rwx) : ORIGIN = 0x30040000, LENGTH = 0x20000
    SRAM2  (rwx) : ORIGIN = 0x30060000, LENGTH = 0x20000
    DDR_BASE (rwx)      : ORIGIN = 0x80000000, LENGTH = 1024M
    Memory_SYSRAM_B1(xrw)   : ORIGIN = 0x3003F000, LENGTH = 0x60
    Memory_SYSRAM_B2(xrw)   : ORIGIN = 0x3003F060, LENGTH = 0x60
    Memory_SRAM_B1(xrw)   : ORIGIN = 0x30060000, LENGTH = 0x60
    Memory_SRAM_B2(xrw)   : ORIGIN = 0x30060060, LENGTH = 0x60
    Memory_DDR_B1(xrw)   : ORIGIN = 0x90000000, LENGTH = 0x60
    Memory_DDR_B2(xrw)   : ORIGIN = 0x90000060, LENGTH = 0x60
}

/* Select Memory in which code has to be placed and offset of code from start of this memory */ 
REGION_ALIAS("RAM", DDR_BASE);
START_OFFSET = 0x2000;

/* Code Memory */
__MEM_START__ = ORIGIN(RAM) + START_OFFSET;
__MEM_SIZE__  = LENGTH(RAM) - START_OFFSET;
__FIQ_STACK_SIZE = 0x100;
__IRQ_STACK_SIZE = 0x100;
__ABT_STACK_SIZE = 0x100;
__SVC_STACK_SIZE = 0x100;
__UND_STACK_SIZE = 0x100;


/* put the stacks at the end of the memory */
FIQ_STACK = __MEM_START__ + __MEM_SIZE__;
IRQ_STACK = FIQ_STACK - __FIQ_STACK_SIZE;
ABT_STACK = IRQ_STACK - __IRQ_STACK_SIZE;
SVC_STACK = ABT_STACK - __ABT_STACK_SIZE;
UND_STACK = SVC_STACK - __SVC_STACK_SIZE;
SYS_STACK = UND_STACK - __UND_STACK_SIZE;

SECTIONS
{
    . = __MEM_START__;
    .RESET . : {
        . = ALIGN(7);
        __TEXT_START__ = .;
        * (RESET)
        . = ALIGN(7);
        *(.text*)
        
        /* .init is used by libc_nano */
        KEEP (*(.init))
        KEEP (*(.fini))

        __TEXT_END_UNALIGNED__ = .;
        
        /*
         * Memory page(s) mapped to this section will be marked as
         * read-only, executable.  No non-executable data from the next section must
         * creep in.  Ensure the rest of the current memory page is unused.
         */
        __TEXT_END__ = .;
        
        . = NEXT(4096);

        __RO_START__ = .;
        *(.rodata*)

        __RO_END_UNALIGNED__ = .;
        /*
         * Memory page(s) mapped to this section will be marked as
         * read-only, non-executable.  No RW data from the next section must
         * creep in.  Ensure the rest of the current memory page is unused.
         */
        . = NEXT(4096);
        __RO_END__ = .;
    } >RAM


    .ARM.extab   : {
    . = ALIGN(4);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
    } >RAM

    .ARM : {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
    } >RAM

    /* .init_array is used by libc_nano */

    .preinit_array     :
    {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
    } >RAM

    .init_array :
    {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
    } >RAM

    .fini_array :
    {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
    } >RAM


    .data . : {
        RW_DATA = .;
        *(.data*)
        __DATA_END__ = .;
    } >RAM

    /*
     * The .bss section gets initialised to 0 at runtime.
     * Its base address must be 16-byte aligned.
     */
    .bss : ALIGN(32) {
        ZI_DATA = .;
        *(SORT_BY_ALIGNMENT(.bss*))
        *(COMMON)
        . = ALIGN(32);
        __BSS_END__ = .;
    } >RAM

    __BSS_SIZE__ = SIZEOF(.bss);

/* 1 TTB Level 1 table */
/* 4 TTB Level 2 tables */
    ._TTB : ALIGN(16384) {
       TTB = .;
       . = . + TTB_L1_SIZE;
       . = . + TTB_L2_SIZE * 4;
    } > RAM


    /* User_heap_stack section, used to check that there is enough RAM left */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } >RAM

	.RxDecripSection : { *(.DMARxDscrTab_section) } >Memory_DDR_B1
	.TxDecripSection : { *(.DMATxDscrTab_section) } >Memory_DDR_B2


  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }
}




/* Linker script to configure memory regions. */
MEMORY
{
  SYSRAM_BASE   (rx)  : ORIGIN = 0x30000000, LENGTH = 0x4000
  SRAM1  (rwx) : ORIGIN = 0x30040000, LENGTH = 0x20000
  SRAM2  (rwx) : ORIGIN = 0x30060000, LENGTH = 0x20000
  DDR_BASE  (rwx) : ORIGIN = 0x80000000, LENGTH = 1024M
  Memory_SYSRAM_B1(xrw)   : ORIGIN = 0x3003F000, LENGTH = 0x60
  Memory_SYSRAM_B2(xrw)   : ORIGIN = 0x3003F060, LENGTH = 0x60
  Memory_SRAM_B1(xrw)   : ORIGIN = 0x30060000, LENGTH = 0x60
  Memory_SRAM_B2(xrw)   : ORIGIN = 0x30060060, LENGTH = 0x60
  Memory_DDR_B1(xrw)   : ORIGIN = 0x90000000, LENGTH = 0x60
  Memory_DDR_B2(xrw)   : ORIGIN = 0x90000060, LENGTH = 0x60
}


/*REGION_ALIAS("RAM", SYSRAM_BASE);*/
REGION_ALIAS("RAM", DDR_BASE);

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapBase
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __Vectors_End
 *   __Vectors_Size
 */
ENTRY(Reset_Handler)

SECTIONS
{
	.text :
	{
		KEEP(*(.vectors))
		__Vectors_End = .;
		__Vectors_Size = __Vectors_End - __Vectors;
		__end__ = .;

		*(.text*)

		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
 		*crtbegin.o(.dtors)
 		*crtbegin?.o(.dtors)
 		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
 		*(SORT(.dtors.*))
 		*(.dtors)

		*(.rodata*)

		KEEP(*(.eh_frame*))
	} > RAM

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > RAM

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > RAM
	__exidx_end = .;

	/* To copy multiple RAM to RAM sections,
	 * uncomment .copy.table section and,
	 * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
	/*
	.copy.table :
	{
		. = ALIGN(4);
		__copy_table_start__ = .;
		LONG (__etext)
		LONG (__data_start__)
		LONG (__data_end__ - __data_start__)
		LONG (__etext2)
		LONG (__data2_start__)
		LONG (__data2_end__ - __data2_start__)
		__copy_table_end__ = .;
	} > RAM
	*/

	/* To clear multiple BSS sections,
	 * uncomment .zero.table section and,
	 * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
	/*
	.zero.table :
	{
		. = ALIGN(4);
		__zero_table_start__ = .;
		LONG (__bss_start__)
		LONG (__bss_end__ - __bss_start__)
		LONG (__bss2_start__)
		LONG (__bss2_end__ - __bss2_start__)
		__zero_table_end__ = .;
	} > RAM
	*/

	__etext = .;

	.data : AT (__etext)
	{
		__data_start__ = .;
		*(vtable)
		*(.data*)

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);


		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;

	} > RAM

	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} > RAM

	.heap (COPY):
	{
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		KEEP(*(.heap*))
		__HeapLimit = .;
	} > RAM

	.RxDecripSection : { *(.DMARxDscrTab_section) } >Memory_DDR_B1
	.TxDecripSection : { *(.DMATxDscrTab_section) } >Memory_DDR_B2

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (COPY):
	{
		KEEP(*(.stack*))
	} > RAM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(RAM) + LENGTH(RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}

