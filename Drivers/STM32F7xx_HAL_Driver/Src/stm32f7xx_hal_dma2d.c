#include "stm32f7xx_hal.h"

#define DMA2D_TIMEOUT_ABORT     ((uint32_t)1000)  /*!<  1s  */
#define DMA2D_TIMEOUT_SUSPEND   ((uint32_t)1000)  /*!<  1s  */

static void DMA2D_SetConfig(DMA2D_HandleTypeDef *hdma2d, uint32_t pdata, uint32_t DstAddress, uint32_t Width, uint32_t Height);
//{{{
HAL_StatusTypeDef HAL_DMA2D_Init (DMA2D_HandleTypeDef *hdma2d)
{
  /* Check the DMA2D peripheral state */
  if(hdma2d == NULL)
  {
     return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA2D_ALL_INSTANCE(hdma2d->Instance));
  assert_param(IS_DMA2D_MODE(hdma2d->Init.Mode));
  assert_param(IS_DMA2D_CMODE(hdma2d->Init.ColorMode));
  assert_param(IS_DMA2D_OFFSET(hdma2d->Init.OutputOffset));

  if(hdma2d->State == HAL_DMA2D_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hdma2d->Lock = HAL_UNLOCKED;
    /* Init the low level hardware */
    HAL_DMA2D_MspInit(hdma2d);
  }

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* DMA2D CR register configuration -------------------------------------------*/
  MODIFY_REG(hdma2d->Instance->CR, DMA2D_CR_MODE, hdma2d->Init.Mode);

  /* DMA2D OPFCCR register configuration ---------------------------------------*/
  MODIFY_REG(hdma2d->Instance->OPFCCR, DMA2D_OPFCCR_CM, hdma2d->Init.ColorMode);

  /* DMA2D OOR register configuration ------------------------------------------*/
  MODIFY_REG(hdma2d->Instance->OOR, DMA2D_OOR_LO, hdma2d->Init.OutputOffset);

#if defined (DMA2D_OPFCCR_AI)
  /* DMA2D OPFCCR AI fields setting (Output Alpha Inversion)*/
  MODIFY_REG(hdma2d->Instance->OPFCCR, DMA2D_OPFCCR_AI, (hdma2d->Init.AlphaInverted << DMA2D_OPFCCR_AI_Pos));
#endif /* DMA2D_OPFCCR_AI */

#if defined (DMA2D_OPFCCR_RBS)
  MODIFY_REG(hdma2d->Instance->OPFCCR, DMA2D_OPFCCR_RBS,(hdma2d->Init.RedBlueSwap << DMA2D_OPFCCR_RBS_Pos));
#endif /* DMA2D_OPFCCR_RBS */


  /* Update error code */
  hdma2d->ErrorCode = HAL_DMA2D_ERROR_NONE;

  /* Initialize the DMA2D state*/
  hdma2d->State  = HAL_DMA2D_STATE_READY;

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_DeInit (DMA2D_HandleTypeDef *hdma2d)
{

  /* Check the DMA2D peripheral state */
  if(hdma2d == NULL)
  {
     return HAL_ERROR;
  }

  /* Before aborting any DMA2D transfer or CLUT loading, check
     first whether or not DMA2D clock is enabled */
  if (__HAL_RCC_DMA2D_IS_CLK_ENABLED())
  {
    /* Abort DMA2D transfer if any */
    if ((hdma2d->Instance->CR & DMA2D_CR_START) == DMA2D_CR_START)
    {
      if (HAL_DMA2D_Abort(hdma2d) != HAL_OK)
      {
        /* Issue when aborting DMA2D transfer */
        return HAL_ERROR;
      }
    }
    else
    {
      /* Abort background CLUT loading if any */
      if ((hdma2d->Instance->BGPFCCR & DMA2D_BGPFCCR_START) == DMA2D_BGPFCCR_START)
      {
        if (HAL_DMA2D_CLUTLoading_Abort(hdma2d, 0) != HAL_OK)
        {
          /* Issue when aborting background CLUT loading */
          return HAL_ERROR;
        }
      }
      else
      {
        /* Abort foreground CLUT loading if any */
        if ((hdma2d->Instance->FGPFCCR & DMA2D_FGPFCCR_START) == DMA2D_FGPFCCR_START)
        {
          if (HAL_DMA2D_CLUTLoading_Abort(hdma2d, 1) != HAL_OK)
          {
            /* Issue when aborting foreground CLUT loading */
            return HAL_ERROR;
          }
        }
      }
    }
  }


  /* Carry on with de-initialization of low level hardware */
  HAL_DMA2D_MspDeInit(hdma2d);

  /* Reset DMA2D control registers*/
  hdma2d->Instance->CR = 0;
  hdma2d->Instance->FGOR = 0;
  hdma2d->Instance->BGOR = 0;
  hdma2d->Instance->FGPFCCR = 0;
  hdma2d->Instance->BGPFCCR = 0;
  hdma2d->Instance->OPFCCR = 0;

  /* Update error code */
  hdma2d->ErrorCode = HAL_DMA2D_ERROR_NONE;

  /* Initialize the DMA2D state*/
  hdma2d->State  = HAL_DMA2D_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hdma2d);

  return HAL_OK;
}
//}}}

//{{{
__weak void HAL_DMA2D_MspInit (DMA2D_HandleTypeDef* hdma2d)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma2d);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_DMA2D_MspInit can be implemented in the user file.
   */
}
//}}}
//{{{
__weak void HAL_DMA2D_MspDeInit(DMA2D_HandleTypeDef* hdma2d)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma2d);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_DMA2D_MspDeInit can be implemented in the user file.
   */
}
//}}}

//{{{
HAL_StatusTypeDef HAL_DMA2D_Start (DMA2D_HandleTypeDef *hdma2d, uint32_t pdata, uint32_t DstAddress, uint32_t Width,  uint32_t Height)
{
  /* Check the parameters */
  assert_param(IS_DMA2D_LINE(Height));
  assert_param(IS_DMA2D_PIXEL(Width));

  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Configure the source, destination address and the data size */
  DMA2D_SetConfig(hdma2d, pdata, DstAddress, Width, Height);

  /* Enable the Peripheral */
  __HAL_DMA2D_ENABLE(hdma2d);

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_Start_IT (DMA2D_HandleTypeDef *hdma2d, uint32_t pdata, uint32_t DstAddress, uint32_t Width,  uint32_t Height)
{
  /* Check the parameters */
  assert_param(IS_DMA2D_LINE(Height));
  assert_param(IS_DMA2D_PIXEL(Width));

  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Configure the source, destination address and the data size */
  DMA2D_SetConfig(hdma2d, pdata, DstAddress, Width, Height);

  /* Enable the transfer complete, transfer error and configuration error interrupts */
  __HAL_DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_TC|DMA2D_IT_TE|DMA2D_IT_CE);

  /* Enable the Peripheral */
  __HAL_DMA2D_ENABLE(hdma2d);

  return HAL_OK;
}
//}}}

//{{{
HAL_StatusTypeDef HAL_DMA2D_BlendingStart (DMA2D_HandleTypeDef *hdma2d, uint32_t SrcAddress1, uint32_t  SrcAddress2, uint32_t DstAddress, uint32_t Width,  uint32_t Height)
{
  /* Check the parameters */
  assert_param(IS_DMA2D_LINE(Height));
  assert_param(IS_DMA2D_PIXEL(Width));

  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Configure DMA2D Stream source2 address */
  WRITE_REG(hdma2d->Instance->BGMAR, SrcAddress2);

  /* Configure the source, destination address and the data size */
  DMA2D_SetConfig(hdma2d, SrcAddress1, DstAddress, Width, Height);

  /* Enable the Peripheral */
  __HAL_DMA2D_ENABLE(hdma2d);

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_BlendingStart_IT (DMA2D_HandleTypeDef *hdma2d, uint32_t SrcAddress1, uint32_t  SrcAddress2, uint32_t DstAddress, uint32_t Width,  uint32_t Height)
{
  /* Check the parameters */
  assert_param(IS_DMA2D_LINE(Height));
  assert_param(IS_DMA2D_PIXEL(Width));

  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Configure DMA2D Stream source2 address */
  WRITE_REG(hdma2d->Instance->BGMAR, SrcAddress2);

  /* Configure the source, destination address and the data size */
  DMA2D_SetConfig(hdma2d, SrcAddress1, DstAddress, Width, Height);

  /* Enable the transfer complete, transfer error and configuration error interrupts */
  __HAL_DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_TC|DMA2D_IT_TE|DMA2D_IT_CE);

  /* Enable the Peripheral */
  __HAL_DMA2D_ENABLE(hdma2d);

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_Abort (DMA2D_HandleTypeDef *hdma2d)
{
  uint32_t tickstart = 0;

  /* Abort the DMA2D transfer */
  /* START bit is reset to make sure not to set it again, in the event the HW clears it
     between the register read and the register write by the CPU (writing 0 has no
     effect on START bitvalue) */
   MODIFY_REG(hdma2d->Instance->CR, DMA2D_CR_ABORT|DMA2D_CR_START, DMA2D_CR_ABORT);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Check if the DMA2D is effectively disabled */
  while((hdma2d->Instance->CR & DMA2D_CR_START) != RESET)
  {
    if((HAL_GetTick() - tickstart ) > DMA2D_TIMEOUT_ABORT)
    {
      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TIMEOUT;

      /* Change the DMA2D state */
      hdma2d->State = HAL_DMA2D_STATE_TIMEOUT;

      /* Process Unlocked */
      __HAL_UNLOCK(hdma2d);

      return HAL_TIMEOUT;
    }
  }

  /* Disable the Transfer Complete, Transfer Error and Configuration Error interrupts */
  __HAL_DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_TC|DMA2D_IT_TE|DMA2D_IT_CE);

  /* Change the DMA2D state*/
  hdma2d->State = HAL_DMA2D_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hdma2d);

  return HAL_OK;
}
//}}}

//{{{
HAL_StatusTypeDef HAL_DMA2D_Suspend (DMA2D_HandleTypeDef *hdma2d)
{
  uint32_t tickstart = 0;

  /* Suspend the DMA2D transfer */
  /* START bit is reset to make sure not to set it again, in the event the HW clears it
     between the register read and the register write by the CPU (writing 0 has no
     effect on START bitvalue). */
  MODIFY_REG(hdma2d->Instance->CR, DMA2D_CR_SUSP|DMA2D_CR_START, DMA2D_CR_SUSP);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Check if the DMA2D is effectively suspended */
  while (((hdma2d->Instance->CR & DMA2D_CR_SUSP) != DMA2D_CR_SUSP) \
    && ((hdma2d->Instance->CR & DMA2D_CR_START) == DMA2D_CR_START))
  {
    if((HAL_GetTick() - tickstart ) > DMA2D_TIMEOUT_SUSPEND)
    {
      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TIMEOUT;

      /* Change the DMA2D state */
      hdma2d->State = HAL_DMA2D_STATE_TIMEOUT;

      return HAL_TIMEOUT;
    }
  }

   /* Check whether or not a transfer is actually suspended and change the DMA2D state accordingly */
  if ((hdma2d->Instance->CR & DMA2D_CR_START) != RESET)
  {
    hdma2d->State = HAL_DMA2D_STATE_SUSPEND;
  }
  else
  {
    /* Make sure SUSP bit is cleared since it is meaningless
       when no tranfer is on-going */
    CLEAR_BIT(hdma2d->Instance->CR, DMA2D_CR_SUSP);
  }

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_Resume (DMA2D_HandleTypeDef *hdma2d)
{
  /* Check the SUSP and START bits */
  if((hdma2d->Instance->CR & (DMA2D_CR_SUSP | DMA2D_CR_START)) == (DMA2D_CR_SUSP | DMA2D_CR_START))
  {
    /* Ongoing transfer is suspended: change the DMA2D state before resuming */
    hdma2d->State = HAL_DMA2D_STATE_BUSY;
  }

  /* Resume the DMA2D transfer */
  /* START bit is reset to make sure not to set it again, in the event the HW clears it
     between the register read and the register write by the CPU (writing 0 has no
     effect on START bitvalue). */
  CLEAR_BIT(hdma2d->Instance->CR, (DMA2D_CR_SUSP|DMA2D_CR_START));

  return HAL_OK;
}
//}}}

//{{{
HAL_StatusTypeDef HAL_DMA2D_EnableCLUT (DMA2D_HandleTypeDef *hdma2d, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_DMA2D_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  if(LayerIdx == 0)
  {
    /* Enable the background CLUT loading */
    SET_BIT(hdma2d->Instance->BGPFCCR, DMA2D_BGPFCCR_START);
  }
  else
  {
    /* Enable the foreground CLUT loading */
    SET_BIT(hdma2d->Instance->FGPFCCR, DMA2D_FGPFCCR_START);
  }

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_CLUTLoad (DMA2D_HandleTypeDef *hdma2d, DMA2D_CLUTCfgTypeDef CLUTCfg, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_DMA2D_LAYER(LayerIdx));
  assert_param(IS_DMA2D_CLUT_CM(CLUTCfg.CLUTColorMode));
  assert_param(IS_DMA2D_CLUT_SIZE(CLUTCfg.Size));

  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Configure the CLUT of the background DMA2D layer */
  if(LayerIdx == 0)
  {
    /* Write background CLUT memory address */
    WRITE_REG(hdma2d->Instance->BGCMAR, (uint32_t)CLUTCfg.pCLUT);

    /* Write background CLUT size and CLUT color mode */
    MODIFY_REG(hdma2d->Instance->BGPFCCR, (DMA2D_BGPFCCR_CS | DMA2D_BGPFCCR_CCM),
            ((CLUTCfg.Size << DMA2D_BGPFCCR_CS_Pos) | (CLUTCfg.CLUTColorMode << DMA2D_BGPFCCR_CCM_Pos)));

    /* Enable the CLUT loading for the background */
    SET_BIT(hdma2d->Instance->BGPFCCR, DMA2D_BGPFCCR_START);
  }
  /* Configure the CLUT of the foreground DMA2D layer */
  else
  {
    /* Write foreground CLUT memory address */
    WRITE_REG(hdma2d->Instance->FGCMAR, (uint32_t)CLUTCfg.pCLUT);

    /* Write foreground CLUT size and CLUT color mode */
    MODIFY_REG(hdma2d->Instance->FGPFCCR, (DMA2D_FGPFCCR_CS | DMA2D_FGPFCCR_CCM),
            ((CLUTCfg.Size << DMA2D_BGPFCCR_CS_Pos) | (CLUTCfg.CLUTColorMode << DMA2D_FGPFCCR_CCM_Pos)));

 /* Enable the CLUT loading for the foreground */
    SET_BIT(hdma2d->Instance->FGPFCCR, DMA2D_FGPFCCR_START);
  }

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_CLUTLoad_IT (DMA2D_HandleTypeDef *hdma2d, DMA2D_CLUTCfgTypeDef CLUTCfg, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_DMA2D_LAYER(LayerIdx));
  assert_param(IS_DMA2D_CLUT_CM(CLUTCfg.CLUTColorMode));
  assert_param(IS_DMA2D_CLUT_SIZE(CLUTCfg.Size));

  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Configure the CLUT of the background DMA2D layer */
  if(LayerIdx == 0)
  {
    /* Write background CLUT memory address */
    WRITE_REG(hdma2d->Instance->BGCMAR, (uint32_t)CLUTCfg.pCLUT);

    /* Write background CLUT size and CLUT color mode */
    MODIFY_REG(hdma2d->Instance->BGPFCCR, (DMA2D_BGPFCCR_CS | DMA2D_BGPFCCR_CCM),
            ((CLUTCfg.Size << DMA2D_BGPFCCR_CS_Pos) | (CLUTCfg.CLUTColorMode << DMA2D_BGPFCCR_CCM_Pos)));

    /* Enable the CLUT Transfer Complete, transfer Error, configuration Error and CLUT Access Error interrupts */
    __HAL_DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_CTC | DMA2D_IT_TE | DMA2D_IT_CE |DMA2D_IT_CAE);

    /* Enable the CLUT loading for the background */
    SET_BIT(hdma2d->Instance->BGPFCCR, DMA2D_BGPFCCR_START);
  }
  /* Configure the CLUT of the foreground DMA2D layer */
  else
  {
    /* Write foreground CLUT memory address */
    WRITE_REG(hdma2d->Instance->FGCMAR, (uint32_t)CLUTCfg.pCLUT);

    /* Write foreground CLUT size and CLUT color mode */
    MODIFY_REG(hdma2d->Instance->FGPFCCR, (DMA2D_FGPFCCR_CS | DMA2D_FGPFCCR_CCM),
            ((CLUTCfg.Size << DMA2D_BGPFCCR_CS_Pos) | (CLUTCfg.CLUTColorMode << DMA2D_FGPFCCR_CCM_Pos)));

    /* Enable the CLUT Transfer Complete, transfer Error, configuration Error and CLUT Access Error interrupts */
    __HAL_DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_CTC | DMA2D_IT_TE | DMA2D_IT_CE |DMA2D_IT_CAE);

    /* Enable the CLUT loading for the foreground */
    SET_BIT(hdma2d->Instance->FGPFCCR, DMA2D_FGPFCCR_START);
  }

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_CLUTLoading_Abort (DMA2D_HandleTypeDef *hdma2d, uint32_t LayerIdx)
{
  uint32_t tickstart  = 0;
  __IO uint32_t * reg =  &(hdma2d->Instance->BGPFCCR); /* by default, point at background register */

  /* Abort the CLUT loading */
  SET_BIT(hdma2d->Instance->CR, DMA2D_CR_ABORT);

  /* If foreground CLUT loading is considered, update local variables */
  if(LayerIdx == 1)
  {
    reg  = &(hdma2d->Instance->FGPFCCR);
  }


  /* Get tick */
  tickstart = HAL_GetTick();

  /* Check if the CLUT loading is aborted */
  while((*reg & DMA2D_BGPFCCR_START) != RESET)
  {
    if((HAL_GetTick() - tickstart ) > DMA2D_TIMEOUT_ABORT)
    {
      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TIMEOUT;

      /* Change the DMA2D state */
      hdma2d->State = HAL_DMA2D_STATE_TIMEOUT;

      /* Process Unlocked */
      __HAL_UNLOCK(hdma2d);

      return HAL_TIMEOUT;
    }
  }

  /* Disable the CLUT Transfer Complete, Transfer Error, Configuration Error and CLUT Access Error interrupts */
  __HAL_DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_CTC | DMA2D_IT_TE | DMA2D_IT_CE |DMA2D_IT_CAE);

  /* Change the DMA2D state*/
  hdma2d->State = HAL_DMA2D_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hdma2d);

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_CLUTLoading_Suspend (DMA2D_HandleTypeDef *hdma2d, uint32_t LayerIdx)
{
  uint32_t tickstart = 0;
  __IO uint32_t * reg =  &(hdma2d->Instance->BGPFCCR); /* by default, point at background register */

  /* Suspend the CLUT loading */
  SET_BIT(hdma2d->Instance->CR, DMA2D_CR_SUSP);

  /* If foreground CLUT loading is considered, update local variables */
  if(LayerIdx == 1)
  {
    reg  = &(hdma2d->Instance->FGPFCCR);
  }

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Check if the CLUT loading is suspended */
  while (((hdma2d->Instance->CR & DMA2D_CR_SUSP) != DMA2D_CR_SUSP) \
    && ((*reg & DMA2D_BGPFCCR_START) == DMA2D_BGPFCCR_START))
  {
    if((HAL_GetTick() - tickstart ) > DMA2D_TIMEOUT_SUSPEND)
    {
      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TIMEOUT;

      /* Change the DMA2D state */
      hdma2d->State = HAL_DMA2D_STATE_TIMEOUT;

      return HAL_TIMEOUT;
    }
  }

   /* Check whether or not a transfer is actually suspended and change the DMA2D state accordingly */
  if ((*reg & DMA2D_BGPFCCR_START) != RESET)
  {
    hdma2d->State = HAL_DMA2D_STATE_SUSPEND;
  }
  else
  {
    /* Make sure SUSP bit is cleared since it is meaningless
       when no tranfer is on-going */
    CLEAR_BIT(hdma2d->Instance->CR, DMA2D_CR_SUSP);
  }

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_CLUTLoading_Resume (DMA2D_HandleTypeDef *hdma2d, uint32_t LayerIdx)
{
  /* Check the SUSP and START bits for background or foreground CLUT loading */
  if(LayerIdx == 0)
  {
    /* Background CLUT loading suspension check */
    if (((hdma2d->Instance->CR & DMA2D_CR_SUSP) == DMA2D_CR_SUSP)
      && ((hdma2d->Instance->BGPFCCR & DMA2D_BGPFCCR_START) == DMA2D_BGPFCCR_START))
    {
      /* Ongoing CLUT loading is suspended: change the DMA2D state before resuming */
      hdma2d->State = HAL_DMA2D_STATE_BUSY;
    }
  }
  else
  {
    /* Foreground CLUT loading suspension check */
    if (((hdma2d->Instance->CR & DMA2D_CR_SUSP) == DMA2D_CR_SUSP)
      && ((hdma2d->Instance->FGPFCCR & DMA2D_FGPFCCR_START) == DMA2D_FGPFCCR_START))
    {
      /* Ongoing CLUT loading is suspended: change the DMA2D state before resuming */
      hdma2d->State = HAL_DMA2D_STATE_BUSY;
    }
  }

  /* Resume the CLUT loading */
  CLEAR_BIT(hdma2d->Instance->CR, DMA2D_CR_SUSP);

  return HAL_OK;
}

//}}}

//{{{
HAL_StatusTypeDef HAL_DMA2D_PollForTransfer (DMA2D_HandleTypeDef *hdma2d, uint32_t Timeout) {

  uint32_t tickstart = 0;
  __IO uint32_t isrflags = 0x0;

  /* Polling for DMA2D transfer */
  if ((hdma2d->Instance->CR & DMA2D_CR_START) != RESET) {
    tickstart = HAL_GetTick();
    while (__HAL_DMA2D_GET_FLAG (hdma2d, DMA2D_FLAG_TC) == RESET) {
      isrflags = READ_REG (hdma2d->Instance->ISR);
      if ((isrflags & (DMA2D_FLAG_CE|DMA2D_FLAG_TE)) != RESET) {
        if ((isrflags & DMA2D_FLAG_CE) != RESET)
          hdma2d->ErrorCode |= HAL_DMA2D_ERROR_CE;
        if ((isrflags & DMA2D_FLAG_TE) != RESET)
          hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TE;
        /* Clear the transfer and configuration error flags */
        __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CE | DMA2D_FLAG_TE);
        /* Change DMA2D state */
        hdma2d->State = HAL_DMA2D_STATE_ERROR;
        /* Process unlocked */
        __HAL_UNLOCK(hdma2d);
        return HAL_ERROR;
        }

      /* Check for the Timeout */
      if (Timeout != HAL_MAX_DELAY) {
        if ((Timeout == 0) || ((HAL_GetTick() - tickstart ) > Timeout)) {
          /* Update error code */
          hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TIMEOUT;
          /* Change the DMA2D state */
          hdma2d->State = HAL_DMA2D_STATE_TIMEOUT;
          /* Process unlocked */
          __HAL_UNLOCK(hdma2d);
          return HAL_TIMEOUT;
          }
        }
      }
    }

  /* Polling for CLUT loading (foreground or background) */
  if (((hdma2d->Instance->FGPFCCR & DMA2D_FGPFCCR_START) != RESET)  ||
      ((hdma2d->Instance->BGPFCCR & DMA2D_BGPFCCR_START) != RESET)) {
    /* Get tick */
    tickstart = HAL_GetTick();

    while (__HAL_DMA2D_GET_FLAG(hdma2d, DMA2D_FLAG_CTC) == RESET) {
      isrflags = READ_REG(hdma2d->Instance->ISR);
      if ((isrflags & (DMA2D_FLAG_CAE|DMA2D_FLAG_CE|DMA2D_FLAG_TE)) != RESET) {
        if ((isrflags & DMA2D_FLAG_CAE) != RESET)
          hdma2d->ErrorCode |= HAL_DMA2D_ERROR_CAE;
        if ((isrflags & DMA2D_FLAG_CE) != RESET)
          hdma2d->ErrorCode |= HAL_DMA2D_ERROR_CE;
        if ((isrflags & DMA2D_FLAG_TE) != RESET)
          hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TE;
        /* Clear the CLUT Access Error, Configuration Error and Transfer Error flags */
        __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CAE | DMA2D_FLAG_CE | DMA2D_FLAG_TE);

        /* Change DMA2D state */
        hdma2d->State= HAL_DMA2D_STATE_ERROR;

        /* Process unlocked */
        __HAL_UNLOCK(hdma2d);

        return HAL_ERROR;
        }

      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY) {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout)) {
          /* Update error code */
          hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TIMEOUT;
          /* Change the DMA2D state */
          hdma2d->State= HAL_DMA2D_STATE_TIMEOUT;
          /* Process unlocked */
          __HAL_UNLOCK(hdma2d);
          return HAL_TIMEOUT;
          }
        }
      }
    }

  /* Clear the transfer complete and CLUT loading flags */
  __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_TC|DMA2D_FLAG_CTC);

  /* Change DMA2D state */
  hdma2d->State = HAL_DMA2D_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hdma2d);

  return HAL_OK;
  }
//}}}

//{{{
void HAL_DMA2D_IRQHandler (DMA2D_HandleTypeDef *hdma2d)
{
  uint32_t isrflags = READ_REG(hdma2d->Instance->ISR);
  uint32_t crflags = READ_REG(hdma2d->Instance->CR);

  /* Transfer Error Interrupt management ***************************************/
  if ((isrflags & DMA2D_FLAG_TE) != RESET)
  {
    if ((crflags & DMA2D_IT_TE) != RESET)
    {
      /* Disable the transfer Error interrupt */
      __HAL_DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_TE);

      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TE;

      /* Clear the transfer error flag */
      __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_TE);

      /* Change DMA2D state */
      hdma2d->State = HAL_DMA2D_STATE_ERROR;

      /* Process Unlocked */
      __HAL_UNLOCK(hdma2d);

      if(hdma2d->XferErrorCallback != NULL)
      {
        /* Transfer error Callback */
        hdma2d->XferErrorCallback(hdma2d);
      }
    }
  }
  /* Configuration Error Interrupt management **********************************/
  if ((isrflags & DMA2D_FLAG_CE) != RESET)
  {
    if ((crflags & DMA2D_IT_CE) != RESET)
    {
      /* Disable the Configuration Error interrupt */
      __HAL_DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_CE);

      /* Clear the Configuration error flag */
      __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CE);

      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_CE;

      /* Change DMA2D state */
      hdma2d->State = HAL_DMA2D_STATE_ERROR;

      /* Process Unlocked */
      __HAL_UNLOCK(hdma2d);

      if(hdma2d->XferErrorCallback != NULL)
      {
        /* Transfer error Callback */
        hdma2d->XferErrorCallback(hdma2d);
      }
    }
  }
  /* CLUT access Error Interrupt management ***********************************/
  if ((isrflags & DMA2D_FLAG_CAE) != RESET)
  {
    if ((crflags & DMA2D_IT_CAE) != RESET)
    {
      /* Disable the CLUT access error interrupt */
      __HAL_DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_CAE);

      /* Clear the CLUT access error flag */
      __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CAE);

      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_CAE;

      /* Change DMA2D state */
      hdma2d->State = HAL_DMA2D_STATE_ERROR;

      /* Process Unlocked */
      __HAL_UNLOCK(hdma2d);

      if(hdma2d->XferErrorCallback != NULL)
      {
        /* Transfer error Callback */
        hdma2d->XferErrorCallback(hdma2d);
      }
    }
  }
  /* Transfer watermark Interrupt management **********************************/
  if ((isrflags & DMA2D_FLAG_TW) != RESET)
  {
    if ((crflags & DMA2D_IT_TW) != RESET)
    {
      /* Disable the transfer watermark interrupt */
      __HAL_DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_TW);

      /* Clear the transfer watermark flag */
      __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_TW);

      /* Transfer watermark Callback */
      HAL_DMA2D_LineEventCallback(hdma2d);
    }
  }
  /* Transfer Complete Interrupt management ************************************/
  if ((isrflags & DMA2D_FLAG_TC) != RESET)
  {
    if ((crflags & DMA2D_IT_TC) != RESET)
    {
      /* Disable the transfer complete interrupt */
      __HAL_DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_TC);

      /* Clear the transfer complete flag */
      __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_TC);

      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_NONE;

      /* Change DMA2D state */
      hdma2d->State = HAL_DMA2D_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hdma2d);

      if(hdma2d->XferCpltCallback != NULL)
      {
        /* Transfer complete Callback */
        hdma2d->XferCpltCallback(hdma2d);
      }
    }
  }
  /* CLUT Transfer Complete Interrupt management ******************************/
  if ((isrflags & DMA2D_FLAG_CTC) != RESET)
  {
    if ((crflags & DMA2D_IT_CTC) != RESET)
    {
      /* Disable the CLUT transfer complete interrupt */
      __HAL_DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_CTC);

      /* Clear the CLUT transfer complete flag */
      __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CTC);

      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_NONE;

      /* Change DMA2D state */
      hdma2d->State = HAL_DMA2D_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hdma2d);

      /* CLUT Transfer complete Callback */
      HAL_DMA2D_CLUTLoadingCpltCallback(hdma2d);
    }
  }

}
//}}}

//{{{
__weak void HAL_DMA2D_LineEventCallback (DMA2D_HandleTypeDef *hdma2d)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma2d);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_DMA2D_LineEventCallback can be implemented in the user file.
   */
}
//}}}
//{{{
__weak void HAL_DMA2D_CLUTLoadingCpltCallback (DMA2D_HandleTypeDef *hdma2d)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma2d);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_DMA2D_CLUTLoadingCpltCallback can be implemented in the user file.
   */
}
//}}}

//{{{
HAL_StatusTypeDef HAL_DMA2D_ConfigLayer (DMA2D_HandleTypeDef *hdma2d, uint32_t LayerIdx)
{
  DMA2D_LayerCfgTypeDef *pLayerCfg = &hdma2d->LayerCfg[LayerIdx];

  uint32_t regMask = 0, regValue = 0;

  /* Check the parameters */
  assert_param(IS_DMA2D_LAYER(LayerIdx));
  assert_param(IS_DMA2D_OFFSET(pLayerCfg->InputOffset));
  if(hdma2d->Init.Mode != DMA2D_R2M)
  {
    assert_param(IS_DMA2D_INPUT_COLOR_MODE(pLayerCfg->InputColorMode));
    if(hdma2d->Init.Mode != DMA2D_M2M)
    {
      assert_param(IS_DMA2D_ALPHA_MODE(pLayerCfg->AlphaMode));
    }
  }

  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* DMA2D BGPFCR register configuration -----------------------------------*/
  /* Prepare the value to be written to the BGPFCCR register */

  regValue = pLayerCfg->InputColorMode | (pLayerCfg->AlphaMode << DMA2D_BGPFCCR_AM_Pos);
  regMask  = DMA2D_BGPFCCR_CM | DMA2D_BGPFCCR_AM | DMA2D_BGPFCCR_ALPHA;

#if defined (DMA2D_FGPFCCR_AI) && defined (DMA2D_BGPFCCR_AI)
  regValue |= (pLayerCfg->AlphaInverted << DMA2D_BGPFCCR_AI_Pos);
  regMask  |= DMA2D_BGPFCCR_AI;
#endif /* (DMA2D_FGPFCCR_AI) && (DMA2D_BGPFCCR_AI)  */

#if defined (DMA2D_FGPFCCR_RBS) && defined (DMA2D_BGPFCCR_RBS)
  regValue |= (pLayerCfg->RedBlueSwap << DMA2D_BGPFCCR_RBS_Pos);
  regMask  |= DMA2D_BGPFCCR_RBS;
#endif

  if ((pLayerCfg->InputColorMode == DMA2D_INPUT_A4) || (pLayerCfg->InputColorMode == DMA2D_INPUT_A8))
  {
    regValue |= (pLayerCfg->InputAlpha & DMA2D_BGPFCCR_ALPHA);
  }
  else
  {
    regValue |=  (pLayerCfg->InputAlpha << DMA2D_BGPFCCR_ALPHA_Pos);
  }

  /* Configure the background DMA2D layer */
  if(LayerIdx == 0)
  {
    /* Write DMA2D BGPFCCR register */
    MODIFY_REG(hdma2d->Instance->BGPFCCR, regMask, regValue);

    /* DMA2D BGOR register configuration -------------------------------------*/
    WRITE_REG(hdma2d->Instance->BGOR, pLayerCfg->InputOffset);

    /* DMA2D BGCOLR register configuration -------------------------------------*/
    if ((pLayerCfg->InputColorMode == DMA2D_INPUT_A4) || (pLayerCfg->InputColorMode == DMA2D_INPUT_A8))
    {
      WRITE_REG(hdma2d->Instance->BGCOLR, pLayerCfg->InputAlpha & (DMA2D_BGCOLR_BLUE|DMA2D_BGCOLR_GREEN|DMA2D_BGCOLR_RED));
    }
  }
  /* Configure the foreground DMA2D layer */
  else
  {
     /* Write DMA2D FGPFCCR register */
    MODIFY_REG(hdma2d->Instance->FGPFCCR, regMask, regValue);

    /* DMA2D FGOR register configuration -------------------------------------*/
    WRITE_REG(hdma2d->Instance->FGOR, pLayerCfg->InputOffset);

    /* DMA2D FGCOLR register configuration -------------------------------------*/
    if ((pLayerCfg->InputColorMode == DMA2D_INPUT_A4) || (pLayerCfg->InputColorMode == DMA2D_INPUT_A8))
    {
      WRITE_REG(hdma2d->Instance->FGCOLR, pLayerCfg->InputAlpha & (DMA2D_FGCOLR_BLUE|DMA2D_FGCOLR_GREEN|DMA2D_FGCOLR_RED));
    }
  }
  /* Initialize the DMA2D state*/
  hdma2d->State = HAL_DMA2D_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hdma2d);

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_ConfigCLUT (DMA2D_HandleTypeDef *hdma2d, DMA2D_CLUTCfgTypeDef CLUTCfg, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_DMA2D_LAYER(LayerIdx));
  assert_param(IS_DMA2D_CLUT_CM(CLUTCfg.CLUTColorMode));
  assert_param(IS_DMA2D_CLUT_SIZE(CLUTCfg.Size));

  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Configure the CLUT of the background DMA2D layer */
  if(LayerIdx == 0)
  {
    /* Write background CLUT memory address */
    WRITE_REG(hdma2d->Instance->BGCMAR, (uint32_t)CLUTCfg.pCLUT);

    /* Write background CLUT size and CLUT color mode */
    MODIFY_REG(hdma2d->Instance->BGPFCCR, (DMA2D_BGPFCCR_CS | DMA2D_BGPFCCR_CCM),
            ((CLUTCfg.Size << DMA2D_BGPFCCR_CS_Pos) | (CLUTCfg.CLUTColorMode << DMA2D_BGPFCCR_CCM_Pos)));
 }
 /* Configure the CLUT of the foreground DMA2D layer */
 else
 {
   /* Write foreground CLUT memory address */
    WRITE_REG(hdma2d->Instance->FGCMAR, (uint32_t)CLUTCfg.pCLUT);

    /* Write foreground CLUT size and CLUT color mode */
    MODIFY_REG(hdma2d->Instance->FGPFCCR, (DMA2D_FGPFCCR_CS | DMA2D_FGPFCCR_CCM),
            ((CLUTCfg.Size << DMA2D_BGPFCCR_CS_Pos) | (CLUTCfg.CLUTColorMode << DMA2D_FGPFCCR_CCM_Pos)));
  }

  /* Set the DMA2D state to Ready*/
  hdma2d->State = HAL_DMA2D_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hdma2d);

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_ProgramLineEvent (DMA2D_HandleTypeDef *hdma2d, uint32_t Line)
{
  /* Check the parameters */
  assert_param(IS_DMA2D_LINEWATERMARK(Line));

  if (Line > DMA2D_LWR_LW)
  {
    return HAL_ERROR;
  }
  else
  {
    /* Process locked */
    __HAL_LOCK(hdma2d);

    /* Change DMA2D peripheral state */
    hdma2d->State = HAL_DMA2D_STATE_BUSY;

    /* Sets the Line watermark configuration */
    WRITE_REG(hdma2d->Instance->LWR, Line);

    /* Enable the Line interrupt */
    __HAL_DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_TW);

    /* Initialize the DMA2D state*/
    hdma2d->State = HAL_DMA2D_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma2d);

    return HAL_OK;
  }
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_EnableDeadTime (DMA2D_HandleTypeDef *hdma2d)
{
  /* Process Locked */
  __HAL_LOCK(hdma2d);

  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Set DMA2D_AMTCR EN bit */
  SET_BIT(hdma2d->Instance->AMTCR, DMA2D_AMTCR_EN);

  hdma2d->State = HAL_DMA2D_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hdma2d);

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_DisableDeadTime (DMA2D_HandleTypeDef *hdma2d)
{
  /* Process Locked */
  __HAL_LOCK(hdma2d);

  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Clear DMA2D_AMTCR EN bit */
  CLEAR_BIT(hdma2d->Instance->AMTCR, DMA2D_AMTCR_EN);

  hdma2d->State = HAL_DMA2D_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hdma2d);

  return HAL_OK;
}
//}}}
//{{{
HAL_StatusTypeDef HAL_DMA2D_ConfigDeadTime (DMA2D_HandleTypeDef *hdma2d, uint8_t DeadTime)
{
  /* Process Locked */
  __HAL_LOCK(hdma2d);

  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Set DMA2D_AMTCR DT field */
  MODIFY_REG(hdma2d->Instance->AMTCR, DMA2D_AMTCR_DT, (((uint32_t) DeadTime) << DMA2D_AMTCR_DT_Pos));

  hdma2d->State = HAL_DMA2D_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hdma2d);

  return HAL_OK;
}
//}}}
//{{{
HAL_DMA2D_StateTypeDef HAL_DMA2D_GetState (DMA2D_HandleTypeDef *hdma2d)
{
  return hdma2d->State;
}
//}}}

//{{{
uint32_t HAL_DMA2D_GetError (DMA2D_HandleTypeDef *hdma2d)
{
  return hdma2d->ErrorCode;
}
//}}}
//{{{
static void DMA2D_SetConfig (DMA2D_HandleTypeDef *hdma2d, uint32_t pdata, uint32_t DstAddress, uint32_t Width, uint32_t Height)
{
  uint32_t tmp = 0;
  uint32_t tmp1 = 0;
  uint32_t tmp2 = 0;
  uint32_t tmp3 = 0;
  uint32_t tmp4 = 0;

  /* Configure DMA2D data size */
  MODIFY_REG(hdma2d->Instance->NLR, (DMA2D_NLR_NL|DMA2D_NLR_PL), (Height| (Width << DMA2D_NLR_PL_Pos)));

  /* Configure DMA2D destination address */
  WRITE_REG(hdma2d->Instance->OMAR, DstAddress);

  /* Register to memory DMA2D mode selected */
  if (hdma2d->Init.Mode == DMA2D_R2M)
  {
    tmp1 = pdata & DMA2D_OCOLR_ALPHA_1;
    tmp2 = pdata & DMA2D_OCOLR_RED_1;
    tmp3 = pdata & DMA2D_OCOLR_GREEN_1;
    tmp4 = pdata & DMA2D_OCOLR_BLUE_1;

    /* Prepare the value to be written to the OCOLR register according to the color mode */
    if (hdma2d->Init.ColorMode == DMA2D_OUTPUT_ARGB8888)
    {
      tmp = (tmp3 | tmp2 | tmp1| tmp4);
    }
    else if (hdma2d->Init.ColorMode == DMA2D_OUTPUT_RGB888)
    {
      tmp = (tmp3 | tmp2 | tmp4);
    }
    else if (hdma2d->Init.ColorMode == DMA2D_OUTPUT_RGB565)
    {
      tmp2 = (tmp2 >> 19);
      tmp3 = (tmp3 >> 10);
      tmp4 = (tmp4 >> 3 );
      tmp  = ((tmp3 << 5) | (tmp2 << 11) | tmp4);
    }
    else if (hdma2d->Init.ColorMode == DMA2D_OUTPUT_ARGB1555)
    {
      tmp1 = (tmp1 >> 31);
      tmp2 = (tmp2 >> 19);
      tmp3 = (tmp3 >> 11);
      tmp4 = (tmp4 >> 3 );
      tmp  = ((tmp3 << 5) | (tmp2 << 10) | (tmp1 << 15) | tmp4);
    }
    else /* Dhdma2d->Init.ColorMode = DMA2D_OUTPUT_ARGB4444 */
    {
      tmp1 = (tmp1 >> 28);
      tmp2 = (tmp2 >> 20);
      tmp3 = (tmp3 >> 12);
      tmp4 = (tmp4 >> 4 );
      tmp  = ((tmp3 << 4) | (tmp2 << 8) | (tmp1 << 12) | tmp4);
    }
    /* Write to DMA2D OCOLR register */
    WRITE_REG(hdma2d->Instance->OCOLR, tmp);
  }
  else /* M2M, M2M_PFC or M2M_Blending DMA2D Mode */
  {
    /* Configure DMA2D source address */
    WRITE_REG(hdma2d->Instance->FGMAR, pdata);
  }
}
//}}}
