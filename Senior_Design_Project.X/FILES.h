/**
 * @file FILES.h
 * @author Kue Yang
 * @date 11/22/2016
 */

#ifndef FILES_H
#define	FILES_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "./fatfs/ff.h"

/**
 * @brief AUDIOINFO data structure.
 * @details The AUDIOINFO data structure is used to store the audio header
 * data and file name.
 */
typedef struct AUDIOINFO
{
    /**@{*/
    UINT16  bitsPerSample;      /**< Variable used to store the bits per sample. */
    UINT16  numOfChannels;      /**< Variable used to store the number of channels. */
    UINT16  sampleRate;         /**< Variable used to store the sample rate. */
    UINT16  blockAlign;         /**< Variable used to store the block align. */
    UINT32  dataSize;           /**< Variable used to store the size of the file data. */
    char fileName[16];          /**< Variable used to store the file name. */
    /**@}*/
}AUDIOINFO;

/**
 * @brief FILES data structure.
 * @details The FILES data structure is used to store the file data used in
 * conjunction with the Fatfs File System Library. The structure also stores the
 * audio header data corresponding to the specified file.
 */
typedef struct FILES
{
    /**@{*/
    FIL File;			/* File objects */
    FSIZE_t startPtr;
    DWORD cluster;
    DWORD sector;
    AUDIOINFO audioInfo;
    /**@}*/
}FILES;

FRESULT FILES_ReadFile(FIL* file, BYTE* buffer, UINT16 bytes, UINT16* ptr);
FRESULT FILES_FindFile(DIR* dir, FILINFO* fileInfo, const char* fileName);
BOOL FILES_ListFiles(const char* selectedName);
FRESULT FILES_CloseFile(FIL* file);
FRESULT FILES_OpenFile(FIL* file, const char* fileName, int mode);

#ifdef	__cplusplus
}
#endif

#endif	/* FILES_H */

