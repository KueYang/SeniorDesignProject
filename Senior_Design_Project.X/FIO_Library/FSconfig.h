/**
 * @file FSconfig.h
 * @author Kue Yang
 * @date 11/22/2016
 * @brief Defines the File System configuration settings.
 */

#ifndef _FS_DEF_

// Summary: A macro indicating the maximum number of concurrently open files
// Description: The FS_MAX_FILES_OPEN #define is only applicable when dynamic memory allocation is not used (FS_DYNAMIC_MEM is not defined).
//              This macro defines the maximum number of open files at any given time.  The amount of RAM used by FSFILE objects will
//              be equal to the size of an FSFILE object multipled by this macro value.  This value should be kept as small as possible
//              as dictated by the application.  This will reduce memory usage.
#define FS_MAX_FILES_OPEN 	19


// Summary: A macro defining the size of a sector
// Description: The MEDIA_SECTOR_SIZE macro will define the size of a sector on the FAT file system.  This value must equal 512 bytes,
//              1024 bytes, 2048 bytes, or 4096 bytes.  The value of a sector will usually be 512 bytes.
#define MEDIA_SECTOR_SIZE 		512


/* *******************************************************************************************************/
/************** Compiler options to enable/Disable Features based on user's application ******************/
/* *******************************************************************************************************/

// Summary: A macro to enable/disable file search functions.
// Description: The ALLOW_FILESEARCH definition can be commented out to disable file search functions in the library.  This will
//              prevent the use of the FindFirst and FindNext functions and reduce code size.
#define ALLOW_FILESEARCH

// Summary: A macro to enable/disable FAT32 support.
// Description: The SUPPORT_FAT32 definition can be commented out to disable support for FAT32 functionality.  This will save a small amount
//              of code space.
#define SUPPORT_FAT32

// Function definitions
// Associate the physical layer functions with the correct physical layer
#ifdef USE_SD_INTERFACE_WITH_SPI       // SD-SPI.c and .h

    // Description: Function pointer to the Media Initialize Physical Layer function
    #define MDD_MediaInitialize     MDD_SDSPI_MediaInitialize

    // Description: Function pointer to the Media Detect Physical Layer function
    #define MDD_MediaDetect         MDD_SDSPI_MediaDetect

    // Description: Function pointer to the Sector Read Physical Layer function
    #define MDD_SectorRead          MDD_SDSPI_SectorRead

    // Description: Function pointer to the Sector Write Physical Layer function
    #define MDD_SectorWrite         MDD_SDSPI_SectorWrite

    // Description: Function pointer to the I/O Initialization Physical Layer function
    #define MDD_InitIO              MDD_SDSPI_InitIO

    // Description: Function pointer to the Media Shutdown Physical Layer function
    #define MDD_ShutdownMedia       MDD_SDSPI_ShutdownMedia

    // Description: Function pointer to the Write Protect Check Physical Layer function
    #define MDD_WriteProtectState   MDD_SDSPI_WriteProtectState

    // Description: Function pointer to the Read Capacity Physical Layer function
    #define MDD_ReadCapacity        MDD_SDSPI_ReadCapacity

    // Description: Function pointer to the Read Sector Size Physical Layer Function
    #define MDD_ReadSectorSize      MDD_SDSPI_ReadSectorSize
#endif

#endif
