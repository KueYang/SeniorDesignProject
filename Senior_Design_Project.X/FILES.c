/**
 * @file FILES.c
 * @author Kue Yang
 * @date 11/22/2016
 * @brief The FILES module.
 */

#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "./FIO_Library/FSIO.h"
#include "FILES.h"

/**  
 * @privatesection
 * @{
 */
/** @var Stores the attributes of files to be read. */
BYTE file_attributes;
/** @} */

/**
 * @brief Initializes the FILES module.
 * @return Void
 */
void FILES_Init(void)
{
    // Initialize the library
    while(!FSInit());
    
    // Files and Search configurations.
    file_attributes = ATTR_ARCHIVE | ATTR_READ_ONLY | ATTR_HIDDEN;
}

/**
 * @brief Opens a given file to be read.
 * @param fileName The name of the file.
 * @param pointer A pointer that will be used to reference the open file.
 * @param rec A pointer that will store information on the opened file.
 * @return Returns a boolean indicating if the file was open successfully.
 * @retval TRUE if the file was open successfully.
 * @retval FALSE if the file was open unsuccessfully.
 */
BOOL FILES_OpenFile(const char* fileName, FSFILE* pointer , SearchRec* rec)
{
    if(FILES_FindFile(fileName, rec))
    {
        pointer = FSfopen(fileName, "r");
        if (pointer == NULL)
        {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Closes an open file.
 * @param pointer The pointer that points to the file.
 * @return Returns a boolean indicating if the file was closed successfully.
 * @retval TRUE if the file was closed successfully.
 * @retval FALSE if the file was closed unsuccessfully.
 */
BOOL FILES_CloseFile(FSFILE* pointer)
{
   if(FSfclose(pointer))
   {
       return FALSE;
   }
   return TRUE;
}

/**
 * @brief Displays a list of files.
 * @param rec A pointer that has information on an opened file.
 * @remarks Requires the UART module to initialized.
 * @return Returns a boolean indicating if there are files to be displayed.
 * @retval TRUE if there are files listed.
 * @retval FALSE if there are no files to be listed.
 */
BOOL FILES_ListFiles(SearchRec* rec)
{
    char buf[FILENAME_LENGTH];
    
    UART_sendString("\n\rShowing all WAV files in root directory:\n\r");
    if (FindFirst("*.WAV", file_attributes, rec) == 0) 
    {
        snprintf(buf, FILENAME_LENGTH, "%s\t%u KB \n\r", rec->filename, rec->filesize/1000);
        UART_sendString(&buf[0]);
        while (FindNext(rec) == 0) 
        {
            snprintf(buf, FILENAME_LENGTH, "%s\t%u KB \n\r", rec->filename, rec->filesize/1000);
            UART_sendString(&buf[0]);
        }
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Finds a file.
 * @param fileName The name of the file to be searched for.
 * @param rec A pointer that has information on an opened file.
 * @remarks Requires the UART module to initialized.
 * @return Returns a boolean indicating if the specified file is found.
 * @retval TRUE if there the specified file is found.
 * @retval FALSE if there the specified file is not found.
 */
BOOL FILES_FindFile(const char* fileName, SearchRec* rec)
{
   if(FindFirst(fileName, file_attributes, rec) == 0)
   {
       return TRUE;
   }
   return FALSE;
}

/**
 * @brief Reads from a file.
 * @param buffer The buffer used to store the data read from the specified file.
 * @param bytes The number of bytes to be read from the file.
 * @param blocks The block sizes of the data to be read.
 * @param pointer A pointer to the file being read.
 * @return Returns a boolean indicating if the specified file is found.
 * @retval TRUE if there the specified file is found.
 * @retval FALSE if there the specified file is not found.
 */
BOOL FILES_ReadFile(BYTE* buffer, UINT8 bytes, UINT32 blocks, FSFILE* pointer)
{
    /* Reads bytes-Byte blocks from the file and stores it in the receive buffer. */
    if(FSfread(buffer, bytes, blocks, pointer) != blocks)
    {
        return FALSE;
    }
    return TRUE;
}


