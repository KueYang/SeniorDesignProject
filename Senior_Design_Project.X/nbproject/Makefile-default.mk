#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Senior_Design_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Senior_Design_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=FIO_Library/FSIO.c FIO_Library/SD-SPI.c main.c ADC.c I2C.c IO.c SPI.c TIMER.c UART.c FIFO.c Audio.c FILES.c DAC.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/FIO_Library/FSIO.o ${OBJECTDIR}/FIO_Library/SD-SPI.o ${OBJECTDIR}/main.o ${OBJECTDIR}/ADC.o ${OBJECTDIR}/I2C.o ${OBJECTDIR}/IO.o ${OBJECTDIR}/SPI.o ${OBJECTDIR}/TIMER.o ${OBJECTDIR}/UART.o ${OBJECTDIR}/FIFO.o ${OBJECTDIR}/Audio.o ${OBJECTDIR}/FILES.o ${OBJECTDIR}/DAC.o
POSSIBLE_DEPFILES=${OBJECTDIR}/FIO_Library/FSIO.o.d ${OBJECTDIR}/FIO_Library/SD-SPI.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/ADC.o.d ${OBJECTDIR}/I2C.o.d ${OBJECTDIR}/IO.o.d ${OBJECTDIR}/SPI.o.d ${OBJECTDIR}/TIMER.o.d ${OBJECTDIR}/UART.o.d ${OBJECTDIR}/FIFO.o.d ${OBJECTDIR}/Audio.o.d ${OBJECTDIR}/FILES.o.d ${OBJECTDIR}/DAC.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/FIO_Library/FSIO.o ${OBJECTDIR}/FIO_Library/SD-SPI.o ${OBJECTDIR}/main.o ${OBJECTDIR}/ADC.o ${OBJECTDIR}/I2C.o ${OBJECTDIR}/IO.o ${OBJECTDIR}/SPI.o ${OBJECTDIR}/TIMER.o ${OBJECTDIR}/UART.o ${OBJECTDIR}/FIFO.o ${OBJECTDIR}/Audio.o ${OBJECTDIR}/FILES.o ${OBJECTDIR}/DAC.o

# Source Files
SOURCEFILES=FIO_Library/FSIO.c FIO_Library/SD-SPI.c main.c ADC.c I2C.c IO.c SPI.c TIMER.c UART.c FIFO.c Audio.c FILES.c DAC.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Senior_Design_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX250F128B
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/FIO_Library/FSIO.o: FIO_Library/FSIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FIO_Library" 
	@${RM} ${OBJECTDIR}/FIO_Library/FSIO.o.d 
	@${RM} ${OBJECTDIR}/FIO_Library/FSIO.o 
	@${FIXDEPS} "${OBJECTDIR}/FIO_Library/FSIO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/FIO_Library/FSIO.o.d" -o ${OBJECTDIR}/FIO_Library/FSIO.o FIO_Library/FSIO.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/FIO_Library/SD-SPI.o: FIO_Library/SD-SPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FIO_Library" 
	@${RM} ${OBJECTDIR}/FIO_Library/SD-SPI.o.d 
	@${RM} ${OBJECTDIR}/FIO_Library/SD-SPI.o 
	@${FIXDEPS} "${OBJECTDIR}/FIO_Library/SD-SPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/FIO_Library/SD-SPI.o.d" -o ${OBJECTDIR}/FIO_Library/SD-SPI.o FIO_Library/SD-SPI.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/ADC.o: ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ADC.o.d 
	@${RM} ${OBJECTDIR}/ADC.o 
	@${FIXDEPS} "${OBJECTDIR}/ADC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/ADC.o.d" -o ${OBJECTDIR}/ADC.o ADC.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/I2C.o: I2C.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/I2C.o.d 
	@${RM} ${OBJECTDIR}/I2C.o 
	@${FIXDEPS} "${OBJECTDIR}/I2C.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/I2C.o.d" -o ${OBJECTDIR}/I2C.o I2C.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/IO.o: IO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/IO.o.d 
	@${RM} ${OBJECTDIR}/IO.o 
	@${FIXDEPS} "${OBJECTDIR}/IO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/IO.o.d" -o ${OBJECTDIR}/IO.o IO.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/SPI.o: SPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/SPI.o.d 
	@${RM} ${OBJECTDIR}/SPI.o 
	@${FIXDEPS} "${OBJECTDIR}/SPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/SPI.o.d" -o ${OBJECTDIR}/SPI.o SPI.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/TIMER.o: TIMER.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/TIMER.o.d 
	@${RM} ${OBJECTDIR}/TIMER.o 
	@${FIXDEPS} "${OBJECTDIR}/TIMER.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/TIMER.o.d" -o ${OBJECTDIR}/TIMER.o TIMER.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/UART.o: UART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UART.o.d 
	@${RM} ${OBJECTDIR}/UART.o 
	@${FIXDEPS} "${OBJECTDIR}/UART.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/UART.o.d" -o ${OBJECTDIR}/UART.o UART.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/FIFO.o: FIFO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/FIFO.o.d 
	@${RM} ${OBJECTDIR}/FIFO.o 
	@${FIXDEPS} "${OBJECTDIR}/FIFO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/FIFO.o.d" -o ${OBJECTDIR}/FIFO.o FIFO.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Audio.o: Audio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Audio.o.d 
	@${RM} ${OBJECTDIR}/Audio.o 
	@${FIXDEPS} "${OBJECTDIR}/Audio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/Audio.o.d" -o ${OBJECTDIR}/Audio.o Audio.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/FILES.o: FILES.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/FILES.o.d 
	@${RM} ${OBJECTDIR}/FILES.o 
	@${FIXDEPS} "${OBJECTDIR}/FILES.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/FILES.o.d" -o ${OBJECTDIR}/FILES.o FILES.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/DAC.o: DAC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DAC.o.d 
	@${RM} ${OBJECTDIR}/DAC.o 
	@${FIXDEPS} "${OBJECTDIR}/DAC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/DAC.o.d" -o ${OBJECTDIR}/DAC.o DAC.c    -legacy-libc  $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/FIO_Library/FSIO.o: FIO_Library/FSIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FIO_Library" 
	@${RM} ${OBJECTDIR}/FIO_Library/FSIO.o.d 
	@${RM} ${OBJECTDIR}/FIO_Library/FSIO.o 
	@${FIXDEPS} "${OBJECTDIR}/FIO_Library/FSIO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/FIO_Library/FSIO.o.d" -o ${OBJECTDIR}/FIO_Library/FSIO.o FIO_Library/FSIO.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/FIO_Library/SD-SPI.o: FIO_Library/SD-SPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FIO_Library" 
	@${RM} ${OBJECTDIR}/FIO_Library/SD-SPI.o.d 
	@${RM} ${OBJECTDIR}/FIO_Library/SD-SPI.o 
	@${FIXDEPS} "${OBJECTDIR}/FIO_Library/SD-SPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/FIO_Library/SD-SPI.o.d" -o ${OBJECTDIR}/FIO_Library/SD-SPI.o FIO_Library/SD-SPI.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/ADC.o: ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ADC.o.d 
	@${RM} ${OBJECTDIR}/ADC.o 
	@${FIXDEPS} "${OBJECTDIR}/ADC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/ADC.o.d" -o ${OBJECTDIR}/ADC.o ADC.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/I2C.o: I2C.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/I2C.o.d 
	@${RM} ${OBJECTDIR}/I2C.o 
	@${FIXDEPS} "${OBJECTDIR}/I2C.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/I2C.o.d" -o ${OBJECTDIR}/I2C.o I2C.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/IO.o: IO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/IO.o.d 
	@${RM} ${OBJECTDIR}/IO.o 
	@${FIXDEPS} "${OBJECTDIR}/IO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/IO.o.d" -o ${OBJECTDIR}/IO.o IO.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/SPI.o: SPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/SPI.o.d 
	@${RM} ${OBJECTDIR}/SPI.o 
	@${FIXDEPS} "${OBJECTDIR}/SPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/SPI.o.d" -o ${OBJECTDIR}/SPI.o SPI.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/TIMER.o: TIMER.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/TIMER.o.d 
	@${RM} ${OBJECTDIR}/TIMER.o 
	@${FIXDEPS} "${OBJECTDIR}/TIMER.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/TIMER.o.d" -o ${OBJECTDIR}/TIMER.o TIMER.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/UART.o: UART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UART.o.d 
	@${RM} ${OBJECTDIR}/UART.o 
	@${FIXDEPS} "${OBJECTDIR}/UART.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/UART.o.d" -o ${OBJECTDIR}/UART.o UART.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/FIFO.o: FIFO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/FIFO.o.d 
	@${RM} ${OBJECTDIR}/FIFO.o 
	@${FIXDEPS} "${OBJECTDIR}/FIFO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/FIFO.o.d" -o ${OBJECTDIR}/FIFO.o FIFO.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Audio.o: Audio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Audio.o.d 
	@${RM} ${OBJECTDIR}/Audio.o 
	@${FIXDEPS} "${OBJECTDIR}/Audio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/Audio.o.d" -o ${OBJECTDIR}/Audio.o Audio.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/FILES.o: FILES.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/FILES.o.d 
	@${RM} ${OBJECTDIR}/FILES.o 
	@${FIXDEPS} "${OBJECTDIR}/FILES.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/FILES.o.d" -o ${OBJECTDIR}/FILES.o FILES.c    -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/DAC.o: DAC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DAC.o.d 
	@${RM} ${OBJECTDIR}/DAC.o 
	@${FIXDEPS} "${OBJECTDIR}/DAC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -D_SUPPRESS_PLIB_WARNING -D_DISABLE_OPENADC10_CONFIGPORT_WARNING -MMD -MF "${OBJECTDIR}/DAC.o.d" -o ${OBJECTDIR}/DAC.o DAC.c    -legacy-libc  $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Senior_Design_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Senior_Design_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -legacy-libc  $(COMPARISON_BUILD)    -mreserve=boot@0x1FC00490:0x1FC00BEF  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/Senior_Design_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Senior_Design_Project.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -legacy-libc  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Senior_Design_Project.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
