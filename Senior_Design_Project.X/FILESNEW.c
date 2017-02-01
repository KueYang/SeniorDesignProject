/**
 * @file FILESNEW.c
 * @author Kue Yang
 * @date 11/22/2016
 * @details The FILESNEW module will handle all file related tasks. Tasks includes:
 * opening and closing files, searching for files and reading files. 
 */

#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "./fatfs/diskio.h"
#include "./fatfs/ffconf.h"
#include "./fatfs/ff.h"
#include "FILESNEW.h"

/**  
 * @privatesection
 * @{
 */
FATFS FatFs;			/* File system object */
/** @} */

/**
 * @brief Initializes the FILESNEW module.
 * @details Initializes Microchip MDD File System library. Updates the file attributes
 * that will be used for file related operations. 
 * @remark Requires Microchip's MDD File System library.
 * @return Void
 */
void FILESNEW_Init(void)
{
    // Initialize the sd card to logical drive 0
    while(disk_initialize(0));
    // Mounts the sd card to logical drive 0
    while(f_mount(&FatFs, "", 0));
}

FATFS FILESNEW_getFileSystem(void)
{
    return FatFs;
}

FRESULT FILESNEW_OpenFile(FIL* file, const char* fileName, int mode)
{
    return f_open(file, fileName, mode);
}

FRESULT FILESNEW_CloseFile(FIL* file)
{
   return f_close(file);
}

BOOL FILESNEW_ListFiles(void)
{
    char buf[128];
    FRESULT res;        /* Stores the results of the operation. */
    DIR dir;            /* Stores information on the directory */
    FILINFO Finfo;      /* Stores file information. */
    
    MON_SendString("Showing all WAV files in root directory:");
    
    // Searches for the first file in the directory.
    res = f_findfirst(&dir, &Finfo, "", "*.wav");
    if(res == FR_OK)
    {
        while (res == FR_OK && Finfo.fname[0]) {
            // Prints out the file name and file size.
            snprintf(&buf[0], 128, "%s\t%u KB", Finfo.fname, Finfo.fsize/1000);
            MON_SendString(&buf[0]);
            
            // Searches for the next file in the directory.
            res = f_findnext(&dir, &Finfo);
        }
        return TRUE;
    }
    return FALSE;
}

FRESULT FILESNEW_FindFile(DIR* dir, FILINFO* fileInfo, const char* fileName)
{
   return f_findfirst(dir, fileInfo, fileName, ".wav");
}

FRESULT FILESNEW_ReadFile(FIL* file, BYTE* buffer, UINT16 bytes, UINT16* ptr)
{
     return f_read(file, buffer, bytes, ptr);
}



