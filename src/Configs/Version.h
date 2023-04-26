#ifndef __VERSION_H
#define __VERSION_H

/* Firmware Version */
#define VERSION_FIRMWARE_NAME   "[SmartStation]"
#define VERSION_SOFTWARE        "v1.0"
#define VERSION_HARDWARE        "v1.1"
#define VERSION_AUTHOR_NAME     "Zhijun_Yang"

/* Number to string macro */
#define _VERSION_NUM_TO_STR_(n)  #n
#define VERSION_NUM_TO_STR(n)   _VERSION_NUM_TO_STR_(n)


/* Build Time */
#define VERSION_BUILD_TIME    __DATE__ "\n" __TIME__


#endif
