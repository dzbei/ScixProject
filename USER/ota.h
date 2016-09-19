#ifndef __OTA_H_
#define __OTA_H_
#include "sys.h"
#define OTA_HEAD      0xEE
#define OTA_END				0x55

u8 ota_prase(u8 *buffer, u16 all_length);
void ask_for_update(void);

#endif