/**
 * @file FSIO.c
 * @author Microchip
 * @date 11/22/2016
 */

#include <plib.h>
#include "STDDEF.h"
#include "../UART.h"
#include "FSIO.h"
#include "FSDefs.h"

/*****************************************************************************/
/*                         Global Variables                                  */
/*****************************************************************************/
FSFILE  gFileArray[FS_MAX_FILES_OPEN];      // Array that contains file information (static allocation)
BYTE    gFileSlotOpen[FS_MAX_FILES_OPEN];   // Array that indicates which elements of gFileArray are available for use

DWORD       gLastFATSectorRead = 0xFFFFFFFF;    // Global variable indicating which FAT sector was read last
BYTE        gNeedFATWrite = FALSE;              // Global variable indicating that there is information that needs to be written to the FAT
FSFILE  *   gBufferOwner = NULL;                // Global variable indicating which file is using the data buffer
DWORD       gLastDataSectorRead = 0xFFFFFFFF;   // Global variable indicating which data sector was read last
BYTE        gNeedDataWrite = FALSE;             // Global variable indicating that there is information that needs to be written to the data section
BYTE        nextClusterIsLast = FALSE;          // Global variable indicating that the entries in a directory align with a cluster boundary

BYTE    gBufferZeroed = FALSE;      // Global variable indicating that the data buffer contains all zeros

DWORD   FatRootDirClusterValue;     // Global variable containing the cluster number of the root dir (0 for FAT12/16)

BYTE    FSerrno;                    // Global error variable.  Set to one of many error codes after each function call.

DWORD   TempClusterCalc;            // Global variable used to store the calculated value of the cluster of a specified sector.
BYTE    dirCleared;                 // Global variable used by the "recursive" FSrmdir function to indicate that all subdirectories and files have been deleted from the target directory.
BYTE    recache = FALSE;            // Global variable used by the "recursive" FSrmdir function to indicate that additional cache reads are needed.
FSFILE  tempCWDobj;                 // Global variable used to preserve the current working directory information.
FSFILE  gFileTemp;                  // Global variable used for file operations.

FSFILE   cwd;               // Global current working directory
FSFILE * cwdptr = &cwd;     // Pointer to the current working directory

BYTE __attribute__ ((aligned(4)))   gDataBuffer[MEDIA_SECTOR_SIZE];     // The global data sector buffer
BYTE __attribute__ ((aligned(4)))   gFATBuffer[MEDIA_SECTOR_SIZE];      // The global FAT sector buffer

DISK gDiskData;         // Global structure containing device information.

/* Global Variables to handle ASCII & UTF16 file operations */
char *asciiFilename;
unsigned short int fileNameLength;
    
/************************************************************************/
/*                        Structures and defines                        */
/************************************************************************/
// Directory entry structure
typedef struct
{
    char      DIR_Name[DIR_NAMESIZE];           // File name
    char      DIR_Extension[DIR_EXTENSION];     // File extension
    BYTE      DIR_Attr;                         // File attributes
    BYTE      DIR_NTRes;                        // Reserved byte
    BYTE      DIR_CrtTimeTenth;                 // Create time (millisecond field)
    WORD      DIR_CrtTime;                      // Create time (second, minute, hour field)
    WORD      DIR_CrtDate;                      // Create date
    WORD      DIR_LstAccDate;                   // Last access date
    WORD      DIR_FstClusHI;                    // High word of the entry's first cluster number
    WORD      DIR_WrtTime;                      // Last update time
    WORD      DIR_WrtDate;                      // Last update date
    WORD      DIR_FstClusLO;                    // Low word of the entry's first cluster number
    DWORD     DIR_FileSize;                     // The 32-bit file size
}_DIRENTRY;

typedef _DIRENTRY * DIRENTRY;                   // A pointer to a directory entry structure

// Value indicating that the CreateFileEntry function will be creating a directory
#define DIRECTORY 0x12          
// The number of directory entries in a sector
#define DIRENTRIES_PER_SECTOR   (MEDIA_SECTOR_SIZE / 32)        
// Maximum number of UTF16 words in single Root directory entry
#define MAX_UTF16_CHARS_IN_LFN_ENTRY      (BYTE)13

// Long File Name Entry
typedef struct
{
   BYTE LFN_SequenceNo;   // Sequence number,
   BYTE LFN_Part1[10];    // File name part 1
   BYTE LFN_Attribute;    // File attribute
   BYTE LFN_Type;		// LFN Type
   BYTE LFN_Checksum;     // Checksum
   unsigned short int LFN_Part2[6];    // File name part 2
   unsigned short int LFN_Reserved2;	// Reserved for future use
   unsigned short int LFN_Part3[2];     // File name part 3
}LFN_ENTRY;

/* Summary: Possible type of file or directory name.
** Description: See the FormatFileName() & FormatDirName() function for more information.
*/
typedef enum
{
    NAME_8P3_ASCII_CAPS_TYPE,
    NAME_8P3_ASCII_MIXED_TYPE,
    NAME_8P3_UTF16_TYPE, // SUBTYPES OF 8P3 UTF16 TYPE
	    NAME_8P3_UTF16_ASCII_CAPS_TYPE,
	    NAME_8P3_UTF16_ASCII_MIXED_TYPE,
	    NAME_8P3_UTF16_NONASCII_TYPE,
    NAME_LFN_TYPE,
    NAME_ERROR
} FILE_DIR_NAME_TYPE;

// internal errors
#define CE_FAT_EOF            60   // Error that indicates an attempt to read FAT entries beyond the end of the file
#define CE_EOF                61   // Error that indicates that the end of the file has been reached

typedef FSFILE   * FILEOBJ;         // Pointer to an FSFILE object

static const char s_digits[] = "0123456789abcdef";          // FSfprintf table of conversion digits

/************************************************************************************/
/*                               Prototypes                                         */
/************************************************************************************/

DWORD ReadFAT (DISK *dsk, DWORD ccls);
DIRENTRY Cache_File_Entry( FILEOBJ fo, WORD * curEntry, BYTE ForceRead);
BYTE Fill_File_Object(FILEOBJ fo, WORD *fHandle);
DWORD Cluster2Sector(DISK * disk, DWORD cluster);
DIRENTRY LoadDirAttrib(FILEOBJ fo, WORD *fHandle);
BYTE ReadByte( BYTE* pBuffer, WORD index );
WORD ReadWord( BYTE* pBuffer, WORD index );
DWORD ReadDWord( BYTE* pBuffer, WORD index );

void FileObjectCopy(FILEOBJ foDest,FILEOBJ foSource);
FILE_DIR_NAME_TYPE ValidateChars(BYTE mode);
BYTE FormatFileName( const char* fileName, FILEOBJ fptr, BYTE mode);
CETYPE FILEfind( FILEOBJ foDest, FILEOBJ foCompareTo, BYTE cmd, BYTE mode);
CETYPE FILEopen (FILEOBJ fo, WORD *fHandle, char type);

BYTE DISKmount( DISK *dsk);
BYTE LoadMBR(DISK *dsk);
BYTE LoadBootSector(DISK *dsk);
DWORD GetFullClusterNumber(DIRENTRY entry);

/*************************************************************************
  Function:
    int FSInit(void)
  Summary:
    Function to initialize the device.
  Conditions:
    The physical device should be connected to the microcontroller.
  Input:
    None
  Return Values:
    TRUE -  Initialization successful
    FALSE - Initialization unsuccessful
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    This function initializes the file system stack & the interfacing device.
    Initializes the static or dynamic memory slots for holding file
    structures. Initializes the device with the DISKmount function. Loads 
    MBR and boot sector information. Initializes the current working
    directory to the root directory for the device if directory support
    is enabled.
  Remarks:
    None
  *************************************************************************/
int FSInit(void)
{
    int fIndex;

    gBufferZeroed = FALSE;
    gNeedFATWrite = FALSE;             
    gLastFATSectorRead = 0xFFFFFFFF;       
    gLastDataSectorRead = 0xFFFFFFFF;  

    MDD_SDSPI_InitIO();

    if(DISKmount(&gDiskData) == CE_GOOD)
    {
        /* Clears slots for opening files. */
        int i;
        for(i = 0; i <FS_MAX_FILES_OPEN; i++)
        {
            gFileSlotOpen[i] = TRUE;
        }

        FSerrno = 0;
        return TRUE;
    }

    return FALSE;
}


/********************************************************************************
  Function:
    CETYPE FILEfind (FILEOBJ foDest, FILEOBJ foCompareTo, BYTE cmd, BYTE mode)
  Summary
    Finds a file on the device
  Conditions:
    This function should not be called by the user.
  Input:
    foDest -       FSFILE object containing information of the file found
    foCompareTo -  FSFILE object containing the name/attr of the file to be
                   found
    cmd -
        -          LOOK_FOR_EMPTY_ENTRY: Search for empty entry.
        -          LOOK_FOR_MATCHING_ENTRY: Search for matching entry.
    mode -
         -         0: Match file exactly with default attributes.
         -         1: Match file to user-specified attributes.
  Return Values:
    CE_GOOD -            File found.
    CE_FILE_NOT_FOUND -  File not found.
  Side Effects:
    None.
  Description:
    The FILEfind function will sequentially cache directory entries within
    the current working directory into the foDest FSFILE object.  If the cmd
    parameter is specified as LOOK_FOR_EMPTY_ENTRY the search will continue
    until an empty directory entry is found. If the cmd parameter is specified
    as LOOK_FOR_MATCHING_ENTRY these entries will be compared to the foCompareTo
    object until a match is found or there are no more entries in the current
    working directory. If the mode is specified a '0' the attributes of the FSFILE
    entries are irrelevant. If the mode is specified as '1' the attributes of the
    foDest entry must match the attributes specified in the foCompareTo file and
    partial string search characters may bypass portions of the comparison.
  Remarks:
    None
  ********************************************************************************/
CETYPE FILEfind( FILEOBJ foDest, FILEOBJ foCompareTo, BYTE cmd, BYTE mode)
{
    WORD   attrib, compareAttrib;
    WORD   fHandle = foDest->entry;                  // current entry counter
    CETYPE   statusB = CE_FILE_NOT_FOUND;
    BYTE   character,test,state,index;

    // reset the cluster
    foDest->dirccls = foDest->dirclus;
	// Attribute to be compared as per application layer request
    compareAttrib = 0xFFFF ^ foCompareTo->attributes;

    if (fHandle == 0)
    {
        if (Cache_File_Entry(foDest, &fHandle, TRUE) == NULL)
        {
            statusB = CE_BADCACHEREAD;
        }
    }
    else
    {
		// Maximum 16 entries possible
        if ((fHandle & MASK_MAX_FILE_ENTRY_LIMIT_BITS) != 0)
        {
            if (Cache_File_Entry (foDest, &fHandle, TRUE) == NULL)
            {
                statusB = CE_BADCACHEREAD;
            }
        }
    }

    if (statusB != CE_BADCACHEREAD)
    {
        // Loop until you reach the end or find the file
        while(1)
        {
            if(statusB != CE_GOOD) //First time entry always here
            {
                state = Fill_File_Object(foDest, &fHandle);

                if(state == NO_MORE) // Reached the end of available files. Comparision over and file not found so quit.
                {
                    break;
                }
            }
            else // statusB == CE_GOOD then exit
            {
                break; // Code below intializes"statusB = CE_GOOD;" so, if no problem in the filled file, Exit the while loop.
            }

            if(state == FOUND) // Validate the correct matching of filled file data with the required(to be found) one.
            {
                /* We got something */
                // get the attributes
                attrib = foDest->attributes;

                attrib &= ATTR_MASK;
                switch (mode)
                {
                    case 0:
                        // see if we are a volume id or hidden, ignore
                        if(attrib != ATTR_VOLUME)
                        {
                            statusB = CE_GOOD;
                            character = (BYTE)'m'; // random value

                            // search for one. if status = TRUE we found one
                            for(index = 0; index < DIR_NAMECOMP; index++)
                            {
                                // get the source character
                                character = foDest->name[index];
                                // get the destination character
                                test = foCompareTo->name[index];
                                if(tolower(character) != tolower(test))
                                {
                                    statusB = CE_FILE_NOT_FOUND; // Nope its not a match
                                    break;
                                }
                            }// for loop
                        } // not dir nor vol
                        break;

                    case 1:
                        // Check for attribute match
                        if (((attrib & compareAttrib) == 0) && (attrib != ATTR_LONG_NAME))
                        {
                            statusB = CE_GOOD;                 // Indicate the already filled file data is correct and go back
                            character = (BYTE)'m';             // random value
                            if (foCompareTo->name[0] != '*')   //If "*" is passed for comparion as 1st char then don't proceed. Go back, file alreay found.
                            {
                                for (index = 0; index < DIR_NAMESIZE; index++)
                                {
                                    // Get the source character
                                    character = foDest->name[index];
                                    // Get the destination character
                                    test = foCompareTo->name[index];
                                    if (test == '*')
                                        break;
                                    if (test != '?')
                                    {
                                        if(tolower(character) != tolower(test))
                                        {
                                            statusB = CE_FILE_NOT_FOUND; // it's not a match
                                            break;
                                        }
                                    }
                                }
                            }

                            // Before calling this "FILEfind" fn, "formatfilename" must be called. Hence, extn always starts from position "8".
                            if ((foCompareTo->name[8] != '*') && (statusB == CE_GOOD))
                            {
                                for (index = 8; index < DIR_NAMECOMP; index++)
                                {
                                    // Get the source character
                                    character = foDest->name[index];
                                    // Get the destination character
                                    test = foCompareTo->name[index];
                                    if (test == '*')
                                        break;
                                    if (test != '?')
                                    {
                                        if(tolower(character) != tolower(test))
                                        {
                                            statusB = CE_FILE_NOT_FOUND; // it's not a match
                                            break;
                                        }
                                    }
                                }
                            }

                        } // Attribute match

                        break;
                }
            } // not found
            else
            {
                /*** looking for an empty/re-usable entry ***/
                if ( cmd == LOOK_FOR_EMPTY_ENTRY)
                    statusB = CE_GOOD;
            } // found or not
            // increment it no matter what happened
            fHandle++;
        }// while
    }
    
    return(statusB);
} // FILEFind


/**************************************************************************
  Function:
    CETYPE FILEopen (FILEOBJ fo, WORD *fHandle, char type)
  Summary:
    Loads file information from the device
  Conditions:
    This function should not be called by the user.
  Input:
    fo -       File to be opened
    fHandle -  Location of file
    type -
         -     FS_WRITE -  Create a new file or replace an existing file
         -     FS_READ -   Read data from an existing file
         -     FS_APPEND - Append data to an existing file
  Return Values:
    CE_GOOD -            FILEopen successful
    CE_NOT_INIT -        Device is not yet initialized
    CE_FILE_NOT_FOUND -  Could not find the file on the device
    CE_BAD_SECTOR_READ - A bad read of a sector occured
  Side Effects:
    None
  Description:
    This function will cache a directory entry in the directory specified
    by the dirclus parameter of hte FSFILE object 'fo.'  The offset of the
    entry in the directory is specified by fHandle.  Once the directory entry
    has been loaded, the first sector of the file can be loaded using the
    cluster value specified in the directory entry. The type argument will
    specify the mode the files will be opened in.  This will allow this
    function to set the correct read/write flags for the file.
  Remarks:
    If the mode the file is being opened in is a plus mode (e.g. FS_READ+) the
    flags will be modified further in the FSfopen function.
  **************************************************************************/
CETYPE FILEopen (FILEOBJ fo, WORD *fHandle, char type)
{
    DISK   *dsk;      //Disk structure
    BYTE    r;               //Result of search for file
    DWORD    l;               //lba of first sector of first cluster
    CETYPE    error = CE_GOOD;

    dsk = (DISK *)(fo->dsk);
    if (dsk->mount == FALSE)
    {
        error = CE_NOT_INIT;
    }
    else
    {
        // load the sector
        fo->dirccls = fo->dirclus;
        // Cache no matter what if it's the first entry
        if (*fHandle == 0)
        {
            if (Cache_File_Entry(fo, fHandle, TRUE) == NULL)
            {
                error = CE_BADCACHEREAD;
            }
        }
        else
        {
            // If it's not the first, only cache it if it's
            // not divisible by the number of entries per sector
            // If it is, Fill_File_Object will cache it
            if ((*fHandle & 0xf) != 0)
            {
                if (Cache_File_Entry (fo, fHandle, TRUE) == NULL)
                {
                    error = CE_BADCACHEREAD;
                }
            }
        }

        // Fill up the File Object with the information pointed to by fHandle
        r = Fill_File_Object(fo, fHandle);
        if (r != FOUND)
            error = CE_FILE_NOT_FOUND;
        else
        {
            fo->seek = 0;               // first byte in file
            fo->ccls = fo->cluster;     // first cluster
            fo->sec = 0;                // first sector in the cluster
            fo->pos = 0;                // first byte in sector/cluster

            if  ( r == NOT_FOUND)
            {
                error = CE_FILE_NOT_FOUND;
            }
            else
            {
                // Determine the lba of the selected sector and load
                l = Cluster2Sector(dsk,fo->ccls);

                gBufferOwner = fo;
                if (gLastDataSectorRead != l)
                {
                    gBufferZeroed = FALSE;
                    if ( !MDD_SDSPI_SectorRead( l, dsk->buffer))
                        error = CE_BAD_SECTOR_READ;
                    gLastDataSectorRead = l;
                }
            } // -- found

            fo->flags.FileWriteEOF = FALSE;
            // Set flag for operation type
            fo->flags.write = 0;   //read
            fo->flags.read = 1;
        } // -- r = Found
    } // -- Mounted
    return (error);
} // -- FILEopen

/*************************************************************************
  Function:
    BYTE FILEget_next_cluster(FSFILE *fo, DWORD n)
  Summary:
    Step through a chain of clusters
  Conditions:
    None
  Input:
    fo - The file to get the next cluster of
    n -  Number of links in the FAT cluster chain to jump through
  Return Values:
    CE_GOOD - Operation successful
    CE_BAD_SECTOR_READ - A bad read occured of a sector
    CE_INVALID_CLUSTER - Invalid cluster value \> maxcls
    CE_FAT_EOF - Fat attempt to read beyond EOF
  Side Effects:
    None
  Description:
    This function will load 'n' proximate clusters for a file from
    the FAT on the device.  It will stop checking for clusters if the
    ReadFAT function returns an error, if it reaches the last cluster in
    a file, or if the device tries to read beyond the last cluster used
    by the device.
  Remarks:
    None
  *************************************************************************/
BYTE FILEget_next_cluster(FSFILE *fo, DWORD n)
{
    DWORD         c, c2, ClusterFailValue, LastClustervalue;
    BYTE          error = CE_GOOD;
    DISK *      disk;

    disk = fo->dsk;

    /* Settings based on FAT type */
    switch (disk->type)
    {
#ifdef SUPPORT_FAT32 // If FAT32 supported.
        case FAT32:
            LastClustervalue = LAST_CLUSTER_FAT32;
            ClusterFailValue  = CLUSTER_FAIL_FAT32;
            break;
#endif
        case FAT12:
            LastClustervalue = LAST_CLUSTER_FAT12;
            ClusterFailValue  = CLUSTER_FAIL_FAT16;
            break;
        case FAT16:
        default:
            LastClustervalue = LAST_CLUSTER_FAT16;
            ClusterFailValue  = CLUSTER_FAIL_FAT16;
            break;
    }

    // loop n times
    do
    {
        // get the next cluster link from FAT
        c2 = fo->ccls;
        if ( (c = ReadFAT( disk, c2)) == ClusterFailValue)
            error = CE_BAD_SECTOR_READ;
        else
        {
            // check if cluster value is valid
            if ( c >= (disk->maxcls + 2))
            {
                error = CE_INVALID_CLUSTER;
            }

            // compare against max value of a cluster in FAT
            // return if eof
            if ( c >= LastClustervalue)    // check against eof
            {
                error = CE_FAT_EOF;
            }
        }

        // update the FSFILE structure
        fo->ccls = c;

    } while ((--n > 0) && (error == CE_GOOD));// loop end

    return(error);
} // get next cluster


/**************************************************************************
  Function:
    BYTE DISKmount ( DISK *dsk)
  Summary:
    Initialies the device and loads MBR and boot sector information
  Conditions:
    This function should not be called by the user.
  Input:
    dsk -  The disk structure to be initialized.
  Return Values:
    CE_GOOD -       Disk mounted
    CE_INIT_ERROR - Initialization error has occured
    CE_UNSUPPORTED_SECTOR_SIZE - Media sector size bigger than
                MEDIA_SECTOR_SIZE as defined in FSconfig.h.
  Side Effects:
    None
  Description:
    This function will use the function pointed to by the MDD_SDSPI_MediaInitialize
    function pointer to initialize the device (if any initialization is
    required).  It then attempts to load the master boot record with the
    LoadMBR function and the boot sector with the LoadBootSector function.
    These two functions will be used to initialize a global DISK structure
    that will be used when accessing file information in the future.
  Remarks:
    None
  **************************************************************************/
BYTE DISKmount( DISK *dsk)
{
    BYTE                error = CE_GOOD;
    MEDIA_INFORMATION   *mediaInformation;

    dsk->mount = FALSE; // default invalid
    dsk->buffer = gDataBuffer;    // assign buffer

    // Initialize the device
    mediaInformation = (MEDIA_INFORMATION *)MDD_SDSPI_MediaInitialize();
    if (mediaInformation->errorCode != MEDIA_NO_ERROR)
    {
        error = CE_INIT_ERROR;
        FSerrno = CE_INIT_ERROR;
    }
    else
    {
        // If the media initialization routine determined the sector size,
        // check it and make sure we can support it.
        if (mediaInformation->validityFlags.bits.sectorSize)
        {
			dsk->sectorSize = mediaInformation->sectorSize;
            if (mediaInformation->sectorSize > MEDIA_SECTOR_SIZE)
            {
                error = CE_UNSUPPORTED_SECTOR_SIZE;
                FSerrno = CE_UNSUPPORTED_SECTOR_SIZE;
                return error;
            }
        }

        // Load the Master Boot Record (partition)
        if((error = LoadMBR(dsk)) == CE_GOOD)
        {
            // Now the boot sector
            if((error = LoadBootSector(dsk)) == CE_GOOD)
                dsk->mount = TRUE; // Mark that the DISK mounted successfully
        }
    } // -- Load file parameters

    return(error);
} // -- mount

/********************************************************************
  Function:
    CETYPE LoadMBR ( DISK *dsk)
  Summary:
    Loads the MBR and extracts necessary information
  Conditions:
    This function should not be called by the user.
  Input:
    dsk -  The disk containing the master boot record to be loaded
  Return Values:
    CE_GOOD -            MBR loaded successfully
    CE_BAD_SECTOR_READ - A bad read occured of a sector
    CE_BAD_PARTITION -   The boot record is bad
  Side Effects:
    None
  Description:
    The LoadMBR function will use the function pointed to by the
    MDD_SDSPI_SectorRead function pointer to read the 0 sector from the
    device.  If a valid boot signature is obtained, this function
    will compare fields in that cached sector to the values that
    would be present if that sector was a boot sector.  If all of
    those values match, it will be assumed that the device does not
    have a master boot record and the 0 sector is actually the boot
    sector.  Otherwise, data about the partition and the actual
    location of the boot sector will be loaded from the MBR into
    the DISK structure pointed to by 'dsk.'
  Remarks:
    None
  ********************************************************************/
BYTE LoadMBR(DISK *dsk)
{
    PT_MBR  Partition;
    BYTE error = CE_GOOD;
    BYTE type;
    BootSec BSec;

    // Get the partition table from the MBR
    if ( MDD_SDSPI_SectorRead( FO_MBR, dsk->buffer) != TRUE)
    {
        error = CE_BAD_SECTOR_READ;
        FSerrno = CE_BAD_SECTOR_READ;
    }
    else
    {
        // Check if the card has no MBR
        BSec = (BootSec) dsk->buffer;

        if((BSec->Signature0 == FAT_GOOD_SIGN_0) && (BSec->Signature1 == FAT_GOOD_SIGN_1))
        {
         // Technically, the OEM name is not for indication
         // The alternative is to read the CIS from attribute
         // memory.  See the PCMCIA metaformat for more details
            if ((ReadByte( dsk->buffer, BSI_FSTYPE ) == 'F') && \
            (ReadByte( dsk->buffer, BSI_FSTYPE + 1 ) == 'A') && \
            (ReadByte( dsk->buffer, BSI_FSTYPE + 2 ) == 'T') && \
            (ReadByte( dsk->buffer, BSI_FSTYPE + 3 ) == '1') && \
            (ReadByte( dsk->buffer, BSI_BOOTSIG) == 0x29))
             {
                dsk->firsts = 0;
                dsk->type = FAT16;
                return CE_GOOD;
             }
             else
             {
                if ((ReadByte( dsk->buffer, BSI_FAT32_FSTYPE ) == 'F') && \
                    (ReadByte( dsk->buffer, BSI_FAT32_FSTYPE + 1 ) == 'A') && \
                    (ReadByte( dsk->buffer, BSI_FAT32_FSTYPE + 2 ) == 'T') && \
                    (ReadByte( dsk->buffer, BSI_FAT32_FSTYPE + 3 ) == '3') && \
                    (ReadByte( dsk->buffer, BSI_FAT32_BOOTSIG) == 0x29))
                {
                    dsk->firsts = 0;
                    dsk->type = FAT32;
                    return CE_GOOD;
                }
            }
        }
        // assign it the partition table strucutre
        Partition = (PT_MBR)dsk->buffer;

        // Ensure its good
        if((Partition->Signature0 != FAT_GOOD_SIGN_0) || (Partition->Signature1 != FAT_GOOD_SIGN_1))
        {
            FSerrno = CE_BAD_PARTITION;
            error = CE_BAD_PARTITION;
        }
        else
        {
            BYTE i;
            PTE_MBR* partitionEntry = &Partition->Partition0;

            for(i=0; i<4; i++)
            {                
                /*    Valid Master Boot Record Loaded   */

                // Get the 32 bit offset to the first partition
                dsk->firsts = partitionEntry->PTE_FrstSect;

                // check if the partition type is acceptable
                  type = partitionEntry->PTE_FSDesc;

                switch (type)
                {
                    case 0x01:
                        dsk->type = FAT12;
                        break;

                case 0x04:
                    case 0x06:
                    case 0x0E:
                        dsk->type = FAT16;
                        return(error);

                    case 0x0B:
                    case 0x0C:
                        #ifdef SUPPORT_FAT32 // If FAT32 supported.
                            dsk->type = FAT32;    // FAT32 is supported too
                            return(error);
                        #endif
                } // switch

                /* If we are here, we didn't find a matching partition.  We
                   should increment to the next partition table entry */
                partitionEntry++;
            }

            FSerrno = CE_UNSUPPORTED_FS;
            error = CE_UNSUPPORTED_FS;
        }
    }

    return(error);
}// -- LoadMBR


/**************************************************************************
  Function:
    BYTE LoadBootSector (DISK *dsk)
  Summary:
    Load the boot sector and extract the necessary information
  Conditions:
    This function should not be called by the user.
  Input:
    dsk -  The disk containing the boot sector
  Return Values:
    CE_GOOD -                    Boot sector loaded
    CE_BAD_SECTOR_READ -         A bad read occured of a sector
    CE_NOT_FORMATTED -           The disk is of an unsupported format
    CE_CARDFAT32 -               FAT 32 device not supported
    CE_UNSUPPORTED_SECTOR_SIZE - The sector size is not supported
  Side Effects:
    None
  Description:
    LoadBootSector will use the function pointed to by the MDD_SDSPI_SectorWrite
    function pointer to load the boot sector, whose location was obtained
    by a previous call of LoadMBR.  If the boot sector is loaded successfully,
    partition information will be calcualted from it and copied into the DISK
    structure pointed to by 'dsk.'
  Remarks:
    None
  **************************************************************************/
BYTE LoadBootSector(DISK *dsk)
{
    DWORD       RootDirSectors;
    DWORD       TotSec,DataSec;
    BYTE        error = CE_GOOD;
    BootSec     BSec;
    WORD        BytesPerSec;
    WORD        ReservedSectorCount;

    #if defined(SUPPORT_FAT32)
    BOOL        TriedSpecifiedBackupBootSec = FALSE;
    BOOL        TriedBackupBootSecAtAddress6 = FALSE;
    #endif
    // Get the Boot sector
    if ( MDD_SDSPI_SectorRead( dsk->firsts, dsk->buffer) != TRUE)
    {
        error = CE_BAD_SECTOR_READ;
    }
    else
    {
        BSec = (BootSec)dsk->buffer;

        do      //test each possible boot sector (FAT32 can have backup boot sectors)
        {

            //Verify the Boot Sector has a valid signature
            if(    (BSec->Signature0 != FAT_GOOD_SIGN_0)
                || (BSec->Signature1 != FAT_GOOD_SIGN_1)
              )
            {
                error = CE_NOT_FORMATTED;
            }
            else   
            {

                do      //loop just to allow a break to jump out of this section of code
                {
                    // Read the count of reserved sectors
                    ReservedSectorCount = ReadWord( dsk->buffer, BSI_RESRVSEC );
                    // Load the count of sectors per cluster
                    dsk->SecPerClus = ReadByte( dsk->buffer, BSI_SPC );
                    // Load the sector number of the first FAT sector
                    dsk->fat = dsk->firsts + ReservedSectorCount;
                    // Load the count of FAT tables
                    dsk->fatcopy    = ReadByte( dsk->buffer, BSI_FATCOUNT );
                    // Load the size of the FATs
                    dsk->fatsize = ReadWord( dsk->buffer, BSI_SPF );
                    if(dsk->fatsize == 0)
                        dsk->fatsize  = ReadDWord( dsk->buffer, BSI_FATSZ32 );
                    // Calculate the location of the root sector (for FAT12/16)
                    dsk->root = dsk->fat + (DWORD)(dsk->fatcopy * (DWORD)dsk->fatsize);
                    // Determine the max size of the root (will be 0 for FAT32)
                    dsk->maxroot = ReadWord( dsk->buffer, BSI_ROOTDIRENTS );
    
                    // Determine the total number of sectors in the partition
                    TotSec = ReadWord( dsk->buffer, BSI_TOTSEC16 );
                    if( TotSec == 0 )
                        TotSec = ReadDWord( dsk->buffer, BSI_TOTSEC32 );
    
                    // Calculate the number of bytes in each sector
                    BytesPerSec = ReadWord( dsk->buffer, BSI_BPS );
                    if( (BytesPerSec == 0) || ((BytesPerSec & 1) == 1) )
                    {
                        error = CE_UNSUPPORTED_SECTOR_SIZE;
                        break;
                    }
    
                    // Calculate the number of sectors in the root (will be 0 for FAT32)
                    RootDirSectors = ((dsk->maxroot * NUMBER_OF_BYTES_IN_DIR_ENTRY) + (BytesPerSec - 1)) / BytesPerSec;
                    // Calculate the number of data sectors on the card
                    DataSec = TotSec - (ReservedSectorCount + (dsk->fatcopy * dsk->fatsize )  + RootDirSectors);
                    // Calculate the maximum number of clusters on the card
                    dsk->maxcls = DataSec / dsk->SecPerClus;
    
                    // Determine the file system type based on the number of clusters used
                    if(dsk->maxcls < 4085)
                    {
                        dsk->type = FAT12;
                    }
                    else
                    {
                        if(dsk->maxcls < 65525)
                        {
                            dsk->type = FAT16;
                        }
                        else
                        {
                            #ifdef SUPPORT_FAT32
                                dsk->type = FAT32;
                            #else
                                error = CE_CARDFAT32;
                            #endif
                        }
                    }
        
                    #ifdef SUPPORT_FAT32
                        if (dsk->type == FAT32)
                        {
							FatRootDirClusterValue = ReadDWord( dsk->buffer, BSI_ROOTCLUS );
                            dsk->data = dsk->root + RootDirSectors;
                        }
                        else
                    #endif
                    {
                        FatRootDirClusterValue = 0;
                        dsk->data = dsk->root + ( dsk->maxroot >> 4);
                    }
    

                    if(BytesPerSec > MEDIA_SECTOR_SIZE)
                    {
                        error = CE_UNSUPPORTED_SECTOR_SIZE;
                    }

                }while(0);  // do/while loop designed to allow to break out if
                            //   there is an error detected without returning
                            //   from the function.

            }

            #if defined(SUPPORT_FAT32)
            if ((dsk->type == FAT32) || ((error != CE_GOOD) && ((BSec->FAT.FAT_32.BootSec_BootSig == 0x29) || (BSec->FAT.FAT_32.BootSec_BootSig == 0x28))))
            {
                //Check for possible errors in the formatting
                if(    (BSec->FAT.FAT_32.BootSec_TotSec16 != 0)
                    || (BSec->FAT.FAT_32.BootSec_Reserved[0] != 0)
                    || (BSec->FAT.FAT_32.BootSec_Reserved[1] != 0)
                    || (BSec->FAT.FAT_32.BootSec_Reserved[2] != 0)
                    || (BSec->FAT.FAT_32.BootSec_Reserved[3] != 0)
                    || (BSec->FAT.FAT_32.BootSec_Reserved[4] != 0)
                    || (BSec->FAT.FAT_32.BootSec_Reserved[5] != 0)
                    || (BSec->FAT.FAT_32.BootSec_Reserved[6] != 0)
                    || (BSec->FAT.FAT_32.BootSec_Reserved[7] != 0)
                    || (BSec->FAT.FAT_32.BootSec_Reserved[8] != 0)
                    || (BSec->FAT.FAT_32.BootSec_Reserved[9] != 0)
                    || (BSec->FAT.FAT_32.BootSec_Reserved[10] != 0)
                    || (BSec->FAT.FAT_32.BootSec_Reserved[11] != 0)
                    || ((BSec->FAT.FAT_32.BootSec_BootSig != 0x29) && (BSec->FAT.FAT_32.BootSec_BootSig != 0x28))
                  )
                {
                    error = CE_NOT_FORMATTED;
                }
    
                //If there were formatting errors then in FAT32 we can try to use
                //  the backup boot sector
                if((error != CE_GOOD) && (TriedSpecifiedBackupBootSec == FALSE))
                {
                    TriedSpecifiedBackupBootSec = TRUE;
    
                    if ( MDD_SDSPI_SectorRead( dsk->firsts + BSec->FAT.FAT_32.BootSec_BkBootSec, dsk->buffer) != TRUE)
                    {
                        FSerrno = CE_BAD_SECTOR_READ;
                        return CE_BAD_SECTOR_READ;
                    }
                    else
                    {
                        error = CE_GOOD;
                        continue;
                    }
                }
    
                if((error != CE_GOOD) && (TriedBackupBootSecAtAddress6 == FALSE))
                {
                    TriedBackupBootSecAtAddress6 = TRUE;
    
                    //Here we are using the magic number 6 because the FAT32 specification
                    //  recommends that "No value other than 6 is recommended."  We've
                    //  already tried using the value specified in the BPB_BkBootSec
                    //  field and it must have failed
                    if ( MDD_SDSPI_SectorRead( dsk->firsts + 6, dsk->buffer) != TRUE)
                    {
                        FSerrno = CE_BAD_SECTOR_READ;
                        return CE_BAD_SECTOR_READ;
                    }
                    else
                    {
                        error = CE_GOOD;
                        continue;
                    }
                }

            }   //type == FAT32
            #endif  //SUPPORT_FAT32
            break;
        }
        while(1);
    }

    if(error != CE_GOOD)
    {
        FSerrno = error;
    }

    return(error);
}

/*************************************************************************
  Function:
    DWORD GetFullClusterNumber (DIRENTRY entry)
  Summary:
    Gets the cluster number from a directory entry
  Conditions:
    This function should not be called by the user.
  Input:
    entry - The cached directory entry to get the cluster number from
  Returns:
    The cluster value from the passed directory entry
  Side Effects:
    None.
  Description:
    This function will load both the high and low 16-bit first cluster
    values of a file from a directory entry and copy them into a 32-bit
    cluster number variable, which will be returned.
  Remarks:
    None
  *************************************************************************/
DWORD GetFullClusterNumber(DIRENTRY entry)
{
    DWORD TempFullClusterCalc = 0;

#ifndef SUPPORT_FAT32 // If FAT32 Not supported.
    entry->DIR_FstClusHI = 0; // If FAT32 is not supported then Higher Word of the address is "0"
#endif

    // Get the cluster
    TempFullClusterCalc = (entry->DIR_FstClusHI);
    TempFullClusterCalc = TempFullClusterCalc << 16;
    TempFullClusterCalc |= entry->DIR_FstClusLO;

    return TempFullClusterCalc;
}

/**************************************************************************
  Function:
    DIRENTRY Cache_File_Entry( FILEOBJ fo, WORD * curEntry, BYTE ForceRead)
  Summary:
    Load a file entry
  Conditions:
    This function should not be called by the user.
  Input:
    fo -         File information
    curEntry -   Offset of the directory entry to load.
    ForceRead -  Forces loading of a new sector of the directory.
  Return:
    DIRENTRY - Pointer to the directory entry that was loaded.
  Side Effects:
    Any unwritten data in the data buffer will be written to the device.
  Description:
    Load the sector containing the file entry pointed to by 'curEntry'
    from the directory pointed to by the variables in 'fo.'
  Remarks:
    Any modification of this function is extremely likely to
    break something.
  **************************************************************************/
DIRENTRY Cache_File_Entry( FILEOBJ fo, WORD * curEntry, BYTE ForceRead)
{
    DIRENTRY dir;
    DISK *dsk;
    DWORD sector;
    DWORD cluster, LastClusterLimit;
    DWORD ccls;
    BYTE offset2;
    BYTE numofclus;
	BYTE dirEntriesPerSector;

    dsk = fo->dsk;

    // get the base sector of this directory
    cluster = fo->dirclus;
    ccls = fo->dirccls;

	dirEntriesPerSector = dsk->sectorSize/32;

     // figure out the offset from the base sector
    offset2  = (*curEntry / dirEntriesPerSector);

    /* Settings based on FAT type */
    switch (dsk->type)
    {
#ifdef SUPPORT_FAT32 // If FAT32 supported.
        case FAT32:
            // the ROOT is always cluster based in FAT32
            /* In FAT32: There is no ROOT region. Root etries are made in DATA region only.
            Every cluster of DATA which is accupied by ROOT is tracked by FAT table/entry so the ROOT can grow
            to an amount which is restricted only by available free DATA region. */
            offset2  = offset2 % (dsk->SecPerClus);   // figure out the offset
            LastClusterLimit = LAST_CLUSTER_FAT32;
            break;
#endif
        case FAT12:
        case FAT16:
        default:
            // if its the root its not cluster based
            if(cluster != 0)
                offset2  = offset2 % (dsk->SecPerClus);   // figure out the offset
            LastClusterLimit = LAST_CLUSTER_FAT16;
            break;
    }

    // check if a new sector of the root must be loaded
    if (ForceRead || ((*curEntry & MASK_MAX_FILE_ENTRY_LIMIT_BITS) == 0))     // only 16 entries per sector
    {
        // see if we have to load a new cluster
        if(((offset2 == 0) && (*curEntry >= dirEntriesPerSector)) || ForceRead)
        {
            if(cluster == 0)
            {
                ccls = 0;
            }
            else
            {
                // If ForceRead, read the number of sectors from 0
                if(ForceRead)
                    numofclus = ((WORD)(*curEntry) / (WORD)(((WORD)dirEntriesPerSector) * (WORD)dsk->SecPerClus));
                // Otherwise just read the next sector
                else
                    numofclus = 1;

                // move to the correct cluster
                while(numofclus)
                {
                    ccls = ReadFAT(dsk, ccls);

                    if(ccls >= LastClusterLimit)
                        break;
                    else
                        numofclus--;
                }
            }
        }

        // see if that we have a valid cluster number
        if(ccls < LastClusterLimit)
        {
            fo->dirccls = ccls; // write it back

            sector = Cluster2Sector(dsk,ccls);

            /* see if we are root and about to go pass our boundaries
            FAT32 stores the root directory in the Data Region along with files and other directories,
            allowing it to grow without such a restraint */
            if((ccls == FatRootDirClusterValue) && ((sector + offset2) >= dsk->data) && (FAT32 != dsk->type))
            {
                dir = ((DIRENTRY)NULL);   // reached the end of the root
            }
            else
            {
                gBufferOwner = NULL;
                gBufferZeroed = FALSE;

                if ( MDD_SDSPI_SectorRead( sector + offset2, dsk->buffer) != TRUE) // if FALSE: sector could not be read.
                {
                    dir = ((DIRENTRY)NULL);
                }
                else // Sector has been read properly, Copy the root entry info of the file searched.
                {
                    if(ForceRead)    // Buffer holds all 16 root entry info. Point to the one required.
                        dir = (DIRENTRY)((DIRENTRY)dsk->buffer) + ((*curEntry)%dirEntriesPerSector);
                    else
                        dir = (DIRENTRY)dsk->buffer;
                }
                gLastDataSectorRead = 0xFFFFFFFF;
            }
        }
        else
        {
            nextClusterIsLast = TRUE;
            dir = ((DIRENTRY)NULL);
        }
    }
    else
        dir = (DIRENTRY)((DIRENTRY)dsk->buffer) + ((*curEntry)%dirEntriesPerSector);

    return(dir);
} // Cache_File_Entry

/************************************************************
  Function:
    int FSfclose(FSFILE *fo)
  Summary:
    Update file information and free FSFILE objects
  Conditions:
    File opened
  Input:
    fo -  Pointer to the file to close
  Return Values:
    0 -   File closed successfully
    EOF - Error closing the file
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    This function will update the directory entry for the
    file pointed to by 'fo' with the information contained
    in 'fo,' including the new file size and attributes.
    Timestamp information will also be loaded based on the
    method selected by the user and written to the entry
    as the last modified time and date.  The file entry will
    then be written to the device.  Finally, the memory
    used for the specified file object will be freed from
    the dynamic heap or the array of FSFILE objects.
  Remarks:
    A function to flush data to the device without closing the
    file can be created by removing the portion of this
    function that frees the memory and the line that clears
    the write flag.
  ************************************************************/
int FSfclose(FSFILE   *fo)
{
    WORD        fIndex;
    WORD        fHandle;
    int        error = 72;

    FSerrno = CE_GOOD;
    fHandle = fo->entry;

    // Clear the read acess to file
    fo->flags.read = FALSE;
    for( fIndex = 0; fIndex < FS_MAX_FILES_OPEN; fIndex++ )
    {
        if( fo == &gFileArray[fIndex] )
        {
            gFileSlotOpen[fIndex] = TRUE;
            break;
        }
    }

	// Set Null pointer to close the file, to prevent inadvertent acess
    fo = NULL;

    // File opened in read mode
    if (error == 72)
        error = 0;

    return(error);
} // FSfclose

/*****************************************************************
  Function:
    BYTE Fill_File_Object(FILEOBJ fo, WORD *fHandle)
  Summary:
    Fill a file object with specified dir entry data
  Conditions:
    This function should not be called by the user.
  Input:
    fo -       Pointer to file structure
    fHandle -  Passed member's location
  Return Values:
    FOUND -     Operation successful
    NOT_FOUND - Operation failed
  Side Effects:
    None
  Description:
    This function will cache the sector of directory entries
    in the directory pointed to by the dirclus value in
    the FSFILE object 'fo' that contains the entry that
    corresponds to the fHandle offset.  It will then copy
    the file information for that entry into the 'fo' FSFILE
    object.
  Remarks:
    None.
  *****************************************************************/
BYTE Fill_File_Object(FILEOBJ fo, WORD *fHandle)
{
    DIRENTRY    dir;
    BYTE        index, a;
    BYTE        character;
    BYTE        status;
    BYTE        test = 0;

    // Get the entry
    if (((*fHandle & MASK_MAX_FILE_ENTRY_LIMIT_BITS) == 0) && (*fHandle != 0)) // 4-bit mask because 16-root entries max per sector
    {
        fo->dirccls = fo->dirclus;
        dir = Cache_File_Entry(fo, fHandle, TRUE);
    }
    else
    {
        dir = Cache_File_Entry (fo, fHandle, FALSE);
    }


    // Make sure there is a directory left
    if(dir == (DIRENTRY)NULL)
    {
        status = NO_MORE;
    }
    else
    {
        // Read the first char of the file name
        a = dir->DIR_Name[0];

        // Check for empty or deleted directory
        if ( a == DIR_DEL)
		{
            status = NOT_FOUND;
		}
		else if ( a == DIR_EMPTY)
		{
			status = NO_MORE;
		}
        else
        {
            // Get the attributes
            a = dir->DIR_Attr;

            // print the file name and extension
            for (index=0; index < DIR_NAMESIZE; index++)
            {
                character = dir->DIR_Name[index];
                character = (BYTE)toupper(character);
                fo->name[test++] = character;
            }

            // Get the attributes
            a = dir->DIR_Attr;

            // its possible to have an extension in a directory
            character = dir->DIR_Extension[0];

            // Get the file extension if its there
            for (index=0; index < DIR_EXTENSION; index++)
            {
                character = dir->DIR_Extension[index];
                character = (BYTE)toupper(character);
                fo->name[test++] = character;
            }

            // done and done with the name
            //         fo->name[++test] = (BYTE)'\0';

            // Now store the identifier
            fo->entry = *fHandle;

            // see if we are still a good file
            a = dir->DIR_Name[0];

            if(a == DIR_DEL)
                status = NOT_FOUND;
            else
                status = FOUND;

            // Now store the size
            fo->size = (dir->DIR_FileSize);

            fo->cluster = GetFullClusterNumber(dir); // Get Complete Cluster number.

            /// -Get and store the attributes
            a = dir->DIR_Attr;
            fo->attributes = a;

            // get the date and time
            if ((a & ATTR_DIRECTORY) != 0)
            {
                fo->time = dir->DIR_CrtTime;
                fo->date = dir->DIR_CrtDate;
            }
            else
            {
                fo->time = dir->DIR_WrtTime;
                fo->date = dir->DIR_WrtDate;
            }

        }// deleted directory
    }// Ensure we are still good
    return(status);
} // Fill_File_Object

/************************************************************************
  Function:
    DIRENTRY LoadDirAttrib(FILEOBJ fo, WORD *fHandle)
  Summary:
    Load file information from a directory entry and cache the entry
  Conditions:
    This function should not be called by the user.
  Input:
    fo -       Pointer to file structure
    fHandle -  Information location
  Return Values:
    DIRENTRY - Pointer to the directory entry
    NULL -     Directory entry could not be loaded
  Side Effects:
    None
  Description:
    This function will cache the sector of directory entries
    in the directory pointed to by the dirclus value in
    the FSFILE object 'fo' that contains the entry that
    corresponds to the fHandle offset.  It will then return a pointer
    to the directory entry in the global data buffer.
  Remarks:
    None.
  ************************************************************************/
DIRENTRY LoadDirAttrib(FILEOBJ fo, WORD *fHandle)
{
    DIRENTRY    dir;
    BYTE      a;

    fo->dirccls = fo->dirclus;
    // Get the entry
    dir = Cache_File_Entry( fo, fHandle, TRUE);
    if (dir == NULL)
        return NULL;

    // Read the first char of the file name
    a = dir->DIR_Name[0];

    // Make sure there is a directory left
    if(a == DIR_EMPTY)
        dir = (DIRENTRY)NULL;

    if(dir != (DIRENTRY)NULL)
    {
        // Check for empty or deleted directory
        if ( a == DIR_DEL)
            dir = (DIRENTRY)NULL;
        else
        {
            // Get the attributes
            a = dir->DIR_Attr;

            // scan through all the long dir entries
            while(a == ATTR_LONG_NAME)
            {
                (*fHandle)++;
                dir = Cache_File_Entry( fo, fHandle, FALSE);
                if (dir == NULL)
                    return NULL;
                a = dir->DIR_Attr;
            } // long file name while loop
        } // deleted dir
    }// Ensure we are still good

    return(dir);
} // LoadDirAttrib

/*********************************************************************
  Function:
    FSFILE * FSfopen (const char * fileName, const char *mode)
  Summary:
    Opens a file with ascii input 'fileName' on PIC24/PIC32/dsPIC MCU's.
  Conditions:
    For read modes, file exists; FSInit performed
  Input:
    fileName -  The name of the file to open
    mode -
         - FS_WRITE -      Create a new file or replace an existing file
         - FS_READ -       Read data from an existing file
         - FS_APPEND -     Append data to an existing file
         - FS_WRITEPLUS -  Create a new file or replace an existing file (reads also enabled)
         - FS_READPLUS -   Read data from an existing file (writes also enabled)
         - FS_APPENDPLUS - Append data to an existing file (reads also enabled)
  Return Values:
    FSFILE * - The pointer to the file object
    NULL -     The file could not be opened
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    This function will open a ascii name file or directory on PIC24/PIC32/dsPIC MCU's.
    First, RAM in the dynamic heap or static array will be allocated to a
    new FSFILE object. Then, the specified file name will be formatted to
    ensure that it's in 8.3 format or LFN format. Next, the FILEfind function
    will be used to search for the specified file name. If the name is found,
    one of three things will happen: if the file was opened in read mode, its
    file info will be loaded using the FILEopen function; if it was opened in
    write mode, it will be erased, and a new file will be constructed in
    its place; if it was opened in append mode, its file info will be
    loaded with FILEopen and the current location will be moved to the
    end of the file using the FSfseek function.  If the file was not
    found by FILEfind, a new file will be created if the mode was specified as
    a write or append mode.  In these cases, a pointer to the heap or
    static FSFILE object array will be returned. If the file was not
    found and the mode was specified as a read mode, the memory
    allocated to the file will be freed and the NULL pointer value
    will be returned.
  Remarks:
    None.
  *********************************************************************/
FSFILE * FSfopen( const char * fileName, const char *mode )
{
    FILEOBJ     filePtr;
    BYTE        ModeC;
    WORD        fHandle;
    WORD        fIndex;
    CETYPE      final;

    //Read the mode character
    ModeC = mode[0];

    if(MDD_SDSPI_WriteProtectState() && (ModeC != 'r') && (ModeC != 'R')) 
    { 
        FSerrno = CE_WRITE_PROTECTED; 
        return NULL; 
    } 

    filePtr = NULL;

    //Pick available file structure
    for( fIndex = 0; fIndex < FS_MAX_FILES_OPEN; fIndex++ )
    {
        if( gFileSlotOpen[fIndex] )   //this slot is available
        {
            gFileSlotOpen[fIndex] = FALSE;
            filePtr = &gFileArray[fIndex];
			break;
        }
    }

    if( filePtr == NULL )
    {
        FSerrno = CE_TOO_MANY_FILES_OPEN;
        return NULL;      //no file structure slot available
    }

    //Format the source string.
    if( !FormatFileName(fileName, filePtr, 0) )
    {
		gFileSlotOpen[fIndex] = TRUE;   //put this slot back to the pool
        
		FSerrno = CE_INVALID_FILENAME;
        return NULL;   //bad filename
    }

    filePtr->dsk = &gDiskData;
    filePtr->cluster = 0;
    filePtr->ccls    = 0;
    filePtr->entry = 0;
    filePtr->attributes = ATTR_ARCHIVE;

    // start at the current directory
    filePtr->dirclus = FatRootDirClusterValue;
    filePtr->dirccls = FatRootDirClusterValue;

    // copy file object over
    FileObjectCopy(&gFileTemp, filePtr);

    // See if the file is found
    if(FILEfind (filePtr, &gFileTemp, LOOK_FOR_MATCHING_ENTRY, 0) == CE_GOOD)
    {
        // File is Found
        switch(ModeC)
        {
            case 'R':
            case 'r':
            {
                fHandle = filePtr->entry;
                final = FILEopen (filePtr, &fHandle, 'r');
                break;
            }
            default:
                FSerrno = CE_INVALID_ARGUMENT;
                final = 0xFF;;  //indicate error condition
                break;
        }
    }
    else
    {
        final = CE_FILE_NOT_FOUND;
        FSerrno = CE_FILE_NOT_FOUND;
    }

    if (MDD_SDSPI_WriteProtectState())
    {
        filePtr->flags.write = 0;;
    }

    if( final != CE_GOOD )
    {
        gFileSlotOpen[fIndex] = TRUE;   //put this slot back to the pool
        filePtr = NULL;
    }
    else
    {
        FSerrno = CE_GOOD;
    }

    return filePtr;
}

/*******************************************************************
  Function:
    long FSftell (FSFILE * fo)
  Summary:
    Determine the current location in a file
  Conditions:
    File opened
  Input:
    fo -  Pointer to file structure
  Return: Current location in the file
  Side Effects:
    The FSerrno variable will be changed
  Description:
    The FSftell function will return the current position in the
    file pointed to by 'fo' by returning the 'seek' variable in the
    FSFILE object, which is used to keep track of the absolute
    location of the current position in the file.
  Remarks:
    None
  *******************************************************************/
long FSftell (FSFILE * fo)
{
    FSerrno = CE_GOOD;
    return (fo->seek);
}

/*********************************************************
  Function:
    void FSrewind (FSFILE * fo)
  Summary:
    Set the current position in a file to the beginning
  Conditions:
    File opened.
  Input:
    fo -  Pointer to file structure
  Return Values:
    None
  Side Effects:
    None.
  Description:
    The FSrewind funciton will reset the position of the
    specified file to the beginning of the file.  This
    functionality is faster than using FSfseek to reset
    the position in the file.
  Remarks:
    None.
  *********************************************************/
void FSrewind (FSFILE * fo)
{
    fo->seek = 0;
    fo->pos = 0;
    fo->sec = 0;
    fo->ccls = fo->cluster;
    gBufferOwner = NULL;
    return;
}

/**************************************************************************
  Function:
    int FSerror (void)
  Summary:
    Return an error code for the last function call
  Conditions:
    The return value depends on the last function called.
  Input:
    None
  Side Effects:
    None.
  Return Values:
    FSInit       -
                 - CE_GOOD �                  No Error
                 - CE_INIT_ERROR �            The physical media could not be initialized
                 - CE_BAD_SECTOR_READ �       The MBR or the boot sector could not be
                                              read correctly
                 - CE_BAD_PARITION �          The MBR signature code was incorrect.
                 - CE_NOT_FORMATTED �         The boot sector signature code was incorrect or
                                              indicates an invalid number of bytes per sector.
                 - CE_UNSUPPORTED_SECTOR_SIZE - The number of bytes per sector is unsupported
                 - CE_CARDFAT32 �             The physical media is FAT32 type (only an error
                                              when FAT32 support is disabled).
                 - CE_UNSUPPORTED_FS �        The device is formatted with an unsupported file
                                              system (not FAT12 or 16).
    FSfopen      -
                 - CE_GOOD �                  No Error
                 - CE_NOT_INIT �              The device has not been initialized.
                 - CE_TOO_MANY_FILES_OPEN �   The function could not allocate any
                                              additional file information to the array
                                              of FSFILE structures or the heap.
                 - CE_INVALID_FILENAME �      The file name argument was invalid.
                 - CE_INVALID_ARGUMENT �      The user attempted to open a directory in a
                                              write mode or specified an invalid mode argument.
                 - CE_FILE_NOT_FOUND �        The specified file (which was to be opened in read
                                              mode) does not exist on the device.
                 - CE_BADCACHEREAD �          A read from the device failed.
                 - CE_ERASE_FAIL �            The existing file could not be erased (when opening
                                              a file in FS_WRITE mode).
                 - CE_DIR_FULL �              The directory is full.
                 - CE_DISK_FULL�              The data memory section is full.
                 - CE_WRITE_ERROR �           A write to the device failed.
                 - CE_SEEK_ERROR �            The current position in the file could not be set to
                                              the end (when the file was opened in FS_APPEND mode).
    FSfclose     -
                 - CE_GOOD �                  No Error
                 - CE_WRITE_ERROR �           The existing data in the data buffer or the new file
                                              entry information could not be written to the device.
                 - CE_BADCACHEREAD �          The file entry information could not be cached
    FSfread      -
                 - CE_GOOD �                  No Error
                 - CE_WRITEONLY �             The file was opened in a write-only mode.
                 - CE_WRITE_ERROR �           The existing data in the data buffer could not be
                                              written to the device.
                 - CE_BAD_SECTOR_READ �       The data sector could not be read.
                 - CE_EOF �                   The end of the file was reached.
                 - CE_COULD_NOT_GET_CLUSTER � Additional clusters in the file could not be loaded.
    FSfwrite     -
                 - CE_GOOD �                  No Error
                 - CE_READONLY �              The file was opened in a read-only mode.
                 - CE_WRITE_PROTECTED �       The device write-protect check function indicated
                                              that the device has been write-protected.
                 - CE_WRITE_ERROR �           There was an error writing data to the device.
                 - CE_BADCACHEREAD �          The data sector to be modified could not be read from
                                              the device.
                 - CE_DISK_FULL �             All data clusters on the device are in use.
    FSfseek      -
                 - CE_GOOD �                  No Error
                 - CE_WRITE_ERROR �           The existing data in the data buffer could not be
                                              written to the device.
                 - CE_INVALID_ARGUMENT �      The specified offset exceeds the size of the file.
                 - CE_BADCACHEREAD �          The sector that contains the new current position
                                              could not be loaded.
                 - CE_COULD_NOT_GET_CLUSTER � Additional clusters in the file could not be
                                              loaded/allocated.
    FSftell      -
                 - CE_GOOD �                  No Error
    FSattrib     -
                 - CE_GOOD �                  No Error
                 - CE_INVALID_ARGUMENT �      The attribute argument was invalid.
                 - CE_BADCACHEREAD �          The existing file entry information could not be
                                              loaded.
                 - CE_WRITE_ERROR �           The file entry information could not be written to
                                              the device.
    FSrename     -
                 - CE_GOOD �                  No Error
                 - CE_FILENOTOPENED �         A null file pointer was passed into the function.
                 - CE_INVALID_FILENAME �      The file name passed into the function was invalid.
                 - CE_BADCACHEREAD �          A read from the device failed.
                 - CE_FILENAME_EXISTS �       A file with the specified name already exists.
                 - CE_WRITE_ERROR �           The new file entry data could not be written to the
                                              device.
    FSfeof       -
                 - CE_GOOD �                  No Error
    FSformat     -
                 - CE_GOOD �                  No Error
                 - CE_INIT_ERROR �            The device could not be initialized.
                 - CE_BADCACHEREAD �          The master boot record or boot sector could not be
                                              loaded successfully.
                 - CE_INVALID_ARGUMENT �      The user selected to create their own boot sector on
                                              a device that has no master boot record, or the mode
                                              argument was invalid.
                 - CE_WRITE_ERROR �           The updated MBR/Boot sector could not be written to
                                              the device.
                 - CE_BAD_PARTITION �         The calculated number of sectors per clusters was
                                              invalid.
                 - CE_NONSUPPORTED_SIZE �     The card has too many sectors to be formatted as
                                              FAT12 or FAT16.
    FSremove     -
                 - CE_GOOD �                  No Error
                 - CE_WRITE_PROTECTED �       The device write-protect check function indicated
                                              that the device has been write-protected.
                 - CE_INVALID_FILENAME �      The specified filename was invalid.
                 - CE_FILE_NOT_FOUND �        The specified file could not be found.
                 - CE_ERASE_FAIL �            The file could not be erased.
    FSchdir      -
                 - CE_GOOD �                  No Error
                 - CE_INVALID_ARGUMENT �      The path string was mis-formed or the user tried to
                                              change to a non-directory file.
                 - CE_BADCACHEREAD �          A directory entry could not be cached.
                 - CE_DIR_NOT_FOUND �         Could not find a directory in the path.
    FSgetcwd     -
                 - CE_GOOD �                  No Error
                 - CE_INVALID_ARGUMENT �      The user passed a 0-length buffer into the function.
                 - CE_BADCACHEREAD �          A directory entry could not be cached.
                 - CE_BAD_SECTOR_READ �       The function could not determine a previous directory
                                              of the current working directory.
    FSmkdir      -
                 - CE_GOOD �                  No Error
                 - CE_WRITE_PROTECTED �       The device write-protect check function indicated
                                              that the device has been write-protected.
                 - CE_INVALID_ARGUMENT �      The path string was mis-formed.
                 - CE_BADCACHEREAD �          Could not successfully change to a recently created
                                              directory to store its dir entry information, or
                                              could not cache directory entry information.
                 - CE_INVALID_FILENAME �      One or more of the directory names has an invalid
                                              format.
                 - CE_WRITE_ERROR �           The existing data in the data buffer could not be
                                              written to the device or the dot/dotdot entries could
                                              not be written to a newly created directory.
                 - CE_DIR_FULL �              There are no available dir entries in the CWD.
                 - CE_DISK_FULL �             There are no available clusters in the data region of
                                              the device.
    FSrmdir      -
                 - CE_GOOD �                  No Error
                 - CE_DIR_NOT_FOUND �         The directory specified could not be found or the
                                              function could not change to a subdirectory within
                                              the directory to be deleted (when recursive delete is
                                              enabled).
                 - CE_INVALID_ARGUMENT �      The user tried to remove the CWD or root directory.
                 - CE_BADCACHEREAD �          A directory entry could not be cached.
                 - CE_DIR_NOT_EMPTY �         The directory to be deleted was not empty and
                                              recursive subdirectory removal was disabled.
                 - CE_ERASE_FAIL �            The directory or one of the directories or files
                                              within it could not be deleted.
                 - CE_BAD_SECTOR_READ �       The function could not determine a previous directory
                                              of the CWD.
    SetClockVars -
                 - CE_GOOD �                  No Error
                 - CE_INVALID_ARGUMENT �      The time values passed into the function were
                                              invalid.
    FindFirst    -
                 - CE_GOOD �                  No Error
                 - CE_INVALID_FILENAME �      The specified filename was invalid.
                 - CE_FILE_NOT_FOUND �        No file matching the specified criteria was found.
                 - CE_BADCACHEREAD �          The file information for the file that was found
                                              could not be cached.
    FindNext     -
                 - CE_GOOD �                  No Error
                 - CE_NOT_INIT �              The SearchRec object was not initialized by a call to
                                              FindFirst.
                 - CE_INVALID_ARGUMENT �      The SearchRec object was initialized in a different
                                              directory from the CWD.
                 - CE_INVALID_FILENAME �      The filename is invalid.
                 - CE_FILE_NOT_FOUND �        No file matching the specified criteria was found.
    FSfprintf    -
                 - CE_GOOD �                  No Error
                 - CE_WRITE_ERROR �           Characters could not be written to the file.
  Description:
    The FSerror function will return the FSerrno variable.  This global
    variable will have been set to an error value during the last call of a
    library function.
  Remarks:
    None
  **************************************************************************/
int FSerror (void)
{
    return FSerrno;
}

/**************************************************************
  Function:
    void FileObjectCopy(FILEOBJ foDest,FILEOBJ foSource)
  Summary:
    Copy a file object
  Conditions:
    This function should not be called by the user.
  Input:
    foDest -    The destination
    foSource -  the source
  Return:
    None
  Side Effects:
    None
  Description:
    The FileObjectCopy function will make an exacy copy of
    a specified FSFILE object.
  Remarks:
    None
  **************************************************************/
void FileObjectCopy(FILEOBJ foDest,FILEOBJ foSource)
{
    int size;
    BYTE *dest;
    BYTE *source;
    int Index;

    dest = (BYTE *)foDest;
    source = (BYTE *)foSource;

    size = sizeof(FSFILE);

    for(Index=0;Index< size; Index++)
    {
        dest[Index] = source[Index];
    }
}

/***************************************************
  Function:
    BYTE ReadByte(BYTE * pBuffer, WORD index)
  Summary:
    Read a byte from a buffer
  Conditions:
    This function should not be called by the user.
  Input:
    pBuffer -  pointer to a buffer to read from
    index -    index in the buffer to read to
  Return:
    BYTE - the byte read
  Side Effects:
    None
  Description:
    Reads a byte from a buffer
  Remarks:
    None.
  ***************************************************/
BYTE ReadByte( BYTE* pBuffer, WORD index )
{
    return( pBuffer[index] );
}

/***************************************************
  Function:
    BYTE ReadWord(BYTE * pBuffer, WORD index)
  Summary:
    Read a 16-bit word from a buffer
  Conditions:
    This function should not be called by the user.
  Input:
    pBuffer -  pointer to a buffer to read from
    index -    index in the buffer to read to
  Return:
    WORD - the word read
  Side Effects:
    None
  Description:
    Reads a 16-bit word from a buffer
  Remarks:
    None.
  ***************************************************/
WORD ReadWord( BYTE* pBuffer, WORD index )
{
    BYTE loByte, hiByte;
    WORD res;

    loByte = pBuffer[index];
    hiByte = pBuffer[index+1];
    res = hiByte;
    res *= 0x100;
    res |= loByte;
    return( res );
}

/****************************************************
  Function:
    BYTE ReadDWord(BYTE * pBuffer, WORD index)
  Summary:
    Read a 32-bit double word from a buffer
  Conditions:
    This function should not be called by the user.
  Input:
    pBuffer -  pointer to a buffer to read from
    index -    index in the buffer to read to
  Return:
    DWORD - the double word read
  Side Effects:
    None
  Description:
    Reads a 32-bit double word from a buffer
  Remarks:
    None.
  ****************************************************/
DWORD ReadDWord( BYTE* pBuffer, WORD index )
{
    WORD loWord, hiWord;
    DWORD result;

    loWord = ReadWord( pBuffer, index );
    hiWord = ReadWord( pBuffer, index+2 );

    result = hiWord;
    result *= 0x10000;
    result |= loWord;
    return result;
}

/****************************************************
  Function:
    DWORD Cluster2Sector(DISK * dsk, DWORD cluster)
  Summary:
    Convert a cluster number to the corresponding sector
  Conditions:
    This function should not be called by the user.
  Input:
    disk -     Disk structure
    cluster -  Cluster to be converted
  Return:
    sector - Sector that corresponds to given cluster
  Side Effects:
    None
  Description:
    The Cluster2Sector function will calculate the
    sector number that corresponds to the first sector
    of the cluster whose value was passed into the
    function.
  Remarks:
    None.
  ****************************************************/
DWORD Cluster2Sector(DISK * dsk, DWORD cluster)
{
    DWORD sector;

    /* Rt: Settings based on FAT type */
    switch (dsk->type)
    {
#ifdef SUPPORT_FAT32 // If FAT32 supported.
        case FAT32:
            /* In FAT32, there is no separate ROOT region. It is as well stored in DATA region */
            sector = (((DWORD)cluster-2) * dsk->SecPerClus) + dsk->data;
            break;
#endif
        case FAT12:
        case FAT16:
        default:
            // The root dir takes up cluster 0 and 1
            if((cluster == 0) || (cluster == 1))
                sector = dsk->root + cluster;
            else
                sector = (((DWORD)cluster-2) * dsk->SecPerClus) + dsk->data;
            break;
    }
    return(sector);
}

/****************************************************
  Function:
    int FSfeof( FSFILE * stream )
  Summary:
    Indicate whether the current file position is at the end
  Conditions:
    File is open in a read mode
  Input:
    stream -  Pointer to the target file
  Return Values:
    Non-Zero - EOF reached
    0 - Not at end of File
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    The FSfeof function will indicate that the end-of-
    file has been reached for the specified file by
    comparing the absolute location in the file to the
    size of the file.
  Remarks:
    None.
  ****************************************************/
int FSfeof( FSFILE * stream )
{
    FSerrno = CE_GOOD;
    return( stream->seek == stream->size );
}

/**************************************************************************
  Function:
    size_t FSfread(void *ptr, size_t size, size_t n, FSFILE *stream)
  Summary:
    Read data from a file
  Conditions:
    File is opened in a read mode
  Input:
    ptr -     Destination buffer for read bytes
    size -    Size of units in bytes
    n -       Number of units to be read
    stream -  File to be read from
  Return:
    size_t - number of units read
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    The FSfread function will read data from the specified file.  First,
    the appropriate sector of the file is loaded.  Then, data is read into
    the specified buffer until the specified number of bytes have been read.
    When a cluster boundary is reached, a new cluster will be loaded.  The
    parameters 'size' and 'n' indicate how much data to read.  'Size'
    refers to the size of one object to read (in bytes), and 'n' will refer
    to the number of these objects to read.  The value returned will be equal
    to 'n' unless an error occured or the user tried to read beyond the end
    of the file.
  Remarks:
    None.
  **************************************************************************/
UINT32 FSfread (void *ptr, UINT32 size, UINT32 n, FSFILE *stream)
{
    DWORD   len = size * n;
    BYTE    *pointer = (BYTE *) ptr;
    DISK    *dsk;               // Disk structure
    DWORD    seek, sec_sel;
    WORD    pos;       //position within sector
    CETYPE   error = CE_GOOD;
    WORD    readCount = 0;

    FSerrno = CE_GOOD;

    dsk    = (DISK *)stream->dsk;
    pos    = stream->pos;
    seek    = stream->seek;

    if( !stream->flags.read )
    {
        FSerrno = CE_WRITEONLY;
        return 0;   // CE_WRITEONLY
    }

    // if it not my buffer, then get it from the disk.
    if( (gBufferOwner != stream) && (pos != dsk->sectorSize))
    {
        gBufferOwner = stream;
        sec_sel = Cluster2Sector(dsk,stream->ccls);
        sec_sel += (WORD)stream->sec;      // add the sector number to it

        gBufferZeroed = FALSE;
        if( !MDD_SDSPI_SectorRead( sec_sel, dsk->buffer) )
        {
            FSerrno = CE_BAD_SECTOR_READ;
            error = CE_BAD_SECTOR_READ;
            return 0;
        }
        gLastDataSectorRead = sec_sel;
    }

    //loop reading (count) bytes
    while( len )
    {
        if( seek == stream->size )
        {
            FSerrno = CE_EOF;
            error = CE_EOF;
            break;
        }

        // In fopen, pos is init to 0 and the sect is loaded
        if( pos == dsk->sectorSize )
        {
            // reset position
            pos = 0;
            // point to the next sector
            stream->sec++;

            // get a new cluster if necessary
            if( stream->sec == dsk->SecPerClus )
            {
                stream->sec = 0;
                if( (error = FILEget_next_cluster( stream, 1)) != CE_GOOD )
                {
                    FSerrno = CE_COULD_NOT_GET_CLUSTER;
                    break;
                }
            }

            sec_sel = Cluster2Sector(dsk,stream->ccls);
            sec_sel += (WORD)stream->sec;      // add the sector number to it


            gBufferOwner = stream;
            gBufferZeroed = FALSE;
            if( !MDD_SDSPI_SectorRead( sec_sel, dsk->buffer) )
            {
                FSerrno = CE_BAD_SECTOR_READ;
                error = CE_BAD_SECTOR_READ;
                break;
            }
            gLastDataSectorRead = sec_sel;
        }

        // copy one byte at a time
        *pointer = RAMread( dsk->buffer, pos++ );
        pointer++;
        seek++;
        readCount++;
        len--;
    }

    // save off the positon
    stream->pos = pos;
    // save off the seek
    stream->seek = seek;

    return(readCount / size);
} // fread


/***************************************************************************
  Function:
    BYTE FormatFileName( const char* fileName, FILEOBJ fptr, BYTE mode )
  Summary:
    Format a file name into dir entry format
  Conditions:
    This function should not be called by the user.
  Input:
    fileName -  The name to be formatted
    fN2 -       The location the formatted name will be stored
    mode -      Non-zero if parital string search chars are allowed
  Return Values:
    TRUE - Name formatted successfully
    FALSE - File name could not be formatted
  Side Effects:
    None
  Description:
    Format an 8.3 filename into FSFILE structure format. If filename is less
    than 8 chars, then it will be padded with spaces. If the extension name is
    fewer than 3 chars, then it will also be oadded with spaces. The
    ValidateChars function is used to ensure the characters in the specified
    filename are valid in this filesystem.
  Remarks:
    None.
  ***************************************************************************/
BYTE FormatFileName( const char* fileName, FILEOBJ fptr, BYTE mode)
{
	char *fN2;
	FILE_DIR_NAME_TYPE fileNameType;
    int temp,count1,count2,count3,count4;
    BOOL supportLFN = FALSE;
	char *localFileName = NULL;

	// go with static allocation
    unsigned short int	tempString[13];
    count1 = 12;

	// Check whether the length of the file name is valid
    fileNameLength = strlen(fileName);

    if((fileNameLength > count1) || (*fileName == '.') || (*fileName == 0))
    {
        return FALSE;
    }

    asciiFilename = (char *)tempString;
    for (count1 = 0;count1 < fileNameLength; count1++)
    {
        asciiFilename[count1] = fileName[count1];
    }

    // Make sure the characters are valid
   	fileNameType = ValidateChars(mode);

    // If the file name doesn't follow 8P3 or LFN format, then return FALSE
    if(NAME_ERROR == fileNameType)
	{
		return FALSE;
	}

	temp = fileNameLength;

	// If the file name follows 8P3 type
	if((NAME_LFN_TYPE != fileNameType) && (FALSE == supportLFN))
	{
		for (count3 = 0; count3 < temp; count3++)
		{
            if(((asciiFilename[count3] == '.') && ((temp - count3) > 4)) ||
                (count3 > 8))
            {
                // File extension greater then 3 characters or
                // File name greater then 8 charcters
                return FALSE;
                break;
            }
            else if(asciiFilename[count3] == '.')
            {
                break;
            }
		}
		
		// If LFN not supported try to adjust in 8P3 format
		if(FALSE == supportLFN)
		{
		    // point fN2 to short file name
		    fN2 = fptr -> name;
		    
		    // Load destination filename to be space intially.
		    for (count1 = 0; count1 < FILE_NAME_SIZE_8P3; count1++)
		    {
		        *(fN2 + count1) = ' ';
		    }

			// multiply the length by 2 as each UTF word has 2 byte
            count4 = count3;
            localFileName = asciiFilename;

		    //copy only file name ( not the extension part )
		    for (count1 = 0,count2 = 0; (count2 < 8) && (count1 < count4);count1++ )
		    {
				if(localFileName[count1])
				{
					fN2[count2] = localFileName[count1]; // Destination filename initially filled with SPACE. Now copy only available chars.

	    			// Convert lower-case to upper-case
	    			if ((fN2[count2] >= 0x61) && (fN2[count2] <= 0x7A))
	    			{
	    			    fN2[count2] -= 0x20;
					}
					count2++;
				}
		    }

			if(count4 < temp)
			{
				// Discard the '.' part
				count4++;

    		    // Copy the extn to 8th position onwards. Ex: "FILE    .Tx "
    		    for (count3 = 8; (count3 < 11) && (count4 < temp);count4++ )
    		    {
					if(localFileName[count4])
					{
    		        	fN2[count3] = localFileName[count4];

		   	 			// Convert lower-case to upper-case
		   	 			if ((fN2[count3] >= 0x61) && (fN2[count3] <= 0x7A))
		   	 			{
		   	 			    fN2[count3] -= 0x20;
						}
						count3++;
					}
    		    }
			}
		}
	}

	// If the file name follows LFN format
    if((NAME_LFN_TYPE == fileNameType) || (TRUE == supportLFN))
	{
		return FALSE;
	}

	// Free the temporary heap used for intermediate execution
	return TRUE;
}

/*************************************************************
  Function:
    FILE_DIR_NAME_TYPE ValidateChars(BYTE mode)
  Summary:
    Validate the characters in a given file name
  Conditions:
    This function should not be called by the user.
  Input:
    fileName -  The name to be validated
    mode -      Determines if partial string search is allowed
  Return Values:
    TRUE - Name was validated
    FALSE - File name was not valid
  Side Effects:
    None
  Description:
    The ValidateChars function will compare characters in a
    specified filename to determine if they're permissable
    in the FAT file system.  Lower-case characters will be
    converted to upper-case.  If the mode argument is specifed
    to be 'TRUE,' partial string search characters are allowed.
  Remarks:
    None.
  *************************************************************/
FILE_DIR_NAME_TYPE ValidateChars(BYTE mode)
{
	FILE_DIR_NAME_TYPE fileNameType;
    unsigned short int count1;
    unsigned char radix = FALSE,asciiValue;

 	// If the string length is greater then 8P3 length, then assume
	// the file name as LFN type provided there are no errors in the
	// below for loop.
    if(fileNameLength > TOTAL_FILE_SIZE_8P3)
    {
        fileNameType = NAME_LFN_TYPE;
    }
    else
    {
        fileNameType = NAME_8P3_ASCII_CAPS_TYPE;
    }
	
	for( count1 = 0; count1 < fileNameLength; count1++ )
	{
        asciiValue = asciiFilename[count1];
        if(((asciiValue < 0x20) && (asciiValue != 0x05)) || (asciiValue == 0x22) ||
            (asciiValue == 0x2F) || (asciiValue == 0x3A) || (asciiValue == 0x3C) ||
            (asciiValue == 0x3E) || (asciiValue == 0x5C) || (asciiValue == 0x7C))
        {
            return NAME_ERROR;
        }

        // Check for partial string search chars
        if (mode == FALSE)
        {
            if ((asciiValue == '*') || (asciiValue == '?'))
            {
                return NAME_ERROR;
            }
        }

        if(fileNameType != NAME_LFN_TYPE)
        {
            // Characters valid for LFN format only
            if ((asciiValue == 0x20) || (asciiValue == 0x2B) || (asciiValue == 0x2C) ||
                (asciiValue == 0x3B) || (asciiValue == 0x3D) || (asciiValue == 0x5B) ||
                (asciiValue == 0x5D) || ((asciiValue == 0x2E) && (radix == TRUE)))
            {
                fileNameType = NAME_LFN_TYPE;
                continue;
            }

            // only one radix ('.') character is allowed in 8P3 format, where as 
            // multiple radices can be present in LFN format
            if (asciiValue == 0x2E)
            {
                radix = TRUE;
            }

            // If the characters are mixed type & are within 8P3 length range
            // then store file type as 8P3 mixed type format
            if(fileNameType != NAME_8P3_ASCII_MIXED_TYPE)
            {
                if((asciiValue >= 0x61) && (asciiValue <= 0x7A))
                {
                    fileNameType = NAME_8P3_ASCII_MIXED_TYPE;
                }
            }
        }
	}
	return fileNameType;
}

/**********************************************************************
  Function:
    int FSfseek(FSFILE *stream, long offset, int whence)
  Summary:
    Change the current position in a file
  Conditions:
    File opened
  Input:
    stream -    Pointer to file structure
    offset -    Offset from base location
    whence -
           - SEEK_SET -  Seek from start of file
           - SEEK_CUR -  Seek from current location
           - SEEK_END -  Seek from end of file (subtract offset)
  Return Values:
    0 -  Operation successful
    -1 - Operation unsuccesful
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    The FSfseek function will change the current position in the file to
    one specified by the user.  First, an absolute offset is calculated
    using the offset and base location passed in by the user.  Then, the
    position variables are updated, and the sector number that corresponds
    to the new location.  That sector is then loaded.  If the offset
    falls exactly on a cluster boundary, a new cluster will be allocated
    to the file and the position will be set to the first byte of that
    cluster.
  Remarks:
    None
  **********************************************************************/
int FSfseek(FSFILE *stream, long offset, int whence)
{
    DWORD    numsector, temp;   // lba of first sector of first cluster
    DISK*   dsk;            // pointer to disk structure
    BYTE   test;
    long offset2 = offset;

    dsk = stream->dsk;

    switch(whence)
    {
        case SEEK_CUR:
            // Apply the offset to the current position
            offset2 += stream->seek;
            break;
        case SEEK_END:
            // Apply the offset to the end of the file
            offset2 = stream->size - offset2;
            break;
        case SEEK_SET:
            // automatically there
        default:
            break;
   }

    // start from the beginning
    temp = stream->cluster;
    stream->ccls = temp;

    temp = stream->size;

    if (offset2 > temp)
    {
        FSerrno = CE_INVALID_ARGUMENT;
        return (-1);      // past the limits
    }
    else
    {
        // if we are writing we are no longer at the end
        stream->flags.FileWriteEOF = FALSE;

        // set the new postion
        stream->seek = offset2;

        // figure out how many sectors
        numsector = offset2 / dsk->sectorSize;

        // figure out how many bytes off of the offset
        offset2 = offset2 - (numsector * dsk->sectorSize);
        stream->pos = offset2;

        // figure out how many clusters
        temp = numsector / dsk->SecPerClus;

        // figure out the stranded sectors
        numsector = numsector - (dsk->SecPerClus * temp);
        stream->sec = numsector;

        // if we are in the current cluster stay there
        if (temp > 0)
        {
            test = FILEget_next_cluster(stream, temp);
            if (test != CE_GOOD)
            {
                if (test == CE_FAT_EOF)
                {
                    stream->ccls = stream->cluster;
                    test = FILEget_next_cluster(stream, temp - 1);
                    if (test != CE_GOOD)
                    {
                        FSerrno = CE_COULD_NOT_GET_CLUSTER;
                        return (-1);
                    }
                    stream->pos = dsk->sectorSize;
                    stream->sec = dsk->SecPerClus - 1;
                }
                else
                {
                    FSerrno = CE_COULD_NOT_GET_CLUSTER;
                    return (-1);   // past the limits
                }
            }
        }

        // Determine the lba of the selected sector and load
        temp = Cluster2Sector(dsk,stream->ccls);

        // now the extra sectors
        numsector = stream->sec;
        temp += numsector;

        gBufferOwner = NULL;
        gBufferZeroed = FALSE;
        if( !MDD_SDSPI_SectorRead(temp, dsk->buffer) )
        {
            FSerrno = CE_BADCACHEREAD;
            return (-1);   // Bad read
        }
        gLastDataSectorRead = temp;
    }

    FSerrno = CE_GOOD;

    return (0);
}


/***********************************************
  Function:
    DWORD ReadFAT (DISK *dsk, DWORD ccls)
  Summary:
    Read the next entry from the FAT
  Conditions:
    This function should not be called by the user.
  Input:
    dsk -   The disk structure
    ccls -  The current cluster
  Return:
    DWORD - The next cluster in a file chain
  Side Effects:
    None
  Description:
    The ReadFAT function will read the FAT and
    determine the next cluster value after the
    cluster specified by 'ccls.' Note that the
    FAT sector that is read is stored in the
    global FAT cache buffer.
  Remarks:
    None.
  ***********************************************/
DWORD ReadFAT (DISK *dsk, DWORD ccls)
{
    BYTE q;
    DWORD p, l;  // "l" is the sector Address
    DWORD c = 0, d, ClusterFailValue,LastClusterLimit;   // ClusterEntries

    gBufferZeroed = FALSE;

    /* Settings based on FAT type */
    switch (dsk->type)
    {
#ifdef SUPPORT_FAT32 // If FAT32 supported.
        case FAT32:
            p = (DWORD)ccls * 4;
            q = 0; // "q" not used for FAT32, only initialized to remove a warning
            ClusterFailValue = CLUSTER_FAIL_FAT32;
            LastClusterLimit = LAST_CLUSTER_FAT32;
            break;
#endif
        case FAT12:
            p = (DWORD) ccls *3;  // Mulby1.5 to find cluster pos in FAT
            q = p&1;
            p >>= 1;
            ClusterFailValue = CLUSTER_FAIL_FAT16;
            LastClusterLimit = LAST_CLUSTER_FAT12;
            break;
        case FAT16:
        default:
            p = (DWORD)ccls *2;     // Mulby 2 to find cluster pos in FAT
            q = 0; // "q" not used for FAT16, only initialized to remove a warning
            ClusterFailValue = CLUSTER_FAIL_FAT16;
            LastClusterLimit = LAST_CLUSTER_FAT16;
            break;
    }

    l = dsk->fat + (p / dsk->sectorSize);     //
    p &= dsk->sectorSize - 1;                 // Restrict 'p' within the FATbuffer size

    // Check if the appropriate FAT sector is already loaded
    if (gLastFATSectorRead == l)
    {
#ifdef SUPPORT_FAT32 // If FAT32 supported.
        if (dsk->type == FAT32)
            c = RAMreadD (gFATBuffer, p);
        else
#endif
            if(dsk->type == FAT16)
                c = RAMreadW (gFATBuffer, p);
            else if(dsk->type == FAT12)
            {
                c = RAMread (gFATBuffer, p);
                if (q)
                {
                    c >>= 4;
                }
                // Check if the MSB is across the sector boundry
                p = (p +1) & (dsk->sectorSize-1);
                if (p == 0)
                {
                    // Start by writing the sector we just worked on to the card
                    // if we need to
                    if (!MDD_SDSPI_SectorRead (l+1, gFATBuffer))
                    {
                        gLastFATSectorRead = 0xFFFF;
                        return ClusterFailValue;
                    }
                    else
                    {
                        gLastFATSectorRead = l +1;
                    }
                }
                d = RAMread (gFATBuffer, p);
                if (q)
                {
                    c += (d <<4);
                }
                else
                {
                    c += ((d & 0x0F)<<8);
                }
            }
        }
        else
        {
            // If there's a currently open FAT sector,
            // write it back before reading into the buffer
            if (!MDD_SDSPI_SectorRead (l, gFATBuffer))
            {
                gLastFATSectorRead = 0xFFFF;  // Note: It is Sector not Cluster.
                return ClusterFailValue;
            }
            else
            {
                gLastFATSectorRead = l;

#ifdef SUPPORT_FAT32 // If FAT32 supported.
                if (dsk->type == FAT32)
                    c = RAMreadD (gFATBuffer, p);
                else
#endif
                    if(dsk->type == FAT16)
                        c = RAMreadW (gFATBuffer, p);
                    else if (dsk->type == FAT12)
                    {
                        c = RAMread (gFATBuffer, p);
                        if (q)
                        {
                            c >>= 4;
                        }
                        p = (p +1) & (dsk->sectorSize-1);
                        d = RAMread (gFATBuffer, p);
                        if (q)
                        {
                            c += (d <<4);
                        }
                        else
                        {
                            c += ((d & 0x0F)<<8);
                        }
                    }
            }
    }

    // Normalize it so 0xFFFF is an error
    if (c >= LastClusterLimit)
        c = LastClusterLimit;

   return c;
}   // ReadFAT

#ifdef ALLOW_FILESEARCH
/***********************************************************************************
  Function:
    int FindFirst (const char * fileName, unsigned int attr, SearchRec * rec)
  Summary:
    Initial search function for the input Ascii fileName on PIC24/PIC32/dsPIC devices.
  Conditions:
    None
  Input:
    fileName - The name to search for
             - Parital string search characters
             - * - Indicates the rest of the filename or extension can vary (e.g. FILE.*)
             - ? - Indicates that one character in a filename can vary (e.g. F?LE.T?T)
    attr -            The attributes that a found file may have
         - ATTR_READ_ONLY -  File may be read only
         - ATTR_HIDDEN -     File may be a hidden file
         - ATTR_SYSTEM -     File may be a system file
         - ATTR_VOLUME -     Entry may be a volume label
         - ATTR_DIRECTORY -  File may be a directory
         - ATTR_ARCHIVE -    File may have archive attribute
         - ATTR_MASK -       All attributes
    rec -             pointer to a structure to put the file information in
  Return Values:
    0 -  File was found
    -1 - No file matching the specified criteria was found
  Side Effects:
    Search criteria from previous FindFirst call on passed SearchRec object
    will be lost. "utf16LFNfound" is overwritten after subsequent FindFirst/FindNext
    operations.It is the responsibility of the application to read the "utf16LFNfound"
    before it is lost.The FSerrno variable will be changed.
  Description:
    Initial search function for the input Ascii fileName on PIC24/PIC32/dsPIC devices.
    The FindFirst function will search for a file based on parameters passed in
    by the user.  This function will use the FILEfind function to parse through
    the current working directory searching for entries that match the specified
    parameters.  If a file is found, its parameters are copied into the SearchRec
    structure, as are the initial parameters passed in by the user and the position
    of the file entry in the current working directory.If the return value of the 
    function is 0 then "utf16LFNfoundLength" indicates whether the file found was 
    long file name or short file name(8P3 format). The "utf16LFNfoundLength" is non-zero
    for long file name and is zero for 8P3 format."utf16LFNfound" points to the
    address of long file name if found during the operation.
  Remarks:
    Call FindFirst or FindFirstpgm before calling FindNext
  ***********************************************************************************/
int FindFirst (const char * fileName, unsigned int attr, SearchRec * rec)
{
    FSFILE f;
    FILEOBJ fo = &f;
    WORD fHandle;
    BYTE j;
    BYTE Index;
    FSerrno = CE_GOOD;

	// Format the file name as per 8.3 format or LFN format
    if( !FormatFileName(fileName, fo, 1) )
    {
        FSerrno = CE_INVALID_FILENAME;
        return -1;
    }

    rec->initialized = FALSE;

    for (Index = 0; (Index < 12) && (fileName[Index] != 0); Index++)
    {
        rec->searchname[Index] = fileName[Index];
    }
    
    for (;Index < FILE_NAME_SIZE_8P3 + 2; Index++)
    {
        rec->searchname[Index] = 0;
    }

    rec->searchattr = attr;
    rec->cwdclus = FatRootDirClusterValue;

    fo->dsk = &gDiskData;
    fo->cluster = 0;
    fo->ccls    = 0;
    fo->entry = 0;
    fo->attributes = attr;

    // start at the root directory
    fo->dirclus    = FatRootDirClusterValue;
    fo->dirccls    = FatRootDirClusterValue;

    // copy file object over
    FileObjectCopy(&gFileTemp, fo);

    // See if the file is found
    if (FILEfind (fo, &gFileTemp,LOOK_FOR_MATCHING_ENTRY, 1) != CE_GOOD)
    {
        FSerrno = CE_FILE_NOT_FOUND;
        return -1;
    }

    fHandle = fo->entry;

    if (FILEopen (fo, &fHandle, 'r') == CE_GOOD)
    {
		for(j = 0; j < FILE_NAME_SIZE_8P3 + 2 ; j++)
		{
            rec->filename[j] = 0;
		}

        // Copy as much name as there is
        if (fo->attributes != ATTR_VOLUME)
        {
            for (Index = 0, j = 0; (j < 8) && (fo->name[j] != 0x20); Index++, j++)
            {
               rec->filename[Index] = fo->name[j];
            }

			if(fo->name[8] != 0x20)
			{
            	rec->filename[Index++] = '.';

	            // Move to the extension, even if there are more space chars
	            for (j = 8; (j < 11) && (fo->name[j] != 0x20); Index++, j++)
	            {
	               rec->filename[Index] = fo->name[j];
	            }
			}
        }
        else
        {
            for (Index = 0; Index < DIR_NAMECOMP; Index++)
            {
                rec->filename[Index] = fo->name[Index];
            }
        }

        rec->attributes = fo->attributes;
        rec->filesize = fo->size;
        rec->timestamp = (DWORD)((DWORD)fo->date << 16) + fo->time;
        rec->entry = fo->entry;
        rec->initialized = TRUE;
        return 0;
    }
    else
    {
        FSerrno = CE_BADCACHEREAD;
        return -1;
    }
}

/**********************************************************************
  Function:
    int FindNext (SearchRec * rec)
  Summary:
    Sequential search function
  Conditions:
    None
  Input:
    rec -  The structure to store the file information in
  Return Values:
    0 -  File was found
    -1 - No additional files matching the specified criteria were found
  Side Effects:
    Search criteria from previous FindNext call on passed SearchRec object
    will be lost. "utf16LFNfound" is overwritten after subsequent FindFirst/FindNext
    operations.It is the responsibility of the application to read the "utf16LFNfound"
    before it is lost.The FSerrno variable will be changed.
  Description:
    The FindNext function performs the same function as the FindFirst
    funciton, except it does not copy any search parameters into the
    SearchRec structure (only info about found files) and it begins
    searching at the last directory entry offset at which a file was
    found, rather than at the beginning of the current working
    directory.If the return value of the function is 0 then "utf16LFNfoundLength"
    indicates whether the file found was long file name or short file
    name(8P3 format). The "utf16LFNfoundLength" is non-zero for long file name
    and is zero for 8P3 format."utf16LFNfound" points to the address of long 
    file name if found during the operation.
  Remarks:
    Call FindFirst or FindFirstpgm before calling this function
  **********************************************************************/
int FindNext (SearchRec * rec)
{
    FSFILE f;
    FILEOBJ fo = &f;
    BYTE i, j;

    FSerrno = CE_GOOD;

    // Make sure we called FindFirst on this object
    if (rec->initialized == FALSE)
    {
        FSerrno = CE_NOT_INIT;
        return -1;
    }

    // Make sure we called FindFirst in the cwd
    // Format the file name
    if( !FormatFileName(rec->searchname, fo, 1) )
    {
        FSerrno = CE_INVALID_FILENAME;
        return -1;
    }

    /* Brn: Copy the formatted name to "fo" which is necesary before calling "FILEfind" function */
    //strcpy(fo->name,rec->searchname);

    fo->dsk = &gDiskData;
    fo->cluster = 0;
    fo->ccls    = 0;
    fo->entry = rec->entry + 1;
    fo->attributes = rec->searchattr;

    // start at the root directory
    fo->dirclus    = FatRootDirClusterValue;
    fo->dirccls    = FatRootDirClusterValue;

    // copy file object over
    FileObjectCopy(&gFileTemp, fo);

    // See if the file is found
    if (CE_GOOD != FILEfind (fo, &gFileTemp,LOOK_FOR_MATCHING_ENTRY, 1))
    {
        FSerrno = CE_FILE_NOT_FOUND;
        return -1;
    }
    else
    {
		for(j = 0; j < FILE_NAME_SIZE_8P3 + 2 ; j++)
		{
            rec->filename[j] = 0;
		}

        if (fo->attributes != ATTR_VOLUME)
        {
            for (i = 0, j = 0; (j < 8) && (fo->name[j] != 0x20); i++, j++)
            {
               rec->filename[i] = fo->name[j];
            }

			if(fo->name[8] != 0x20)
			{
            	rec->filename[i++] = '.';

	            // Move to the extension, even if there are more space chars
	            for (j = 8; (j < 11) && (fo->name[j] != 0x20); i++, j++)
	            {
	               rec->filename[i] = fo->name[j];
	            }
			}
        }
        else
        {
            for (i = 0; i < DIR_NAMECOMP; i++)
            {
                rec->filename[i] = fo->name[i];
            }
        }

        rec->attributes = fo->attributes;
        rec->filesize = fo->size;
        rec->timestamp = (DWORD)((DWORD)fo->date << 16) + fo->time;
        rec->entry = fo->entry;
        return 0;
    }
}

#endif
