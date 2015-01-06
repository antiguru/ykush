#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include "usbcom.h"
#include <iostream>
using namespace std;

#define VERSION "0.1.0"
#define VENDOR_ID 0x04D8
#define PRODUCT_ID 0x0042

// HID Class-Specific Requests values. See section 7.2 of the HID specifications
#define HID_GET_REPORT                0x01
#define HID_GET_IDLE                  0x02
#define HID_GET_PROTOCOL              0x03
#define HID_SET_REPORT                0x09
#define HID_SET_IDLE                  0x0A
#define HID_SET_PROTOCOL              0x0B
#define HID_REPORT_TYPE_INPUT         0x01
#define HID_REPORT_TYPE_OUTPUT        0x02
#define HID_REPORT_TYPE_FEATURE       0x03

#define CTRL_IN         LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE
#define CTRL_OUT        LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE


const static int PACKET_CTRL_LEN=2;
const static int PACKET_INT_LEN=6;
const static int INTERFACE=0;
const static int ENDPOINT_INT_IN=0x81; /* endpoint 0x81 address for IN */
const static int ENDPOINT_INT_OUT=0x01; /* endpoint 1 address for OUT */
const static int TIMEOUT=5000; /* timeout in ms */
const static int TIMEOUT_EXEC_CMD=120000; /* timeout in ms */



void bad(const char *why) {
     fprintf(stderr,"Fatal error> %s\n",why);
     exit(17);
}

static struct libusb_device_handle *devh = NULL;



 /* Sends command and waits response*/
 static int sendCommand(unsigned char *toSendBuffer, unsigned char *receivedBuffer)
 {
        int r,i;
        int transferred;


     r = libusb_interrupt_transfer(devh, ENDPOINT_INT_OUT, toSendBuffer, 6,
         &transferred,TIMEOUT);
     if (r < 0) {
         cout << "Interrupt write error";
         cout << r;
         return r;
     }
     
     r = libusb_interrupt_transfer(devh, ENDPOINT_INT_IN, receivedBuffer,6,
         &transferred, TIMEOUT_EXEC_CMD);
     if (r < 0) {
         fprintf(stderr, "Interrupt read error %d\n", r);
         cout << "Interrupt read error ";
         cout << r;
         return r;
     }
     
     if (transferred < PACKET_INT_LEN) {
         fprintf(stderr, "Interrupt transfer short read (%d)\n", r);
         cout << "Interrupt transfer short read ";
         cout << r;
         return -1;
     }

     return 0;
 }




 /*******************************************************************************
   *
   *    command function
   *
   ******************************************************************************/
 int command(char cmd)
 {
    // USB INIT
    int r = 1;
    int err = 0;
    unsigned char toSendBuffer[6], receivedBuffer[6];
    ssize_t cnt; //holding number of devices in list
    
    

    r = libusb_init(NULL);
    if (r < 0) {
        fprintf(stderr, "Failed to initialise libusb\n");
        exit(1);
    }
    
    
    
    devh = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
    if (devh == NULL) {
        fprintf(stderr, "Could not open YKUSH control device\n");
        libusb_close(devh);
        libusb_exit(NULL);
        return 0;
    } 
    

    libusb_detach_kernel_driver(devh, 0);

    r = libusb_set_configuration(devh, 1);
    if (r < 0) {
        fprintf(stderr, "libusb_set_configuration error %d\n", r);
        libusb_close(devh);
        libusb_exit(NULL);
        return r >= 0 ? r : -r;
    }
    
    r = libusb_claim_interface(devh, 0);
    if (r < 0) {
        fprintf(stderr, "libusb_claim_interface error %d\n", r);
        libusb_close(devh);
        libusb_exit(NULL);
        return r >= 0 ? r : -r;
    }

    
    /*  SEND COMMANDS */
    toSendBuffer[0]= cmd;
    toSendBuffer[1]= cmd;
    sendCommand(toSendBuffer, receivedBuffer);

    /* Close */   
    libusb_release_interface(devh, 0);
    libusb_reset_device(devh);
    libusb_close(devh);
    libusb_exit(NULL);
    return r >= 0 ? r : -r;

 }

 
 
 
 