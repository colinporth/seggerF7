#include "stm32f7xx_hal.h"

typedef struct {
  __IO uint32_t ISR;   /*!< DMA interrupt status register */
  __IO uint32_t Reserved0;
  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
  } DMA_Base_Registers;

#define HAL_TIMEOUT_DMA_ABORT    ((uint32_t)5)  /* 5 ms */

//{{{
static void DMA_SetConfig (DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
  /* Clear DBM bit */
  hdma->Instance->CR &= (uint32_t)(~DMA_SxCR_DBM);

  /* Configure DMA Stream data length */
  hdma->Instance->NDTR = DataLength;

  /* Memory to Peripheral */
  if((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH)
  {
    /* Configure DMA Stream destination address */
    hdma->Instance->PAR = DstAddress;

    /* Configure DMA Stream source address */
    hdma->Instance->M0AR = SrcAddress;
  }
  /* Peripheral to Memory */
  else
  {
    /* Configure DMA Stream source address */
    hdma->Instance->PAR = SrcAddress;

    /* Configure DMA Stream destination address */
    hdma->Instance->M0AR = DstAddress;
  }
}
//}}}
//{{{
static uint32_t DMA_CalcBaseAndBitshift (DMA_HandleTypeDef *hdma)
{
  uint32_t stream_number = (((uint32_t)hdma->Instance & 0xFFU) - 16U) / 24U;

  /* lookup table for necessary bitshift of flags within status registers */
  static const uint8_t flagBitshiftOffset[8U] = {0U, 6U, 16U, 22U, 0U, 6U, 16U, 22U};
  hdma->StreamIndex = flagBitshiftOffset[stream_number];

  if (stream_number > 3U)
  {
    /* return pointer to HISR and HIFCR */
    hdma->StreamBaseAddress = (((uint32_t)hdma->Instance & (uint32_t)(~0x3FFU)) + 4U);
  }
  else
  {
    /* return pointer to LISR and LIFCR */
    hdma->StreamBaseAddress = ((uint32_t)hdma->Instance & (uint32_t)(~0x3FFU));
  }

  return hdma->StreamBaseAddress;
}
//}}}
//{{{
static HAL_StatusTypeDef DMA_CheckFifoParam (DMA_HandleTypeDef *hdma)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t tmp = hdma->Init.FIFOThreshold;

  /* Memory Data size equal to Byte */
  if(hdma->Init.MemDataAlignment == DMA_MDATAALIGN_BYTE)
  {
    switch (tmp)
    {
    case DMA_FIFO_THRESHOLD_1QUARTERFULL:
    case DMA_FIFO_THRESHOLD_3QUARTERSFULL:
      if ((hdma->Init.MemBurst & DMA_SxCR_MBURST_1) == DMA_SxCR_MBURST_1)
      {
        status = HAL_ERROR;
      }
      break;
    case DMA_FIFO_THRESHOLD_HALFFULL:
      if (hdma->Init.MemBurst == DMA_MBURST_INC16)
      {
        status = HAL_ERROR;
      }
      break;
    case DMA_FIFO_THRESHOLD_FULL:
      break;
    default:
      break;
    }
  }

  /* Memory Data size equal to Half-Word */
  else if (hdma->Init.MemDataAlignment == DMA_MDATAALIGN_HALFWORD)
  {
    switch (tmp)
    {
    case DMA_FIFO_THRESHOLD_1QUARTERFULL:
    case DMA_FIFO_THRESHOLD_3QUARTERSFULL:
      status = HAL_ERROR;
      break;
    case DMA_FIFO_THRESHOLD_HALFFULL:
      if ((hdma->Init.MemBurst & DMA_SxCR_MBURST_1) == DMA_SxCR_MBURST_1)
      {
        status = HAL_ERROR;
      }
      break;
    case DMA_FIFO_THRESHOLD_FULL:
      if (hdma->Init.MemBurst == DMA_MBURST_INC16)
      {
        status = HAL_ERROR;
      }
      break;
    default:
      break;
    }
  }

  /* Memory Data size equal to Word */
  else
  {
    switch (tmp)
    {
    case DMA_FIFO_THRESHOLD_1QUARTERFULL:
    case DMA_FIFO_THRESHOLD_HALFFULL:
    case DMA_FIFO_THRESHOLD_3QUARTERSFULL:
      status = HAL_ERROR;
      break;
    case DMA_FIFO_THRESHOLD_FULL:
      if ((hdma->Init.MemBurst & DMA_SxCR_MBURST_1) == DMA_SxCR_MBURST_1)
      {
        status = HAL_ERROR;
      }
      break;
    default:
      break;
    }
  }

  return status;
}
//}}}

//{{{
HAL_StatusTypeDef HAL_DMA_Init (DMA_HandleTypeDef *hdma)
{
  uint32_t tmp = 0U;
  uint32_t tickstart = HAL_GetTick();
  DMA_Base_Registers *regs;

  /* Allocate lock resource */
  __HAL_UNLOCK(hdma);

  /* Change DMA peripheral state */
  hdma->State = HAL_DMA_STATE_BUSY;

  /* Disable the peripheral */
  __HAL_DMA_DISABLE(hdma);

  /* Check if the DMA Stream is effectively disabled */
  while((hdma->Instance->CR & DMA_SxCR_EN) != RESET) {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart ) > HAL_TIMEOUT_DMA_ABORT) {
      /* Update error code */
      hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;
      /* Change the DMA state */
      hdma->State = HAL_DMA_STATE_TIMEOUT;
      return HAL_TIMEOUT;
      }
    }

  /* Get the CR register value */
  tmp = hdma->Instance->CR;

  /* Clear CHSEL, MBURST, PBURST, PL, MSIZE, PSIZE, MINC, PINC, CIRC, DIR, CT and DBM bits */
  tmp &= ((uint32_t)~(DMA_SxCR_CHSEL | DMA_SxCR_MBURST | DMA_SxCR_PBURST | \
                      DMA_SxCR_PL    | DMA_SxCR_MSIZE  | DMA_SxCR_PSIZE  | \
                      DMA_SxCR_MINC  | DMA_SxCR_PINC   | DMA_SxCR_CIRC   | \
                      DMA_SxCR_DIR   | DMA_SxCR_CT     | DMA_SxCR_DBM));

  /* Prepare the DMA Stream configuration */
  tmp |=  hdma->Init.Channel             | hdma->Init.Direction        |
          hdma->Init.PeriphInc           | hdma->Init.MemInc           |
          hdma->Init.PeriphDataAlignment | hdma->Init.MemDataAlignment |
          hdma->Init.Mode                | hdma->Init.Priority;

  /* the Memory burst and peripheral burst are not used when the FIFO is disabled */
  if(hdma->Init.FIFOMode == DMA_FIFOMODE_ENABLE)
    /* Get memory burst and peripheral burst */
    tmp |=  hdma->Init.MemBurst | hdma->Init.PeriphBurst;

  /* Write to DMA Stream CR register */
  hdma->Instance->CR = tmp;

  /* Get the FCR register value */
  tmp = hdma->Instance->FCR;

  /* Clear Direct mode and FIFO threshold bits */
  tmp &= (uint32_t)~(DMA_SxFCR_DMDIS | DMA_SxFCR_FTH);

  /* Prepare the DMA Stream FIFO configuration */
  tmp |= hdma->Init.FIFOMode;

  /* The FIFO threshold is not used when the FIFO mode is disabled */
  if(hdma->Init.FIFOMode == DMA_FIFOMODE_ENABLE) {
    /* Get the FIFO threshold */
    tmp |= hdma->Init.FIFOThreshold;

    /* Check compatibility between FIFO threshold level and size of the memory burst for INCR4, INCR8, INCR16 bursts */
    if (hdma->Init.MemBurst != DMA_MBURST_SINGLE) {
      if (DMA_CheckFifoParam(hdma) != HAL_OK) {
        /* Update error code */
        hdma->ErrorCode = HAL_DMA_ERROR_PARAM;
        /* Change the DMA state */
        hdma->State = HAL_DMA_STATE_READY;
        return HAL_ERROR;
        }
      }
    }

  /* Write to DMA Stream FCR */
  hdma->Instance->FCR = tmp;

  /* Initialize StreamBaseAddress and StreamIndex parameters to be used to calculate
     DMA steam Base Address needed by HAL_DMA_IRQHandler() and HAL_DMA_PollForTransfer() */
  regs = (DMA_Base_Registers *)DMA_CalcBaseAndBitshift(hdma);

  /* Clear all interrupt flags */
  regs->IFCR = 0x3FU << hdma->StreamIndex;

  /* Initialize the error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

  /* Initialize the DMA state */
  hdma->State = HAL_DMA_STATE_READY;

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA_DeInit (DMA_HandleTypeDef *hdma)
{
  DMA_Base_Registers *regs;

  /* Check the DMA peripheral state */
  if(hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the DMA peripheral state */
  if(hdma->State == HAL_DMA_STATE_BUSY)
  {
    /* Return error status */
    return HAL_BUSY;
  }

  /* Check the parameters */
  assert_param(IS_DMA_STREAM_ALL_INSTANCE(hdma->Instance));

  /* Disable the selected DMA Streamx */
  __HAL_DMA_DISABLE(hdma);

  /* Reset DMA Streamx control register */
  hdma->Instance->CR   = 0U;

  /* Reset DMA Streamx number of data to transfer register */
  hdma->Instance->NDTR = 0U;

  /* Reset DMA Streamx peripheral address register */
  hdma->Instance->PAR  = 0U;

  /* Reset DMA Streamx memory 0 address register */
  hdma->Instance->M0AR = 0U;

  /* Reset DMA Streamx memory 1 address register */
  hdma->Instance->M1AR = 0U;

  /* Reset DMA Streamx FIFO control register */
  hdma->Instance->FCR  = (uint32_t)0x00000021U;

  /* Get DMA steam Base Address */
  regs = (DMA_Base_Registers *)DMA_CalcBaseAndBitshift(hdma);

  /* Clear all interrupt flags at correct offset within the register */
  regs->IFCR = 0x3FU << hdma->StreamIndex;

  /* Clean all callbacks */
  hdma->XferCpltCallback = NULL;
  hdma->XferHalfCpltCallback = NULL;
  hdma->XferM1CpltCallback = NULL;
  hdma->XferM1HalfCpltCallback = NULL;
  hdma->XferErrorCallback = NULL;
  hdma->XferAbortCallback = NULL;

  /* Reset the error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

  /* Reset the DMA state */
  hdma->State = HAL_DMA_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hdma);

  return HAL_OK;
}
//}}}

//{{{
HAL_StatusTypeDef HAL_DMA_Start (DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength) {

  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));

  /* Process locked */
  __HAL_LOCK(hdma);

  if (HAL_DMA_STATE_READY == hdma->State) {
    /* Change DMA peripheral state */
    hdma->State = HAL_DMA_STATE_BUSY;
    /* Initialize the error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;
    /* Configure the source, destination address and the data length */
    DMA_SetConfig(hdma, SrcAddress, DstAddress, DataLength);
    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(hdma);
    }

  else {
    /* Process unlocked */
    __HAL_UNLOCK(hdma);
    /* Return error status */
    status = HAL_BUSY;
    }

  return status;
  }
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA_Start_IT (DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength) {

  HAL_StatusTypeDef status = HAL_OK;

  /* calculate DMA base and stream number */
  DMA_Base_Registers *regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;

  /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));

  /* Process locked */
  __HAL_LOCK(hdma);

  if(HAL_DMA_STATE_READY == hdma->State) {
    /* Change DMA peripheral state */
    hdma->State = HAL_DMA_STATE_BUSY;

    /* Initialize the error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;

    /* Configure the source, destination address and the data length */
    DMA_SetConfig(hdma, SrcAddress, DstAddress, DataLength);

    /* Clear all interrupt flags at correct offset within the register */
    regs->IFCR = 0x3FU << hdma->StreamIndex;

    /* Enable Common interrupts*/
    hdma->Instance->CR  |= DMA_IT_TC | DMA_IT_TE | DMA_IT_DME;
    hdma->Instance->FCR |= DMA_IT_FE;

    if(hdma->XferHalfCpltCallback != NULL)
    {
      hdma->Instance->CR  |= DMA_IT_HT;
    }

    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(hdma);
    }
  else {
    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    /* Return error status */
    status = HAL_BUSY;
    }

  return status;
  }
//}}}

//{{{
HAL_StatusTypeDef HAL_DMA_Abort (DMA_HandleTypeDef *hdma)
{
  /* calculate DMA base and stream number */
  DMA_Base_Registers *regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;

  uint32_t tickstart = HAL_GetTick();

  if(hdma->State != HAL_DMA_STATE_BUSY)
  {
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }
  else
  {
    /* Disable all the transfer interrupts */
    hdma->Instance->CR  &= ~(DMA_IT_TC | DMA_IT_TE | DMA_IT_DME);
    hdma->Instance->FCR &= ~(DMA_IT_FE);

    if((hdma->XferHalfCpltCallback != NULL) || (hdma->XferM1HalfCpltCallback != NULL))
    {
      hdma->Instance->CR  &= ~(DMA_IT_HT);
    }

    /* Disable the stream */
    __HAL_DMA_DISABLE(hdma);

    /* Check if the DMA Stream is effectively disabled */
    while((hdma->Instance->CR & DMA_SxCR_EN) != RESET)
    {
      /* Check for the Timeout */
      if((HAL_GetTick() - tickstart ) > HAL_TIMEOUT_DMA_ABORT)
      {
        /* Update error code */
        hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;

        /* Process Unlocked */
        __HAL_UNLOCK(hdma);

        /* Change the DMA state */
        hdma->State = HAL_DMA_STATE_TIMEOUT;

        return HAL_TIMEOUT;
      }
    }

    /* Clear all interrupt flags at correct offset within the register */
    regs->IFCR = 0x3FU << hdma->StreamIndex;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    /* Change the DMA state*/
    hdma->State = HAL_DMA_STATE_READY;
  }
  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA_Abort_IT (DMA_HandleTypeDef *hdma)
{
  if(hdma->State != HAL_DMA_STATE_BUSY)
  {
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;
    return HAL_ERROR;
  }
  else
  {
    /* Set Abort State  */
    hdma->State = HAL_DMA_STATE_ABORT;

    /* Disable the stream */
    __HAL_DMA_DISABLE(hdma);
  }

  return HAL_OK;
}
//}}}

//{{{
HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *hdma, HAL_DMA_LevelCompleteTypeDef CompleteLevel, uint32_t Timeout)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t mask_cpltlevel;
  uint32_t tickstart = HAL_GetTick();
  uint32_t tmpisr;

  /* calculate DMA base and stream number */
  DMA_Base_Registers *regs;

  if(HAL_DMA_STATE_BUSY != hdma->State)
  {
    /* No transfer ongoing */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;
    __HAL_UNLOCK(hdma);
    return HAL_ERROR;
  }

  /* Polling mode not supported in circular mode and double buffering mode */
  if ((hdma->Instance->CR & DMA_SxCR_CIRC) != RESET)
  {
    hdma->ErrorCode = HAL_DMA_ERROR_NOT_SUPPORTED;
    return HAL_ERROR;
  }

  /* Get the level transfer complete flag */
  if(CompleteLevel == HAL_DMA_FULL_TRANSFER)
  {
    /* Transfer Complete flag */
    mask_cpltlevel = DMA_FLAG_TCIF0_4 << hdma->StreamIndex;
  }
  else
  {
    /* Half Transfer Complete flag */
    mask_cpltlevel = DMA_FLAG_HTIF0_4 << hdma->StreamIndex;
  }

  regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;
  tmpisr = regs->ISR;

  while(((tmpisr & mask_cpltlevel) == RESET) && ((hdma->ErrorCode & HAL_DMA_ERROR_TE) == RESET))
  {
    /* Check for the Timeout (Not applicable in circular mode)*/
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
      {
        /* Update error code */
        hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;

        /* Process Unlocked */
        __HAL_UNLOCK(hdma);

        /* Change the DMA state */
        hdma->State = HAL_DMA_STATE_READY;

        return HAL_TIMEOUT;
      }
    }

    /* Get the ISR register value */
    tmpisr = regs->ISR;

    if((tmpisr & (DMA_FLAG_TEIF0_4 << hdma->StreamIndex)) != RESET)
    {
      /* Update error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_TE;

      /* Clear the transfer error flag */
      regs->IFCR = DMA_FLAG_TEIF0_4 << hdma->StreamIndex;
    }

    if((tmpisr & (DMA_FLAG_FEIF0_4 << hdma->StreamIndex)) != RESET)
    {
      /* Update error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_FE;

      /* Clear the FIFO error flag */
      regs->IFCR = DMA_FLAG_FEIF0_4 << hdma->StreamIndex;
    }

    if((tmpisr & (DMA_FLAG_DMEIF0_4 << hdma->StreamIndex)) != RESET)
    {
      /* Update error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_DME;

      /* Clear the Direct Mode error flag */
      regs->IFCR = DMA_FLAG_DMEIF0_4 << hdma->StreamIndex;
    }
  }

  if(hdma->ErrorCode != HAL_DMA_ERROR_NONE)
  {
    if((hdma->ErrorCode & HAL_DMA_ERROR_TE) != RESET)
    {
      HAL_DMA_Abort(hdma);

      /* Clear the half transfer and transfer complete flags */
      regs->IFCR = (DMA_FLAG_HTIF0_4 | DMA_FLAG_TCIF0_4) << hdma->StreamIndex;

      /* Process Unlocked */
      __HAL_UNLOCK(hdma);

      /* Change the DMA state */
      hdma->State= HAL_DMA_STATE_READY;

      return HAL_ERROR;
   }
  }

  /* Get the level transfer complete flag */
  if(CompleteLevel == HAL_DMA_FULL_TRANSFER)
  {
    /* Clear the half transfer and transfer complete flags */
    regs->IFCR = (DMA_FLAG_HTIF0_4 | DMA_FLAG_TCIF0_4) << hdma->StreamIndex;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    hdma->State = HAL_DMA_STATE_READY;
  }
  else
  {
    /* Clear the half transfer flag */
    regs->IFCR = (DMA_FLAG_HTIF0_4) << hdma->StreamIndex;
  }

  return status;
}
//}}}

//{{{
void HAL_DMA_IRQHandler (DMA_HandleTypeDef *hdma) {

  __IO uint32_t count = 0;
  uint32_t timeout = SystemCoreClock / 9600;

  /* calculate DMA base and stream number */
  DMA_Base_Registers *regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;
  uint32_t tmpisr = regs->ISR;

  //{{{  Transfer Error Interrupt management ***************************************/
  if ((tmpisr & (DMA_FLAG_TEIF0_4 << hdma->StreamIndex)) != RESET) {
    if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_TE) != RESET)
    { /* Disable the transfer error interrupt */
      hdma->Instance->CR  &= ~(DMA_IT_TE);

      /* Clear the transfer error flag */
      regs->IFCR = DMA_FLAG_TEIF0_4 << hdma->StreamIndex;

      /* Update error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_TE;
    }
  }
  //}}}
  //{{{  FIFO Error Interrupt management ******************************************/
  if ((tmpisr & (DMA_FLAG_FEIF0_4 << hdma->StreamIndex)) != RESET) {
    if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_FE) != RESET) {
      /* Clear the FIFO error flag */
      regs->IFCR = DMA_FLAG_FEIF0_4 << hdma->StreamIndex;

      /* Update error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_FE;
    }
  }
  //}}}
  //{{{  Direct Mode Error Interrupt management ***********************************/
  if ((tmpisr & (DMA_FLAG_DMEIF0_4 << hdma->StreamIndex)) != RESET) {
    if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_DME) != RESET) {
      /* Clear the direct mode error flag */
      regs->IFCR = DMA_FLAG_DMEIF0_4 << hdma->StreamIndex;

      /* Update error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_DME;
    }
  }
  //}}}
  //{{{  Half Transfer Complete Interrupt management ******************************/
  if ((tmpisr & (DMA_FLAG_HTIF0_4 << hdma->StreamIndex)) != RESET)
  { if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_HT) != RESET) {
      /* Clear the half transfer complete flag */
      regs->IFCR = DMA_FLAG_HTIF0_4 << hdma->StreamIndex;

      /* Multi_Buffering mode enabled */
      if(((hdma->Instance->CR) & (uint32_t)(DMA_SxCR_DBM)) != RESET) {
        /* Current memory buffer used is Memory 0 */
        if((hdma->Instance->CR & DMA_SxCR_CT) == RESET) {
          if(hdma->XferHalfCpltCallback != NULL) {
            /* Half transfer callback */
            hdma->XferHalfCpltCallback(hdma);
          }
        }
        /* Current memory buffer used is Memory 1 */
        else {
          if(hdma->XferM1HalfCpltCallback != NULL) {
            /* Half transfer callback */
            hdma->XferM1HalfCpltCallback(hdma);
          }
        }
      }
      else {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if((hdma->Instance->CR & DMA_SxCR_CIRC) == RESET) {
          /* Disable the half transfer interrupt */
          hdma->Instance->CR  &= ~(DMA_IT_HT);
        }

        if(hdma->XferHalfCpltCallback != NULL) {
          /* Half transfer callback */
          hdma->XferHalfCpltCallback(hdma);
        }
      }
    }
  }
  //}}}
  //{{{  Transfer Complete Interrupt management ***********************************/
  if ((tmpisr & (DMA_FLAG_TCIF0_4 << hdma->StreamIndex)) != RESET) {
    if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_TC) != RESET)
    { /* Clear the transfer complete flag */
      regs->IFCR = DMA_FLAG_TCIF0_4 << hdma->StreamIndex;

      if(HAL_DMA_STATE_ABORT == hdma->State) {
        /* Disable all the transfer interrupts */
        hdma->Instance->CR  &= ~(DMA_IT_TC | DMA_IT_TE | DMA_IT_DME);
        hdma->Instance->FCR &= ~(DMA_IT_FE);

        if((hdma->XferHalfCpltCallback != NULL) || (hdma->XferM1HalfCpltCallback != NULL)) {
          hdma->Instance->CR  &= ~(DMA_IT_HT);
        }

        /* Clear all interrupt flags at correct offset within the register */
        regs->IFCR = 0x3FU << hdma->StreamIndex;

        /* Process Unlocked */
        __HAL_UNLOCK(hdma);

        /* Change the DMA state */
        hdma->State = HAL_DMA_STATE_READY;

        if(hdma->XferAbortCallback != NULL)
        {
          hdma->XferAbortCallback(hdma);
        }
        return;
      }

      if(((hdma->Instance->CR) & (uint32_t)(DMA_SxCR_DBM)) != RESET) {
        /* Current memory buffer used is Memory 0 */
        if((hdma->Instance->CR & DMA_SxCR_CT) == RESET) {
          if(hdma->XferM1CpltCallback != NULL)
          {
            /* Transfer complete Callback for memory1 */
            hdma->XferM1CpltCallback(hdma);
          }
        }
        /* Current memory buffer used is Memory 1 */
        else
        {
          if(hdma->XferCpltCallback != NULL) {
            /* Transfer complete Callback for memory0 */
            hdma->XferCpltCallback(hdma);
          }
        }
      }
      /* Disable the transfer complete interrupt if the DMA mode is not CIRCULAR */
      else
      {
        if((hdma->Instance->CR & DMA_SxCR_CIRC) == RESET) {
          /* Disable the transfer complete interrupt */
          hdma->Instance->CR  &= ~(DMA_IT_TC);

          /* Process Unlocked */
          __HAL_UNLOCK(hdma);

          /* Change the DMA state */
          hdma->State = HAL_DMA_STATE_READY;
        }

        if(hdma->XferCpltCallback != NULL) {
          /* Transfer complete callback */
          hdma->XferCpltCallback(hdma);
        }
      }
    }
  }
  //}}}
  //{{{  manage error case */
  if(hdma->ErrorCode != HAL_DMA_ERROR_NONE) {
    if((hdma->ErrorCode & HAL_DMA_ERROR_TE) != RESET) {
      hdma->State = HAL_DMA_STATE_ABORT;

      /* Disable the stream */
      __HAL_DMA_DISABLE(hdma);

      do
      {
        if (++count > timeout)
        {
          break;
        }
      }
      while((hdma->Instance->CR & DMA_SxCR_EN) != RESET);

      /* Process Unlocked */
      __HAL_UNLOCK(hdma);

      /* Change the DMA state */
      hdma->State = HAL_DMA_STATE_READY;
    }

    if(hdma->XferErrorCallback != NULL) {
      /* Transfer error callback */
      hdma->XferErrorCallback(hdma);
    }
  }
  //}}}
  }
//}}}

//{{{
HAL_StatusTypeDef HAL_DMA_RegisterCallback (DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID, void (* pCallback)(DMA_HandleTypeDef *_hdma))
{

  HAL_StatusTypeDef status = HAL_OK;

  /* Process locked */
  __HAL_LOCK(hdma);

  if(HAL_DMA_STATE_READY == hdma->State)
  {
    switch (CallbackID)
    {
    case  HAL_DMA_XFER_CPLT_CB_ID:
      hdma->XferCpltCallback = pCallback;
      break;

    case  HAL_DMA_XFER_HALFCPLT_CB_ID:
      hdma->XferHalfCpltCallback = pCallback;
      break;

    case  HAL_DMA_XFER_M1CPLT_CB_ID:
      hdma->XferM1CpltCallback = pCallback;
      break;

    case  HAL_DMA_XFER_M1HALFCPLT_CB_ID:
      hdma->XferM1HalfCpltCallback = pCallback;
      break;

    case  HAL_DMA_XFER_ERROR_CB_ID:
      hdma->XferErrorCallback = pCallback;
      break;

    case  HAL_DMA_XFER_ABORT_CB_ID:
      hdma->XferAbortCallback = pCallback;
      break;

    default:
      break;
    }
  }
  else
  {
    /* Return error status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hdma);

  return status;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA_UnRegisterCallback (DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Process locked */
  __HAL_LOCK(hdma);

  if(HAL_DMA_STATE_READY == hdma->State)
  {
    switch (CallbackID)
    {
    case  HAL_DMA_XFER_CPLT_CB_ID:
      hdma->XferCpltCallback = NULL;
      break;

    case  HAL_DMA_XFER_HALFCPLT_CB_ID:
      hdma->XferHalfCpltCallback = NULL;
      break;

    case  HAL_DMA_XFER_M1CPLT_CB_ID:
      hdma->XferM1CpltCallback = NULL;
      break;

    case  HAL_DMA_XFER_M1HALFCPLT_CB_ID:
      hdma->XferM1HalfCpltCallback = NULL;
      break;

    case  HAL_DMA_XFER_ERROR_CB_ID:
      hdma->XferErrorCallback = NULL;
      break;

    case  HAL_DMA_XFER_ABORT_CB_ID:
      hdma->XferAbortCallback = NULL;
      break;

    case   HAL_DMA_XFER_ALL_CB_ID:
      hdma->XferCpltCallback = NULL;
      hdma->XferHalfCpltCallback = NULL;
      hdma->XferM1CpltCallback = NULL;
      hdma->XferM1HalfCpltCallback = NULL;
      hdma->XferErrorCallback = NULL;
      hdma->XferAbortCallback = NULL;
      break;

    default:
      status = HAL_ERROR;
      break;
    }
  }
  else
  {
    status = HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hdma);

  return status;
}
//}}}

HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma) { return hdma->State; }
uint32_t HAL_DMA_GetError(DMA_HandleTypeDef *hdma) { return hdma->ErrorCode; }
