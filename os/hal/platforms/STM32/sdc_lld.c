/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    STM32/sdc_lld.c
 * @brief   STM32 SDC subsystem low level driver source.
 *
 * @addtogroup SDC
 * @{
 */

/*
 TODO: Test program to determine erase block size.
 TODO: Write normal interrupt handlers.
 TODO: Evaluate CRC error handling - some retry counts.
 TODO: Evaluate special F4x-F2x DMA features (works at first look).
 TODO: Try preerase blocks before writing (ACMD23).
 TODO: Evaluate needs of DMA interrupts handler.
 TODO: Errors handling.
 */

#include <string.h>

#include "ch.h"
#include "hal.h"

#if HAL_USE_SDC || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define SDC_SDIO_DMA_USE_FIFO   FALSE

#define SDC_SDIO_DMA_CHANNEL                                                  \
  STM32_DMA_GETCHANNEL(STM32_SDC_SDIO_DMA_STREAM,                             \
                       STM32_SDC_SDIO_DMA_CHN)

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief SDCD1 driver identifier.*/
SDCDriver SDCD1;

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief   Reads one or more blocks.
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 * @param[in] startblk  first block to read
 * @param[out] buf      pointer to the read buffer, it must be aligned to
 *                      four bytes boundary
 * @param[in] n         number of blocks to read
 *
 * @return              The operation status.
 * @retval FALSE        operation succeeded, the requested blocks have been
 *                      read.
 * @retval TRUE         operation failed, the state of the buffer is uncertain.
 *
 * @notapi
 */
static bool_t sdc_lld_read_align(SDCDriver *sdcp, uint32_t startblk,
                                    uint8_t *buf, uint32_t n) {
  uint32_t resp[1];

  /* Checks for errors and waits for the card to be ready for reading.*/
  if (_sdc_wait_for_transfer_state(sdcp))
    return SDC_FAILED;

  /* Prepares the DMA channel for reading.*/
  dmaStreamSetMemory0(sdcp->dma, buf);
  dmaStreamSetTransactionSize(sdcp->dma,
                              (n * SDC_BLOCK_SIZE) / sizeof (uint32_t));
  dmaStreamSetMode(sdcp->dma, sdcp->dmamode | STM32_DMA_CR_DIR_P2M);

  /* Setting up data transfer.
     Options: Card to Controller, Block mode, DMA mode, 512 bytes blocks.*/
  SDIO->ICR   = STM32_SDIO_ICR_ALL_FLAGS;
  SDIO->MASK  = SDIO_MASK_DCRCFAILIE |
                SDIO_MASK_DTIMEOUTIE |
                SDIO_MASK_DATAENDIE  |
                SDIO_MASK_STBITERRIE |
                SDIO_MASK_RXOVERRIE;
  SDIO->DLEN  = n * SDC_BLOCK_SIZE;

  if (n > 1){
    /* Send read multiple blocks command to card.*/
    if ((sdcp->cardmode & SDC_MODE_HIGH_CAPACITY) == 0)
      startblk *= SDC_BLOCK_SIZE;
    if (sdc_lld_send_cmd_short_crc(sdcp, SDC_CMD_READ_MULTIPLE_BLOCK,
                                   startblk, resp) ||
        SDC_R1_ERROR(resp[0]))
      goto error;
  }
  else{
    /* Read single block command.*/
    if ((sdcp->cardmode & SDC_MODE_HIGH_CAPACITY) == 0)
      startblk *= SDC_BLOCK_SIZE;
    if (sdc_lld_send_cmd_short_crc(sdcp, SDC_CMD_READ_SINGLE_BLOCK,
                                   startblk, resp) ||
        SDC_R1_ERROR(resp[0]))
      goto error;
  }

  /* DMA channel activation.*/
  dmaStreamEnable(sdcp->dma);

  /* Transaction starts just after DTEN bit setting.*/
  SDIO->DCTRL = SDIO_DCTRL_DTDIR |
                SDIO_DCTRL_DBLOCKSIZE_3 | SDIO_DCTRL_DBLOCKSIZE_0 |
                SDIO_DCTRL_DMAEN |
                SDIO_DCTRL_DTEN;

  chSysLock();
  if (SDIO->MASK != 0) {
    chDbgAssert(sdcp->thread == NULL,
                "sdc_lld_read_multiple(), #1", "not NULL");
    sdcp->thread = chThdSelf();
    chSchGoSleepS(THD_STATE_SUSPENDED);
    chDbgAssert(sdcp->thread == NULL,
                "sdc_lld_read_multiple(), #2", "not NULL");
  }
  if ((SDIO->STA & SDIO_STA_DATAEND) == 0) {
    chSysUnlock();
    goto error;
  }
  dmaStreamClearInterrupt(sdcp->dma);
  dmaStreamDisable(sdcp->dma);
  SDIO->ICR   = STM32_SDIO_ICR_ALL_FLAGS;
  SDIO->DCTRL = 0;
  chSysUnlock();

  return sdc_lld_send_cmd_short_crc(sdcp, SDC_CMD_STOP_TRANSMISSION, 0, resp);

error:
  dmaStreamClearInterrupt(sdcp->dma);
  dmaStreamDisable(sdcp->dma);
  SDIO->ICR   = STM32_SDIO_ICR_ALL_FLAGS;
  SDIO->MASK  = 0;
  SDIO->DCTRL = 0;
  return SDC_FAILED;
}

/**
 * @brief   Writes one or more blocks.
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 * @param[in] startblk  first block to write
 * @param[out] buf      pointer to the write buffer, it must be aligned to
 *                      four bytes boundary
 * @param[in] n         number of blocks to write
 *
 * @return              The operation status.
 * @retval FALSE        operation succeeded, the requested blocks have been
 *                      written.
 * @retval TRUE         operation failed.
 *
 * @notapi
 */
static bool_t sdc_lld_write_align(SDCDriver *sdcp, uint32_t startblk,
                              const uint8_t *buf, uint32_t n) {
  uint32_t resp[1];

  /* Checks for errors and waits for the card to be ready for writing.*/
  if (_sdc_wait_for_transfer_state(sdcp))
    return SDC_FAILED;

  /* Prepares the DMA channel for writing.*/
  dmaStreamSetMemory0(sdcp->dma, buf);
  dmaStreamSetTransactionSize(sdcp->dma,
                              (n * SDC_BLOCK_SIZE) / sizeof (uint32_t));
  dmaStreamSetMode(sdcp->dma, sdcp->dmamode | STM32_DMA_CR_DIR_M2P);

  /* Setting up data transfer.*/
  SDIO->ICR   = STM32_SDIO_ICR_ALL_FLAGS;
  SDIO->MASK  = SDIO_MASK_DCRCFAILIE |
                SDIO_MASK_DTIMEOUTIE |
                SDIO_MASK_DATAENDIE  |
                SDIO_MASK_STBITERRIE |
                SDIO_MASK_TXUNDERRIE;
  SDIO->DLEN  = n * SDC_BLOCK_SIZE;

  if (n > 1){
    /* Write multiple blocks command.*/
    if ((sdcp->cardmode & SDC_MODE_HIGH_CAPACITY) == 0)
      startblk *= SDC_BLOCK_SIZE;
    if (sdc_lld_send_cmd_short_crc(sdcp, SDC_CMD_WRITE_MULTIPLE_BLOCK,
                                   startblk, resp) ||
        SDC_R1_ERROR(resp[0]))
      return SDC_FAILED;
  }
  else{
    /* Write single block command.*/
    if ((sdcp->cardmode & SDC_MODE_HIGH_CAPACITY) == 0)
      startblk *= SDC_BLOCK_SIZE;
    if (sdc_lld_send_cmd_short_crc(sdcp, SDC_CMD_WRITE_BLOCK,
                                   startblk, resp) ||
        SDC_R1_ERROR(resp[0]))
      return SDC_FAILED;
  }

  /* Transaction starts just after DTEN bit setting.*/
  chSysLock();
  SDIO->DCTRL = SDIO_DCTRL_DBLOCKSIZE_3 | SDIO_DCTRL_DBLOCKSIZE_0 |
                SDIO_DCTRL_DMAEN |
                SDIO_DCTRL_DTEN;
  dmaStreamEnable(sdcp->dma);
  chSysUnlock();

  /* Note the mask is checked before going to sleep because the interrupt
     may have occurred before reaching the critical zone.*/
  chSysLock();
  if (SDIO->MASK != 0) {
    chDbgAssert(sdcp->thread == NULL,
                "sdc_lld_write_multiple(), #1", "not NULL");
    sdcp->thread = chThdSelf();
    chSchGoSleepS(THD_STATE_SUSPENDED);
    chDbgAssert(sdcp->thread == NULL,
                "sdc_lld_write_multiple(), #2", "not NULL");
  }
  if ((SDIO->STA & SDIO_STA_DATAEND) == 0) {
    chSysUnlock();
    goto error;
  }
  dmaStreamClearInterrupt(sdcp->dma);
  dmaStreamDisable(sdcp->dma);
  SDIO->ICR   = STM32_SDIO_ICR_ALL_FLAGS;
  SDIO->DCTRL = 0;
  chSysUnlock();

  return sdc_lld_send_cmd_short_crc(sdcp, SDC_CMD_STOP_TRANSMISSION, 0, resp);

error:
  dmaStreamClearInterrupt(sdcp->dma);
  dmaStreamDisable(sdcp->dma);
  SDIO->ICR   = STM32_SDIO_ICR_ALL_FLAGS;
  SDIO->MASK  = 0;
  SDIO->DCTRL = 0;
  return SDC_FAILED;
}

/**
 * @brief   SDIO normal IRQ handler.
 *
 * @notapi
 */
static void sdc_serve_event_interrupt(SDCDriver *sdcp) {

  chSysLockFromIsr();
  if (sdcp->thread != NULL) {
    chSchReadyI(sdcp->thread);
    sdcp->thread = NULL;
  }
  chSysUnlockFromIsr();

  /* Disables the source but the status flags are not reset because the
     read/write functions need to check them.*/
  SDIO->MASK = 0;
}

/**
 * @brief   SDIO error IRQ handler.
 *
 * @notapi
 */
static void sdc_serve_error_interrupt(SDCDriver *sdcp) {

  (void)sdcp;
  chSysHalt();
}

/**
 * @brief   SDC DMA common service routine.
 *
 * @param[in] sdcp      pointer to the @p UARTDriver object
 * @param[in] flags     pre-shifted content of the ISR register
 */
static void sdc_lld_serve_dma_interrupt(SDCDriver *sdcp, uint32_t flags) {

  (void) sdcp;

  /* DMA errors handling.*/
#if defined(STM32_SDC_DMA_ERROR_HOOK)
  if ((flags & (STM32_DMA_ISR_TEIF | STM32_DMA_ISR_DMEIF)) != 0) {
    STM32_SDC_DMA_ERROR_HOOK();
  }
#else
  (void)flags;
#endif
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/**
 * @brief   SDIO IRQ handler.
 *
 * @isr
 */
CH_IRQ_HANDLER(SDIO_IRQHandler) {

  CH_IRQ_PROLOGUE();

  if (SDIO->STA & STM32_SDIO_STA_ERROR_MASK)
    sdc_serve_error_interrupt(&SDCD1);
  else
    sdc_serve_event_interrupt(&SDCD1);

  CH_IRQ_EPILOGUE();
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level SDC driver initialization.
 *
 * @notapi
 */
void sdc_lld_init(void) {

  sdcObjectInit(&SDCD1);
  SDCD1.thread = NULL;
#if CH_DBG_ENABLE_ASSERTS
  SDCD1.sdio   = SDIO;
#endif
  SDCD1.dma    = STM32_DMA_STREAM(STM32_SDC_SDIO_DMA_STREAM);
}

/**
 * @brief   Configures and activates the SDC peripheral.
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object, must be @p NULL,
 *                      this driver does not require any configuration
 *
 * @notapi
 */
void sdc_lld_start(SDCDriver *sdcp) {

  sdcp->dmamode = STM32_DMA_CR_CHSEL(SDC_SDIO_DMA_CHANNEL) |
                  STM32_DMA_CR_PL(STM32_SDC_SDIO_DMA_PRIORITY) |
                  STM32_DMA_CR_PSIZE_WORD | STM32_DMA_CR_MSIZE_WORD |
                  STM32_DMA_CR_MINC;
  #if (SDC_SDIO_DMA_USE_FIFO && (defined(STM32F4XX) || defined(STM32F2XX)))
    sdcp->dmamode |= STM32_DMA_CR_PFCTRL |                                    \
                     STM32_DMA_CR_PBURST_INCR4 | STM32_DMA_CR_MBURST_INCR4;
  #else
    sdcp->dmamode |= STM32_DMA_CR_PFCTRL;
  #endif

  if (sdcp->state == SDC_STOP) {
    /* Note, the DMA must be enabled before the IRQs.*/
    bool_t b;
    b = dmaStreamAllocate(sdcp->dma, STM32_SDC_SDIO_IRQ_PRIORITY,
                         (stm32_dmaisr_t)sdc_lld_serve_dma_interrupt,
                         (void *)sdcp);
    chDbgAssert(!b, "i2c_lld_start(), #3", "stream already allocated");
    dmaStreamSetPeripheral(sdcp->dma, &SDIO->FIFO);
    #if (SDC_SDIO_DMA_USE_FIFO && (defined(STM32F4XX) || defined(STM32F2XX)))
      dmaStreamSetFIFO(sdcp->dma, STM32_DMA_FCR_DMDIS | STM32_DMA_FCR_FTH_FULL);
    #endif
    nvicEnableVector(SDIO_IRQn,
                     CORTEX_PRIORITY_MASK(STM32_SDC_SDIO_IRQ_PRIORITY));
    rccEnableSDIO(FALSE);
  }

  /* Configuration, card clock is initially stopped.*/
  SDIO->POWER  = 0;
  SDIO->CLKCR  = 0;
  SDIO->DCTRL  = 0;
  SDIO->DTIMER = 0;
}

/**
 * @brief   Deactivates the SDC peripheral.
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 *
 * @notapi
 */
void sdc_lld_stop(SDCDriver *sdcp) {

  if ((sdcp->state == SDC_READY) || (sdcp->state == SDC_ACTIVE)) {
    SDIO->POWER  = 0;
    SDIO->CLKCR  = 0;
    SDIO->DCTRL  = 0;
    SDIO->DTIMER = 0;

    /* Clock deactivation.*/
    nvicDisableVector(SDIO_IRQn);
    dmaStreamRelease(sdcp->dma);
    rccDisableSDIO(FALSE);
  }
}

/**
 * @brief   Starts the SDIO clock and sets it to init mode (400KHz or less).
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 *
 * @notapi
 */
void sdc_lld_start_clk(SDCDriver *sdcp) {

  (void)sdcp;
  /* Initial clock setting: 400KHz, 1bit mode.*/
  SDIO->CLKCR  = STM32_SDIO_DIV_LS;
  SDIO->POWER |= SDIO_POWER_PWRCTRL_0 | SDIO_POWER_PWRCTRL_1;
  SDIO->CLKCR |= SDIO_CLKCR_CLKEN;
}

/**
 * @brief   Sets the SDIO clock to data mode (25MHz or less).
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 *
 * @notapi
 */
void sdc_lld_set_data_clk(SDCDriver *sdcp) {

  (void)sdcp;
  SDIO->CLKCR = (SDIO->CLKCR & 0xFFFFFF00) | STM32_SDIO_DIV_HS;
}

/**
 * @brief   Stops the SDIO clock.
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 *
 * @notapi
 */
void sdc_lld_stop_clk(SDCDriver *sdcp) {

  (void)sdcp;
  SDIO->CLKCR = 0;
  SDIO->POWER = 0;
}

/**
 * @brief   Switches the bus to 4 bits mode.
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 * @param[in] mode      bus mode
 *
 * @notapi
 */
void sdc_lld_set_bus_mode(SDCDriver *sdcp, sdcbusmode_t mode) {
  uint32_t clk = SDIO->CLKCR & ~SDIO_CLKCR_WIDBUS;

  (void)sdcp;
  switch (mode) {
  case SDC_MODE_1BIT:
    SDIO->CLKCR = clk;
    break;
  case SDC_MODE_4BIT:
    SDIO->CLKCR = clk | SDIO_CLKCR_WIDBUS_0;
    break;
  case SDC_MODE_8BIT:
    SDIO->CLKCR = clk | SDIO_CLKCR_WIDBUS_1;
    break;
  }
}

/**
 * @brief   Sends an SDIO command with no response expected.
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 * @param[in] cmd       card command
 * @param[in] arg       command argument
 *
 * @notapi
 */
void sdc_lld_send_cmd_none(SDCDriver *sdcp, uint8_t cmd, uint32_t arg) {

  (void)sdcp;
  SDIO->ARG = arg;
  SDIO->CMD = (uint32_t)cmd | SDIO_CMD_CPSMEN;
  while ((SDIO->STA & SDIO_STA_CMDSENT) == 0)
    ;
  SDIO->ICR = SDIO_ICR_CMDSENTC;
}

/**
 * @brief   Sends an SDIO command with a short response expected.
 * @note    The CRC is not verified.
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 * @param[in] cmd       card command
 * @param[in] arg       command argument
 * @param[out] resp     pointer to the response buffer (one word)
 *
 * @return              The operation status.
 * @retval FALSE        the operation succeeded.
 * @retval TRUE         the operation failed because timeout, CRC check or
 *                      other errors.
 *
 * @notapi
 */
bool_t sdc_lld_send_cmd_short(SDCDriver *sdcp, uint8_t cmd, uint32_t arg,
                              uint32_t *resp) {
  uint32_t sta;

  (void)sdcp;
  SDIO->ARG = arg;
  SDIO->CMD = (uint32_t)cmd | SDIO_CMD_WAITRESP_0 | SDIO_CMD_CPSMEN;
  while (((sta = SDIO->STA) & (SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT |
                               SDIO_STA_CCRCFAIL)) == 0)
    ;
  SDIO->ICR = SDIO_ICR_CMDRENDC | SDIO_ICR_CTIMEOUTC | SDIO_ICR_CCRCFAILC;
  if ((sta & (SDIO_STA_CTIMEOUT)) != 0)
    return SDC_FAILED;
  *resp = SDIO->RESP1;
  return SDC_SUCCESS;
}

/**
 * @brief   Sends an SDIO command with a short response expected and CRC.
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 * @param[in] cmd       card command
 * @param[in] arg       command argument
 * @param[out] resp     pointer to the response buffer (one word)
 *
 * @return              The operation status.
 * @retval FALSE        the operation succeeded.
 * @retval TRUE         the operation failed because timeout, CRC check or
 *                      other errors.
 *
 * @notapi
 */
bool_t sdc_lld_send_cmd_short_crc(SDCDriver *sdcp, uint8_t cmd, uint32_t arg,
                                  uint32_t *resp) {
  uint32_t sta;

  (void)sdcp;
  SDIO->ARG = arg;
  SDIO->CMD = (uint32_t)cmd | SDIO_CMD_WAITRESP_0 | SDIO_CMD_CPSMEN;
  while (((sta = SDIO->STA) & (SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT |
                               SDIO_STA_CCRCFAIL)) == 0)
    ;
  SDIO->ICR = SDIO_ICR_CMDRENDC | SDIO_ICR_CTIMEOUTC | SDIO_ICR_CCRCFAILC;
  if ((sta & (SDIO_STA_CTIMEOUT | SDIO_STA_CCRCFAIL)) != 0)
    return SDC_FAILED;
  *resp = SDIO->RESP1;
  return SDC_SUCCESS;
}

/**
 * @brief   Sends an SDIO command with a long response expected and CRC.
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 * @param[in] cmd       card command
 * @param[in] arg       command argument
 * @param[out] resp     pointer to the response buffer (four words)
 *
 * @return              The operation status.
 * @retval FALSE        the operation succeeded.
 * @retval TRUE         the operation failed because timeout, CRC check or
 *                      other errors.
 *
 * @notapi
 */
bool_t sdc_lld_send_cmd_long_crc(SDCDriver *sdcp, uint8_t cmd, uint32_t arg,
                                 uint32_t *resp) {

  uint32_t sta;

  (void)sdcp;
  SDIO->ARG = arg;
  SDIO->CMD = (uint32_t)cmd | SDIO_CMD_WAITRESP_0 | SDIO_CMD_WAITRESP_1 |
                              SDIO_CMD_CPSMEN;
  while (((sta = SDIO->STA) & (SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT |
                               SDIO_STA_CCRCFAIL)) == 0)
    ;
  SDIO->ICR = SDIO_ICR_CMDRENDC | SDIO_ICR_CTIMEOUTC | SDIO_ICR_CCRCFAILC;
  if ((sta & (SDIO_STA_CTIMEOUT | SDIO_STA_CCRCFAIL)) != 0)
    return SDC_FAILED;
  /* save bytes in reverse order because MSB in response comes first */
  *resp++ = SDIO->RESP4;
  *resp++ = SDIO->RESP3;
  *resp++ = SDIO->RESP2;
  *resp   = SDIO->RESP1;
  return SDC_SUCCESS;
}

/**
 * @brief   Reads one or more blocks.
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 * @param[in] startblk  first block to read
 * @param[out] buf      pointer to the read buffer
 * @param[in] n         number of blocks to read
 *
 * @return              The operation status.
 * @retval FALSE        operation succeeded, the requested blocks have been
 *                      read.
 * @retval TRUE         operation failed, the state of the buffer is uncertain.
 *
 * @notapi
 */
bool_t sdc_lld_read(SDCDriver *sdcp, uint32_t startblk,
                    uint8_t *buf, uint32_t n) {

  chDbgCheck((n < (0x1000000 / SDC_BLOCK_SIZE)), "max transaction size");

  SDIO->DTIMER = STM32_SDC_READ_TIMEOUT;

  return sdc_lld_read_align(sdcp, startblk, buf, n);
}

/**
 * @brief   Writes one or more blocks.
 *
 * @param[in] sdcp      pointer to the @p SDCDriver object
 * @param[in] startblk  first block to write
 * @param[out] buf      pointer to the write buffer
 * @param[in] n         number of blocks to write
 *
 * @return              The operation status.
 * @retval FALSE        operation succeeded, the requested blocks have been
 *                      written.
 * @retval TRUE         operation failed.
 *
 * @notapi
 */
bool_t sdc_lld_write(SDCDriver *sdcp, uint32_t startblk,
                     const uint8_t *buf, uint32_t n) {

  chDbgCheck((n < (0x1000000 / SDC_BLOCK_SIZE)), "max transaction size");

  SDIO->DTIMER = STM32_SDC_WRITE_TIMEOUT;

  return sdc_lld_write_align(sdcp, startblk, buf, n);
}

#endif /* HAL_USE_SDC */

/** @} */
