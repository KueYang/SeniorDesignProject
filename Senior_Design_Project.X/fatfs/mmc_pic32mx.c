/*------------------------------------------------------------------------/
/  MMCv3/SDv1/SDv2 (in SPI mode) control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2014, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include <p32xxxx.h>
#include "diskio.h"
#include "../SPI.h"


/* Socket controls  (Platform dependent) */
#define CS_LOW()  _LATB7 = 0       /* MMC CS = L */
#define CS_HIGH() _LATB7 = 1       /* MMC CS = H */
#define CD	1//(!_RB8)                /* Card detected   (yes:true, no:false, default:true) */
#define WP	(_RB10)                 /* Write protected (yes:true, no:false, default:false) */


/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* Definitions for MMC/SDC command */
#define CMD0   (0)			/* GO_IDLE_STATE */
#define CMD1   (1)			/* SEND_OP_COND */
#define ACMD41 (41|0x80)	/* SEND_OP_COND (SDC) */
#define CMD8   (8)			/* SEND_IF_COND */
#define CMD9   (9)			/* SEND_CSD */
#define CMD10  (10)			/* SEND_CID */
#define CMD12  (12)			/* STOP_TRANSMISSION */
#define ACMD13 (13|0x80)	/* SD_STATUS (SDC) */
#define CMD16  (16)			/* SET_BLOCKLEN */
#define CMD17  (17)			/* READ_SINGLE_BLOCK */
#define CMD18  (18)			/* READ_MULTIPLE_BLOCK */
#define CMD23  (23)			/* SET_BLOCK_COUNT */
#define ACMD23 (23|0x80)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24  (24)			/* WRITE_BLOCK */
#define CMD25  (25)			/* WRITE_MULTIPLE_BLOCK */
#define CMD41  (41)			/* SEND_OP_COND (ACMD) */
#define CMD55  (55)			/* APP_CMD */
#define CMD58  (58)			/* READ_OCR */


static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */

static volatile
UINT16 Timer1, Timer2;		/* 1000Hz decrement timer */

static
UINT16 CardType;

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/
int wait_ready (void)
{
	BYTE d;

	Timer2 = 500;               /* Wait for ready in timeout of 500ms */
	do
    {
		d = SPI3_ReadWrite(0xFF);
	}while ((d != 0xFF) && Timer2);

	return ((d == 0xFF) ? 1 : 0);
}

/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/
void deselect (void)
{
	CS_HIGH();                  /* Set CS# high */
	SPI3_ReadWrite(0xFF);		/* Dummy clock (force DO hi-z for multiple slave SPI) */
}

/*-----------------------------------------------------------------------*/
/* Select the card and wait ready                                        */
/*-----------------------------------------------------------------------*/
int select (void)	/* 1:Successful, 0:Timeout */
{
	CS_LOW();                   /* Set CS# low */
	SPI3_ReadWrite(0xFF);       /* Dummy clock (force DO enabled) */

	if (wait_ready()) 
    {
        return 1;               /* Wait for card ready */
    }

	deselect();
	return 0;                   /* Timeout */
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/
/* buff - Data buffer to store received data */
/* btr - Byte count (must be multiple of 4) */
/* 1:OK, 0:Failed */
int rcvr_datablock ( BYTE *buff, UINT16 btr	)
{
	BYTE token;

	Timer1 = 100;
	do {							/* Wait for data packet in timeout of 100ms */
		token = SPI3_ReadWrite(0xFF);
	} while ((token == 0xFF) && Timer1);

	if(token != 0xFE) 
    {
        return 0;                   /* If not valid data token, retutn with error */
    }

	SPI3_MultiRead(buff, btr);		/* Receive the data block into buffer */
	SPI3_ReadWrite(0xFF);			/* Discard CRC */
	SPI3_ReadWrite(0xFF);

	return 1;						/* Return with success */
}

/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/
/* cmd - Command byte */
// arg - Argument
BYTE send_cmd (
	BYTE cmd,		/* Command byte */
	DWORD arg		/* Argument */
)
{
	BYTE n, res;

	if (cmd & 0x80)                     /* ACMD<n> is the command sequense of CMD55-CMD<n> */
	{
        cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) 
    {
		deselect();
		if (!select())
        {
            return 0xFF;
        }
	}

	/* Send command packet */
	SPI3_ReadWrite(0x40 | cmd);			/* Start + Command index */
	SPI3_ReadWrite((BYTE)(arg >> 24));	/* Argument[31..24] */
	SPI3_ReadWrite((BYTE)(arg >> 16));	/* Argument[23..16] */
	SPI3_ReadWrite((BYTE)(arg >> 8));	/* Argument[15..8] */
	SPI3_ReadWrite((BYTE)arg);			/* Argument[7..0] */
    
	n = 0x01;                           /* Dummy CRC + Stop */
	if (cmd == CMD0){ 
        n = 0x95;                       /* Valid CRC for CMD0(0) + Stop */
    }
	if (cmd == CMD8){ 
        n = 0x87;                       /* Valid CRC for CMD8(0x1AA) + Stop */
    }
	SPI3_ReadWrite(n);

	/* Receive command response */
	if (cmd == CMD12) 
    {
        SPI3_ReadWrite(0xFF);           /* Skip a stuff byte on stop to read */
    }
	
    n = 10;                             /* Wait for a valid response in timeout of 10 attempts */
	do
    {
		res = SPI3_ReadWrite(0xFF);
	}while ((res & 0x80) && --n);

	return res;                         /* Return with the response value */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
/* pdrv - Physical drive nmuber (0) */
DSTATUS disk_status ( BYTE pdrv )
{
	if (pdrv != 0)
    {
        return STA_NOINIT;	/* Supports only single drive */
    }
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
/* pdrv - Physical drive nmuber (0) */
DSTATUS disk_initialize ( BYTE pdrv )
{
	BYTE n, cmd, ty, ocr[4];

	if (pdrv != 0) 
    {
        return STA_NOINIT;                                          /* Supports only single drive */
    }	
	if (Stat & STA_NODISK){
        return Stat;                                                /* No card in the socket */
    }

	SPI3_Init(400000);                                              /* Initialize memory card interface at 400kHz. */
	for (n = 10; n; n--) 
    {
        SPI3_ReadWrite(0xFF);                                       /* 80 dummy clocks */
    }	

	ty = 0;
	if (send_cmd(CMD0, 0) == 1)                                     /* Enter Idle state */
    {                                   
		Timer1 = 1000;                                              /* Initialization timeout of 1000 msec */
		if (send_cmd(CMD8, 0x1AA) == 1)                             /* SDv2? */
        {                           
			for (n = 0; n < 4; n++) 
            {
                ocr[n] = SPI3_ReadWrite(0xFF);                      /* Get trailing return value of R7 resp */
            }			
			if (ocr[2] == 0x01 && ocr[3] == 0xAA)                   /* The card can work at vdd range of 2.7-3.6V */
            {				
				while (Timer1 && send_cmd(ACMD41, 0x40000000));     /* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (Timer1 && send_cmd(CMD58, 0) == 0)              /* Check CCS bit in the OCR */
                {			
					for (n = 0; n < 4; n++) 
                    {
                        ocr[n] = SPI3_ReadWrite(0xFF);
                    }
					ty = (ocr[0] & 0x40) ? (CT_SD2|CT_BLOCK) : CT_SD2;/* SDv2 */
				}
			}
		} 
        else 
        {                                                    /* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1)
            {
				ty = CT_SD1; cmd = ACMD41;                          /* SDv1 */
			} 
            else 
            {
				ty = CT_MMC; cmd = CMD1;                            /* MMCv3 */
			}
            
			while (Timer1 && send_cmd(cmd, 0));                     /* Wait for leaving idle state */
			if (!Timer1 || send_cmd(CMD16, 512) != 0)               /* Set read/write block length to 512 */
            {
                ty = 0;
            }
		}
	}
	CardType = ty;
	deselect();

	if (ty)                                 /* Function succeded */
    {		
		Stat &= ~STA_NOINIT;                /* Clear STA_NOINIT */
		SPI3_Init(20000000);   
	} 
    else                                    /* Function failed */ 
    {		
		SPI3CONbits.ON = 0;                 /* Deinitialize interface */
	}

	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
/* pdrv - Physical drive nmuber (0) */
/* buff - Pointer to the data buffer to store read data */
/* sector - Start sector number (LBA) */
/* count - Sector count (1..128) */
DRESULT disk_read ( BYTE pdrv, BYTE *buff, DWORD sector, UINT16 count )
{
	if (pdrv || !count) 
    {
        return RES_PARERR;
    }
	if (Stat & STA_NOINIT)
    {
        return RES_NOTRDY;
    }

	if (!(CardType & CT_BLOCK))
    {
        sector *= 512;	/* Convert to byte address if needed */
    }

	if (count == 1) {		/* Single block read */
		if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
			&& rcvr_datablock(buff, 512))
			count = 0;
	}
	else {                                  /* Multiple block read */
		if (send_cmd(CMD18, sector) == 0)   /* READ_MULTIPLE_BLOCK */
        {	
			do {
				if (!rcvr_datablock(buff, 512)) 
                {
                    break;
                }
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	deselect();

	return (count ? RES_ERROR : RES_OK);
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	DWORD csz;


	if (pdrv) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;
	switch (cmd) {
	case CTRL_SYNC :	/* Flush write-back cache, Wait for end of internal process */
		if (select()) res = RES_OK;
		break;

	case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (WORD) */
		if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
			if ((csd[0] >> 6) == 1) {	/* SDv2? */
				csz = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
				*(DWORD*)buff = csz << 10;
			} else {					/* SDv1 or MMCv3 */
				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
				csz = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
				*(DWORD*)buff = csz << (n - 9);
			}
			res = RES_OK;
		}
		break;

	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sectors (DWORD) */
		if (CardType & CT_SD2) {	/* SDv2? */
			if (send_cmd(ACMD13, 0) == 0) {		/* Read SD status */
				SPI3_ReadWrite(0xFF);
				if (rcvr_datablock(csd, 16)) {				/* Read partial block */
					for (n = 64 - 16; n; n--) SPI3_ReadWrite(0xFF);	/* Purge trailing data */
					*(DWORD*)buff = 16UL << (csd[10] >> 4);
					res = RES_OK;
				}
			}
		} else {					/* SDv1 or MMCv3 */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
				if (CardType & CT_SD1) {	/* SDv1 */
					*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				} else {					/* MMCv3 */
					*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = RES_OK;
			}
		}
		break;

	case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
		*ptr = CardType;
		res = RES_OK;
		break;

	case MMC_GET_CSD :	/* Receive CSD as a data block (16 bytes) */
		if ((send_cmd(CMD9, 0) == 0)	/* READ_CSD */
			&& rcvr_datablock(buff, 16))
			res = RES_OK;
		break;

	case MMC_GET_CID :	/* Receive CID as a data block (16 bytes) */
		if ((send_cmd(CMD10, 0) == 0)	/* READ_CID */
			&& rcvr_datablock(buff, 16))
			res = RES_OK;
		break;

	case MMC_GET_OCR :	/* Receive OCR as an R3 resp (4 bytes) */
		if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
			for (n = 0; n < 4; n++)
				*((BYTE*)buff+n) = SPI3_ReadWrite(0xFF);
			res = RES_OK;
		}
		break;

	case MMC_GET_SDSTAT :	/* Receive SD statsu as a data block (64 bytes) */
		if ((CardType & CT_SD2) && send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
			SPI3_ReadWrite(0xFF);
			if (rcvr_datablock(buff, 64))
				res = RES_OK;
		}
		break;

	case CTRL_POWER_OFF :	/* Power off */
		SPI3CONbits.ON = 0;
		Stat |= STA_NOINIT;
		res = RES_OK;
		break;

	default:
		res = RES_PARERR;
	}

	deselect();

	return res;
}
#endif

/*-----------------------------------------------------------------------*/
/* Device Timer Driven Procedure                                         */
/*-----------------------------------------------------------------------*/
/* This function must be called by timer interrupt in period of 1ms      */
void disk_timerproc (void)
{
	BYTE s;
	UINT16 n;

	n = Timer1;					/* 1000Hz decrement timer with zero stopped */
	if (n) Timer1 = --n;
	n = Timer2;
	if (n) Timer2 = --n;


	/* Update socket status */
	s = Stat;

	if (WP) s |= STA_PROTECT;
	else	s &= ~STA_PROTECT;

	if (CD) s &= ~STA_NODISK;
	else	s |= (STA_NODISK | STA_NOINIT);

	Stat = s;
}

