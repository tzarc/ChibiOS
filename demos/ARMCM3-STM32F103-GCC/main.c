/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010 Giovanni Di Sirio.

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

#include "ch.h"
#include "hal.h"
#include "test.h"

/*===========================================================================*/
/* USB related stuff.                                                        */
/*===========================================================================*/

/* USB Standard Device Descriptor.*/
static const uint8_t vcom_device_descriptor_data[] = {
  18,                               /* bLength.                             */
  USB_DESCRIPTOR_DEVICE,            /* bDescriptorType.                     */
  0x00, 0x02,                       /* bcdUSB (2.00).                       */
  0x02,                             /* bDeviceClass (CDC).                  */
  0x00,                             /* bDeviceSubClass.                     */
  0x00,                             /* bDeviceProtocol.                     */
  0x40,                             /* bMaxPacketSize.                      */
  0x83, 0x04,                       /* idVendor (0x0483).                   */
  0x40, 0x57,                       /* idProduct (0x7540).                  */
  0x00, 0x02,                       /* bcdDevice (2.00).                    */
  1,                                /* iManufacturer.                       */
  2,                                /* iProduct.                            */
  3,                                /* IiSerialNumber.                      */
  1                                 /* bNumConfigurations.                  */
};

/* Configuration Descriptor tree for a VCOM.*/
const uint8_t vcom_configuration_descriptor_data[] = {
  /* Configuration descriptor.*/
  9,                                /* bLength.                             */
  USB_DESCRIPTOR_CONFIGURATION,     /* bDescriptorType.                     */
  67,  0,                           /* wTotalLength.                        */
  0x02,                             /* bNumInterfaces.                      */
  0x01,                             /* bConfigurationValue.                 */
  0,                                /* iConfiguration.                      */
  0xC0,                             /* bmAttributes (self powered).         */
  50,                               /* MaxPower (100mA).                    */
  /* Interface Descriptor.*/
  9,                                /* bLength.                             */
  USB_DESCRIPTOR_INTERFACE,         /* bDescriptorType.                     */
  0x00,                             /* bInterfaceNumber.                    */
  0x00,                             /* bAlternateSetting.                   */
  0x01,                             /* bNumEndpoints.                       */
  0x02,                             /* bInterfaceClass (Communications
                                       Interface Class, CDC section 4.2).   */
  0x02,                             /* bInterfaceSubClass (Abstract Control
                                       Model, CDC section 4.3).             */
  0x01,                             /* bInterfaceProtocol (AT commands, CDC
                                       section 4.4).                        */
  0,                                /* iInterface.                          */
  /* Header Functional Descriptor (CDC section 5.2.3).*/
  5,                                /* bLength.                             */
  0x24,                             /* bDescriptorType (CS_INTERFACE).      */
  0x00,                             /* bDescriptorSubtype (Header Functional
                                       Descriptor.                          */
  0x10, 0x01,                       /* bcdCDC (1.10).                       */
  /* Call Managment Functional Descriptor. */
  5,                                /* bFunctionLength.                     */
  0x24,                             /* bDescriptorType (CS_INTERFACE).      */
  0x01,                             /* bDescriptorSubtype (Call Management
                                       Functional Descriptor).              */
  0x00,                             /* bmCapabilities (D0+D1).              */
  0x01,                             /* bDataInterface.                      */
  /* ACM Functional Descriptor.*/
  4,                                /* bFunctionLength.                     */
  0x24,                             /* bDescriptorType (CS_INTERFACE).      */
  0x02,                             /* bDescriptorSubtype (Abstract Control
                                       Management Descriptor).              */
  0x02,                             /* bmCapabilities.                      */
  /* Union Functional Descriptor.*/
  5,                                /* bFunctionLength.                     */
  0x24,                             /* bDescriptorType (CS_INTERFACE).      */
  0x06,                             /* bDescriptorSubtype (Union Functional
                                       Descriptor).                         */
  0x00,                             /* bMasterInterface (Communication Class
                                       Interface).                          */
  0x01,                             /* bSlaveInterface0 (Data Class
                                       Interface).                          */
  /* Endpoint 2 Descriptor.*/
  7,                                /* bLength.                             */
  USB_DESCRIPTOR_ENDPOINT,          /* bDescriptorType.                     */
  0x82,                             /* bEndpointAddress (IN2).              */
  0x03,                             /* bmAttributes (Interrupt).            */
  0x08, 0x00,                       /* wMaxPacketSize.                      */
  0xFF,                             /* bInterval.                           */
  /* Interface Descriptor.*/
  9,                                /* bLength.                             */
  USB_DESCRIPTOR_INTERFACE,         /* bDescriptorType.                     */
  0x01,                             /* bInterfaceNumber.                    */
  0x00,                             /* bAlternateSetting.                   */
  0x02,                             /* bNumEndpoints.                       */
  0x0A,                             /* bInterfaceClass (Communication Device
                                       Class, CDC section 4.5).             */
  0x00,                             /* bInterfaceSubClass (CDC section 4.6).*/
  0x00,                             /* bInterfaceProtocol (CDC section 4.7).*/
  0x00,                             /* iInterface.                          */
  /* Endpoint 3 Descriptor.*/
  7,                                /* bLength.                             */
  USB_DESCRIPTOR_ENDPOINT,          /* bDescriptorType.                     */
  0x03,                             /* bEndpointAddress (OUT3).             */
  0x02,                             /* bmAttributes (Bulk).                 */
  0x40, 0x00,                       /* wMaxPacketSize.                      */
  0x00,                             /* bInterval (ignored for bulk.         */
  /* Endpoint 1 Descriptor.*/
  7,                                /* bLength.                             */
  USB_DESCRIPTOR_ENDPOINT,          /* bDescriptorType.                     */
  0x81,                             /* bEndpointAddress (IN1).              */
  0x02,                             /* bmAttributes (Bulk).                 */
  0x40, 0x00,                       /* wMaxPacketSize.                      */
  0x00                              /* bInterval (ignored for bulk.         */
};

/*
 * Configuration descriptor wrapper.
 */
static const USBDescriptor vcom_configuration_descriptor = {
  sizeof (vcom_configuration_descriptor_data),
  vcom_configuration_descriptor_data
};


/*
 * U.S. English language identifier.
 */
const uint8_t vcom_string0[] = {
  4,                                /* bLength.                             */
  USB_DESCRIPTOR_STRING,            /* bDescriptorType.                     */
  0x09, 0x04                        /* wLANGID (0x0409, U.S. English).      */
};

/*
 * Vendor string.
 */
const uint8_t vcom_string1[] = {
  38,                               /* bLength.                             */
  USB_DESCRIPTOR_STRING,            /* bDescriptorType.                     */
  'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
  'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
  'c', 0, 's', 0
};

/*
 * Device description string.
 */
const uint8_t vcom_string2[] = {
  56,                               /* bLength.                             */
  USB_DESCRIPTOR_STRING,            /* bDescriptorType.                     */
  'C', 0, 'h', 0, 'i', 0, 'b', 0, 'i', 0, 'O', 0, 'S', 0, '/', 0,
  'R', 0, 'T', 0, ' ', 0, 'V', 0, 'i', 0, 'r', 0, 't', 0, 'u', 0,
  'a', 0, 'l', 0, ' ', 0, 'C', 0, 'O', 0, 'M', 0, ' ', 0, 'P', 0,
  'o', 0, 'r', 0, 't', 0
};

/*
 * Serial number string.
 */
const uint8_t vcom_string3[] = {
  8,                                /* bLength.                             */
  USB_DESCRIPTOR_STRING,            /* bDescriptorType.                     */
  '0' + CH_KERNEL_MAJOR, 0,
  '0' + CH_KERNEL_MINOR, 0,
  '0' + CH_KERNEL_PATCH, 0
};

/*
 * Strings wrappers array.
 */
static const USBDescriptor vcom_strings[] = {
  {sizeof(vcom_string0), vcom_string0},
  {sizeof(vcom_string1), vcom_string1},
  {sizeof(vcom_string2), vcom_string2},
  {sizeof(vcom_string3), vcom_string3}
};

void ep1in(USBDriver *usbp, uint32_t ep) {
}

void ep2in(USBDriver *usbp, uint32_t ep) {
}

void ep3out(USBDriver *usbp, uint32_t ep) {
}

/**
 * @brief   EP1 initialization structure (IN only).
 */
const USBEndpointConfig ep1config = {
  ep1in,
  NULL,
  1,
  0x0040,
  EPR_EP_TYPE_BULK | EPR_STAT_TX_NAK | EPR_STAT_RX_DIS,
  0x00C0,
  0x0000
};

/**
 * @brief   EP2 initialization structure (IN only).
 */
const USBEndpointConfig ep2config = {
  ep2in,
  NULL,
  2,
  0x0010,
  EPR_EP_TYPE_INTERRUPT | EPR_STAT_TX_NAK | EPR_STAT_RX_DIS,
  0x0100,
  0x0000
};

/**
 * @brief   EP3 initialization structure (OUT only).
 */
const USBEndpointConfig ep3config = {
  NULL,
  ep3out,
  3,
  0x0040,
  EPR_EP_TYPE_BULK | EPR_STAT_TX_DIS | EPR_STAT_RX_NAK,
  0x0000,
  0x0110
};

/*
 * Handles the USB driver global events.
 */
static void usb_event(USBDriver *usbp, usbevent_t event) {

  switch (event) {
  case USB_EVENT_RESET:
    asm("nop");
    return;
  case USB_EVENT_ADDRESS:
    usbEPOpenI(usbp, &ep1config);
    usbEPOpenI(usbp, &ep2config);
    usbEPOpenI(usbp, &ep3config);
    return;
  case USB_EVENT_SUSPEND:
    asm("nop");
    return;
  case USB_EVENT_RESUME:
    asm("nop");
    return;
  case USB_EVENT_STALLED:
    asm("nop");
    return;
  }
  return;
}

/*
 * Handles the GET_DESCRIPTOR callback. Except for the device descriptor
 * any other descriptor must be handled here.
 */
static const USBDescriptor *get_descriptor(USBDriver *usbp,
                                           uint8_t dtype,
                                           uint8_t dindex,
                                           uint16_t lang) {

  switch (dtype) {
  case USB_DESCRIPTOR_CONFIGURATION:
    return &vcom_configuration_descriptor;
  case USB_DESCRIPTOR_STRING:
    if (dindex < 4)
      return &vcom_strings[dindex];
  }
  return NULL;
}

/*
 * USB driver configuration.
 */
static const USBConfig usbcfg = {
  usb_event,
  {sizeof(vcom_device_descriptor_data), vcom_device_descriptor_data},
  get_descriptor,
};

/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  while (TRUE) {
    palClearPad(IOPORT3, GPIOC_LED);
    chThdSleepMilliseconds(500);
    palSetPad(IOPORT3, GPIOC_LED);
    chThdSleepMilliseconds(500);
  }
  return 0;
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the USB bus and then the USB driver.
   */
  palClearPad(GPIOC, GPIOC_USB_DISC);
  usbStart(&USBD1, &usbcfg);

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (TRUE) {
    if (palReadPad(IOPORT1, GPIOA_BUTTON))
      TestThread(&SD2);
    chThdSleepMilliseconds(500);
  }
}
