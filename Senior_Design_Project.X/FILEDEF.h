#ifndef _FILEDEF_H    /* Guard against multiple inclusion */
#define _FILEDEF_H

/** @def MAX_NUM_OF_FILES 
 * Defines the max number of audio files to be open. */
#define MAX_NUM_OF_FILES        21

/** @def FILE_0 
 * Defines file index 0. */
#define FILE_0      0
/** @def FILE_1 
 * Defines file index 1. */
#define FILE_1      1
/** @def FILE_2 
 * Defines file index 2. */
#define FILE_2      2
/** @def FILE_3 
 * Defines file index 3. */
#define FILE_3      3
/** @def FILE_4 
 * Defines file index 4. */
#define FILE_4      4
/** @def FILE_5 
 * Defines file index 5. */
#define FILE_5      5
/** @def FILE_6 
 * Defines file index 6. */
#define FILE_6      6
/** @def FILE_7 
 * Defines file index 7. */
#define FILE_7      7
/** @def FILE_8 
 * Defines file index 8. */
#define FILE_8      8
/** @def FILE_9 
 * Defines file index 9. */
#define FILE_9      9
/** @def FILE_10 
 * Defines file index 10. */
#define FILE_10      10
/** @def FILE_11 
 * Defines file index 11. */
#define FILE_11      11
/** @def FILE_12 
 * Defines file index 12. */
#define FILE_12      12
/** @def FILE_3 
 * Defines file index 13. */
#define FILE_13      13
/** @def FILE_4 
 * Defines file index 14. */
#define FILE_14      14
/** @def FILE_15 
 * Defines file index 15. */
#define FILE_15      15
/** @def FILE_16 
 * Defines file index 16. */
#define FILE_16      16
/** @def FILE_17 
 * Defines file index 17. */
#define FILE_17      17
/** @def FILE_18 
 * Defines file index 18. */
#define FILE_18      18
/** @def FILE_19 
 * Defines file index 19. */
#define FILE_19      19
/** @def FILE_20 
 * Defines file index 20. */
#define FILE_20      20

/** @var PIC1 
 * Defines the selected PIC. */
#define PIC1

/** @var fileNames 
 * Stores the list of audio file names. */
const char* fileNames[MAX_NUM_OF_FILES] = 
#ifdef PIC1
{
    "S1_0.wav",
    "S1_1.wav",
    "S1_2.wav",
    "S1_3.wav",
    "S1_4.wav",
    "S1_5.wav",
    "S1_6.wav",
    "S1_7.wav",
    "S1_8.wav",
    "S1_9.wav",
    "S1_10.wav",
    "S1_11.wav",
    "S1_12.wav",
    "S1_13.wav",
    "S1_14.wav",
    "S1_15.wav",
    "S1_16.wav",
    "S1_17.wav",
    "S1_18.wav",
    "S1_19.wav",
    "S1_20.wav"
};
#elif PIC2
{
    "S2_0.wav",
    "S2_1.wav",
    "S2_2.wav",
    "S2_3.wav",
    "S2_4.wav",
    "S2_5.wav",
    "S2_6.wav",
    "S2_7.wav",
    "S2_8.wav",
    "S2_9.wav",
    "S2_10.wav",
    "S2_11.wav",
    "S2_12.wav",
    "S2_13.wav",
    "S2_14.wav",
    "S2_15.wav",
    "S2_16.wav",
    "S2_17.wav",
    "S2_18.wav",
    "S2_19.wav",
    "S2_20.wav"
};
#elif PIC3
{
    "S3_0.wav",
    "S3_1.wav",
    "S3_2.wav",
    "S3_3.wav",
    "S3_4.wav",
    "S3_5.wav",
    "S3_6.wav",
    "S3_7.wav",
    "S3_8.wav",
    "S3_9.wav",
    "S3_10.wav",
    "S3_11.wav",
    "S3_12.wav",
    "S3_13.wav",
    "S3_14.wav",
    "S3_15.wav",
    "S3_16.wav",
    "S3_17.wav",
    "S3_18.wav",
    "S3_19.wav",
    "S3_20.wav"
}
#elif PIC4
{
    "S4_0.wav",
    "S4_1.wav",
    "S4_2.wav",
    "S4_3.wav",
    "S4_4.wav",
    "S4_5.wav",
    "S4_6.wav",
    "S4_7.wav",
    "S4_8.wav",
    "S4_9.wav",
    "S4_10.wav",
    "S4_11.wav",
    "S4_12.wav",
    "S4_13.wav",
    "S4_14.wav",
    "S4_15.wav",
    "S4_16.wav",
    "S4_17.wav",
    "S4_18.wav",
    "S4_19.wav",
    "S4_20.wav"
}
#elif PIC5
{
    "S5_0.wav",
    "S5_1.wav",
    "S5_2.wav",
    "S5_3.wav",
    "S5_4.wav",
    "S5_5.wav",
    "S5_6.wav",
    "S5_7.wav",
    "S5_8.wav",
    "S5_9.wav",
    "S5_10.wav",
    "S5_11.wav",
    "S5_12.wav",
    "S5_13.wav",
    "S5_14.wav",
    "S5_15.wav",
    "S5_16.wav",
    "S5_17.wav",
    "S5_18.wav",
    "S5_19.wav",
    "S5_20.wav"
};
#else
{
    "S6_0.wav",
    "S6_1.wav",
    "S6_2.wav",
    "S6_3.wav",
    "S6_4.wav",
    "S6_5.wav",
    "S6_6.wav",
    "S6_7.wav",
    "S6_8.wav",
    "S6_9.wav",
    "S6_10.wav",
    "S6_11.wav",
    "S6_12.wav",
    "S6_13.wav",
    "S6_14.wav",
    "S6_15.wav",
    "S6_16.wav",
    "S6_17.wav",
    "S6_18.wav",
    "S6_19.wav",
    "S6_20.wav"
};
#endif

#ifdef __cplusplus
}
#endif

#endif 
