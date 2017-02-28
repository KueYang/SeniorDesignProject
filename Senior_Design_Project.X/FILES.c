/**
 * @file FILES.c
 * @author Kue Yang
 * @date 11/22/2016
 * @details The FILES module will handle all file related tasks. Tasks includes:
 * opening and closing files, searching for files and reading files. This module
 * requires the Fatfs File System Library.
 */

#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "./fatfs/diskio.h"
#include "./fatfs/ffconf.h"
#include "./fatfs/ff.h"
#include "FILES.h"

/**  
 * @privatesection
 * @{
 */
FATFS FatFs;			/* File system object */
/** @} */

/**
 * @brief Initializes the FILES module.
 * @details Initializes Microchip MDD File System library. Updates the file attributes
 * that will be used for file related operations. 
 * @remark Requires Fatfs File System library.
 * @return Void
 */
void FILES_Init(void)
{
    // Initialize the sd card to logical drive 0
    while(disk_initialize(0));
    // Mounts the sd card to logical drive 0
    while(f_mount(&FatFs, "", 0));
}

/**
 * @brief Gets the File System Data Structure 
 * @details Gets the File System Data Structure used for file processing.
 * @return Returns the File System Data Structure.
 */
FATFS FILES_getFileSystem(void)
{
    return FatFs;
}

/**
 * @brief Opens a file
 * @details Opens the specified file with the specified access attributes.
 * @arg file The file data structure
 * @arg fileName The name of the file to be read
 * @arg mode The file access mode
 * @return Returns a code indicating if a file successfully opens or not.
 */
FRESULT FILES_OpenFile(FIL* file, const char* fileName, int mode)
{
    return f_open(file, fileName, mode);
}

/**
 * @brief Closes a file
 * @details Closes the specified file
 * @arg file The file data structure
 * @return Returns a code indicating if a file successfully closes or not.
 */
FRESULT FILES_CloseFile(FIL* file)
{
   return f_close(file);
}

/**
 * @brief Displays a list of files
 * @details Displays a list of files and indicates which file is currently selected.
 * @arg selectedName The file name that is selected.
 * @return Returns a boolean indicating if the list of files is found.
 */
BOOL FILES_ListFiles(const char* selectedName)
{
    char buf[128];
    FRESULT res;        /* Stores the results of the operation. */
    DIR dir;            /* Stores information on the directory. */
    FILINFO Finfo;      /* Stores file information. */
    
    MON_SendString("Showing all WAV files in root directory:");
    
    // Searches for the first file in the directory.
    res = f_findfirst(&dir, &Finfo, "", "*.wav");
    if(res == FR_OK)
    {
        while (res == FR_OK && Finfo.fname[0]) 
        {
            // Prints out the file name and file size.
            if(MON_stringsMatch(selectedName, &Finfo.fname[0]))
            {
                // Indicates the file is selected.
                snprintf(&buf[0], 128, "%s\t%u KB ***", Finfo.fname, Finfo.fsize/1000);
            }
            else
            {
                snprintf(&buf[0], 128, "%s\t%u KB", Finfo.fname, Finfo.fsize/1000);
            }
            MON_SendString(&buf[0]);
            
            // Searches for the next file in the directory.
            res = f_findnext(&dir, &Finfo);
        }
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Finds a file
 * @details Finds a specific file
 * @arg dir The directory data structure
 * @arg fileInfo The file data structure
 * @arg fileName The file name to find
 * @return Returns a code indicating if a file successfully found or not.
 */
FRESULT FILES_FindFile(DIR* dir, FILINFO* fileInfo, const char* fileName)
{
   return f_findfirst(dir, fileInfo, fileName, ".wav");
}

/**
 * @brief Reads a file
 * @details Reads a specific file
 * @arg file The file data structure 
 * @arg buffer The buffer to store the bytes read.
 * @arg bytes The number of bytes to read
 * @arg ptr A pointer to the number of bytes read
 * @return Returns a code indicating if a file successfully read or not.
 */
FRESULT FILES_ReadFile(FIL* file, BYTE* buffer, UINT16 bytes, UINT16* ptr)
{
     return f_read(file, buffer, bytes, ptr);
}
