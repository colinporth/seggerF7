#include "stm32f7xx_hal.h"

//{{{
static void DMA_MultiBufferSetConfig (DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
  /* Configure DMA Stream data length */
  hdma->Instance->NDTR = DataLength;

  if ((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH) {
    /* Peripheral to Memory */
    hdma->Instance->PAR = DstAddress;
    hdma->Instance->M0AR = SrcAddress;
    }
  else {
    /* Memory to Peripheral */
    hdma->Instance->PAR = SrcAddress;
    hdma->Instance->M0AR = DstAddress;
    }
  }
//}}}

//{{{
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart (DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));

  /* Memory-to-memory transfer not supported in double buffering mode */
  if (hdma->Init.Direction == DMA_MEMORY_TO_MEMORY)
  {
    hdma->ErrorCode = HAL_DMA_ERROR_NOT_SUPPORTED;
    status = HAL_ERROR;
  }
  else
  {
    /* Process Locked */
    __HAL_LOCK(hdma);

    if(HAL_DMA_STATE_READY == hdma->State)
    {
      /* Change DMA peripheral state */
      hdma->State = HAL_DMA_STATE_BUSY;

      /* Enable the double buffer mode */
      hdma->Instance->CR |= (uint32_t)DMA_SxCR_DBM;

      /* Configure DMA Stream destination address */
      hdma->Instance->M1AR = SecondMemAddress;

      /* Configure the source, destination address and the data length */
      DMA_MultiBufferSetConfig(hdma, SrcAddress, DstAddress, DataLength);

      /* Enable the peripheral */
      __HAL_DMA_ENABLE(hdma);
    }
    else
    {
      /* Return error status */
      status = HAL_BUSY;
    }
  }
  return status;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart_IT (DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Memory-to-memory transfer not supported in double buffering mode */
  if (hdma->Init.Direction == DMA_MEMORY_TO_MEMORY)
  {
    hdma->ErrorCode = HAL_DMA_ERROR_NOT_SUPPORTED;
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hdma);

  if(HAL_DMA_STATE_READY == hdma->State)
  {
    /* Change DMA peripheral state */
    hdma->State = HAL_DMA_STATE_BUSY;

    /* Initialize the error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;

    /* Enable the Double buffer mode */
    hdma->Instance->CR |= (uint32_t)DMA_SxCR_DBM;

    /* Configure DMA Stream destination address */
    hdma->Instance->M1AR = SecondMemAddress;

    /* Configure the source, destination address and the data length */
    DMA_MultiBufferSetConfig(hdma, SrcAddress, DstAddress, DataLength);

    /* Clear all flags */
    __HAL_DMA_CLEAR_FLAG (hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
    __HAL_DMA_CLEAR_FLAG (hdma, __HAL_DMA_GET_HT_FLAG_INDEX(hdma));
    __HAL_DMA_CLEAR_FLAG (hdma, __HAL_DMA_GET_TE_FLAG_INDEX(hdma));
    __HAL_DMA_CLEAR_FLAG (hdma, __HAL_DMA_GET_DME_FLAG_INDEX(hdma));
    __HAL_DMA_CLEAR_FLAG (hdma, __HAL_DMA_GET_FE_FLAG_INDEX(hdma));

    /* Enable Common interrupts*/
    hdma->Instance->CR  |= DMA_IT_TC | DMA_IT_TE | DMA_IT_DME;
    hdma->Instance->FCR |= DMA_IT_FE;

    if((hdma->XferHalfCpltCallback != NULL) || (hdma->XferM1HalfCpltCallback != NULL))
      hdma->Instance->CR  |= DMA_IT_HT;

    /* Enable the peripheral */
    __HAL_DMA_ENABLE(hdma);
  }
  else
  {
    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    /* Return error status */
    status = HAL_BUSY;
  }
  return status;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMAEx_ChangeMemory (DMA_HandleTypeDef *hdma, uint32_t Address, HAL_DMA_MemoryTypeDef memory)
{
  if (memory == MEMORY0) /* change the memory0 address */
    hdma->Instance->M0AR = Address;
  else
    /* change the memory1 address */
    hdma->Instance->M1AR = Address;

  return HAL_OK;
  }
//}}}
