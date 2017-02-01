/* 
 * File:   mmc_pic32mx.h
 * Author: Belinda Yang
 *
 * Created on January 27, 2017, 9:24 AM
 */

#ifndef MMC_PIC32MX_H
#define	MMC_PIC32MX_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "diskio.h"
    
DSTATUS disk_status ( BYTE pdrv );
DSTATUS disk_initialize ( BYTE pdrv );
DRESULT disk_read ( BYTE pdrv, BYTE *buff, DWORD sector, UINT16 count );
void disk_timerproc (void);


#ifdef	__cplusplus
}
#endif

#endif	/* MMC_PIC32MX_H */

