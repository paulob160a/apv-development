/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvCommandAndControlSerialInterfaceMain.c                                  */
/* 01.06.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <conio.h>
#include <process.h>
#include "ApvError.h"
#include "ActiveXInC.h"
#include "ApvCommandAndControlSerialInterface.h"
#include "ApvCommandAndControlSerialInterfaceMain.h"

/******************************************************************************/
/* Constants :                                                                */
/******************************************************************************/

#define ARGV_0         0          
#define ARGV_1        (ARGV_0 + 1)
#define ARGV_2        (ARGV_1 + 1)
#define ARGV_3        (ARGV_2 + 1)
#define ARGV_4        (ARGV_3 + 1)
#define ARGV_5        (ARGV_4 + 1)
#define ARGV_6        (ARGV_5 + 1)
#define ARGV_7        (ARGV_6 + 1)
#define ARGV_8        (ARGV_7 + 1)
#define ARGV_NUM_ARGS (ARGV_8 + 1)

/******************************************************************************/
/* The "humanised" names for the application command-line arguments           */
/******************************************************************************/

#define APV_COMMAND_AND_CONTROL_SERIAL_INTERFACE argv[ARGV_0]  // command-line argument 0 : the program name
#define APV_SERIAL_SERVER_PATH                   argv[ARGV_1]  // the path to the RealTerm executable
#define APV_SERIAL_SERVER                        argv[ARGV_2]  // the full name of the RealTerm executable as <name>.<ext>
#define APV_BAUDS_SELECT                         argv[ARGV_3]  // one of a number of possible symbol rates
#define APV_PORT_SELECT                          argv[ARGV_4]  // one of the available COM/USB serial ports
#define APV_PORT_SEND_FILENAME                   argv[ARGV_5]  // name of a file to send
#define APV_PORT_CHAR_DELAY                      argv[ARGV_6]  // inter-character delay
#define APV_PORT_SEND_REPEAT                     argv[ARGV_7]  // the number of times to send a file
#define APV_PORT_ACTIVE_SELECT                   argv[ARGV_8]  // start with the port open or closed
#define APV_NUMBER_OF_ARGUMENTS                  ARGV_NUM_ARGS // the expected number of command-line arguments

/******************************************************************************/
/* Local Variable Declarations :                                              */
/******************************************************************************/

static const char apvSymbolRates[APV_BAUDS_SET][APV_BAUDS_MAXIMUM_SYMBOL_LENGTH] = 
  {
  APV_BAUD_9600,
  APV_BAUD_19200,
  APV_BAUD_38400,
  APV_BAUD_57600,
  APV_BAUD_76800,
  APV_BAUD_115200
  };

// The commands all have prefixes and the sub-fields are tacked onto these
char apvRealTermCommands[APV_REALTERM_COMMAND_PREFIXES][APV_REALTERM_COMMAND_PREFIX_LENGTH] = 
  {
  APV_REALTERM_COMMAND_PREFIX_BAUD,
  APV_REALTERM_COMMAND_PREFIX_PORT,
  APV_REALTERM_COMMAND_PREFIX_PORTQUIT,
  APV_REALTERM_COMMAND_PREFIX_DATA,
  APV_REALTERM_COMMAND_PREFIX_FRAMESIZE,
  APV_REALTERM_COMMAND_PREFIX_CAPFILE,
  APV_REALTERM_COMMAND_PREFIX_CAPCOUNT,
  APV_REALTERM_COMMAND_PREFIX_CAPSECS,
  APV_REALTERM_COMMAND_PREFIX_CAPTURE,
  APV_REALTERM_COMMAND_PREFIX_CAPAUTONAME,
  APV_REALTERM_COMMAND_PREFIX_CAPPROCESS,
  APV_REALTERM_COMMAND_PREFIX_CAPQUIT,
  APV_REALTERM_COMMAND_PREFIX_CAPHEX,
  APV_REALTERM_COMMAND_PREFIX_CAPDIRECT,
  APV_REALTERM_COMMAND_PREFIX_TSDELIMITER,
  APV_REALTERM_COMMAND_PREFIX_TIMESTAMP,
  APV_REALTERM_COMMAND_PREFIX_VISIBLE,
  APV_REALTERM_COMMAND_PREFIX_DISPLAY,
  APV_REALTERM_COMMAND_PREFIX_BIGEND,
  APV_REALTERM_COMMAND_PREFIX_FLOW,
  APV_REALTERM_COMMAND_PREFIX_RTS,
  APV_REALTERM_COMMAND_PREFIX_DTR,
  APV_REALTERM_COMMAND_PREFIX_CLOSED,
  APV_REALTERM_COMMAND_PREFIX_TAB,
  APV_REALTERM_COMMAND_PREFIX_ECHO,
  APV_REALTERM_COMMAND_PREFIX_EBAUD,
  APV_REALTERM_COMMAND_PREFIX_EDATA,
  APV_REALTERM_COMMAND_PREFIX_HALF,
  APV_REALTERM_COMMAND_PREFIX_CAPTION,
  APV_REALTERM_COMMAND_PREFIX_SENDFILE,
  APV_REALTERM_COMMAND_PREFIX_SENDFNAME,
  APV_REALTERM_COMMAND_PREFIX_CONTROLS,
  APV_REALTERM_COMMAND_PREFIX_MONITOR,
  APV_REALTERM_COMMAND_PREFIX_CHARDLY,
  APV_REALTERM_COMMAND_PREFIX_LINEDLY,
  APV_REALTERM_COMMAND_PREFIX_ROWS,
  APV_REALTERM_COMMAND_PREFIX_COLS,
  APV_REALTERM_COMMAND_PREFIX_SENDDLY,
  APV_REALTERM_COMMAND_PREFIX_SENDREP,
  APV_REALTERM_COMMAND_PREFIX_SENDQUIT,
  APV_REALTERM_COMMAND_PREFIX_SENDSTR,
  APV_REALTERM_COMMAND_PREFIX_SENDNUM,
  APV_REALTERM_COMMAND_PREFIX_SENDHEX,
  APV_REALTERM_COMMAND_PREFIX_SENDLIT,
  APV_REALTERM_COMMAND_PREFIX_CR,
  APV_REALTERM_COMMAND_PREFIX_LF,
  APV_REALTERM_COMMAND_PREFIX_FIRST,
  APV_REALTERM_COMMAND_PREFIX_LFNL,
  APV_REALTERM_COMMAND_PREFIX_SPY,
  APV_REALTERM_COMMAND_PREFIX_SCANPORTS,
  APV_REALTERM_COMMAND_PREFIX_I2CADD,
  APV_REALTERM_COMMAND_PREFIX_HELP,
  APV_REALTERM_COMMAND_PREFIX_INSTALL,
  APV_REALTERM_COMMAND_PREFIX_SCROLLBACK,
  APV_REALTERM_COMMAND_PREFIX_COLORS,
  APV_REALTERM_COMMAND_PREFIX_HEXCSV,
  APV_REALTERM_COMMAND_PREFIX_WINSOCK,
  APV_REALTERM_COMMAND_PREFIX_EWINSOCK,
  APV_REALTERM_COMMAND_PREFIX_INIFILE,
  APV_REALTERM_COMMAND_PREFIX_MSGBOX,
  APV_REALTERM_COMMAND_PREFIX_SCALE,
  APV_REALTERM_COMMAND_PREFIX_WINDOWSTATE,
  APV_REALTERM_COMMAND_PREFIX_CLEAR,
  APV_REALTERM_COMMAND_PREFIX_STRING1,
  APV_REALTERM_COMMAND_PREFIX_STRING2,
  APV_REALTERM_COMMAND_PREFIX_CRLF,
  APV_REALTERM_COMMAND_PREFIX_FONTNAME,
  APV_REALTERM_COMMAND_PREFIX_FONTSIZE,
  APV_REALTERM_COMMAND_PREFIX_BSYNCIS,
  APV_REALTERM_COMMAND_PREFIX_BSYNCDAT,
  APV_REALTERM_COMMAND_PREFIX_BSYNCAND,
  APV_REALTERM_COMMAND_PREFIX_BSYNCXOR,
  APV_REALTERM_COMMAND_PREFIX_BSYNCHI,
  APV_REALTERM_COMMAND_PREFIX_BSYNCLEAD,
  APV_REALTERM_COMMAND_PREFIX_BSYNCSHOWCOUNT,
  APV_REALTERM_COMMAND_PREFIX_KEYMAPVT,
  APV_REALTERM_COMMAND_PREFIX_KEYMAP,
  APV_REALTERM_COMMAND_PREFIX_CRC,
  APV_REALTERM_COMMAND_PREFIX_VERSION
  };

// These are used for successive calls to the same command to reset the prefix and clear the sub-fields
char apvRealTermPrefixes[APV_REALTERM_COMMAND_PREFIXES][APV_REALTERM_COMMAND_PREFIX_LENGTH] = 
  {
  APV_REALTERM_COMMAND_PREFIX_BAUD,
  APV_REALTERM_COMMAND_PREFIX_PORT,
  APV_REALTERM_COMMAND_PREFIX_PORTQUIT,
  APV_REALTERM_COMMAND_PREFIX_DATA,
  APV_REALTERM_COMMAND_PREFIX_FRAMESIZE,
  APV_REALTERM_COMMAND_PREFIX_CAPFILE,
  APV_REALTERM_COMMAND_PREFIX_CAPCOUNT,
  APV_REALTERM_COMMAND_PREFIX_CAPSECS,
  APV_REALTERM_COMMAND_PREFIX_CAPTURE,
  APV_REALTERM_COMMAND_PREFIX_CAPAUTONAME,
  APV_REALTERM_COMMAND_PREFIX_CAPPROCESS,
  APV_REALTERM_COMMAND_PREFIX_CAPQUIT,
  APV_REALTERM_COMMAND_PREFIX_CAPHEX,
  APV_REALTERM_COMMAND_PREFIX_CAPDIRECT,
  APV_REALTERM_COMMAND_PREFIX_TSDELIMITER,
  APV_REALTERM_COMMAND_PREFIX_TIMESTAMP,
  APV_REALTERM_COMMAND_PREFIX_VISIBLE,
  APV_REALTERM_COMMAND_PREFIX_DISPLAY,
  APV_REALTERM_COMMAND_PREFIX_BIGEND,
  APV_REALTERM_COMMAND_PREFIX_FLOW,
  APV_REALTERM_COMMAND_PREFIX_RTS,
  APV_REALTERM_COMMAND_PREFIX_DTR,
  APV_REALTERM_COMMAND_PREFIX_CLOSED,
  APV_REALTERM_COMMAND_PREFIX_TAB,
  APV_REALTERM_COMMAND_PREFIX_ECHO,
  APV_REALTERM_COMMAND_PREFIX_EBAUD,
  APV_REALTERM_COMMAND_PREFIX_EDATA,
  APV_REALTERM_COMMAND_PREFIX_HALF,
  APV_REALTERM_COMMAND_PREFIX_CAPTION,
  APV_REALTERM_COMMAND_PREFIX_SENDFILE,
  APV_REALTERM_COMMAND_PREFIX_SENDFNAME,
  APV_REALTERM_COMMAND_PREFIX_CONTROLS,
  APV_REALTERM_COMMAND_PREFIX_MONITOR,
  APV_REALTERM_COMMAND_PREFIX_CHARDLY,
  APV_REALTERM_COMMAND_PREFIX_LINEDLY,
  APV_REALTERM_COMMAND_PREFIX_ROWS,
  APV_REALTERM_COMMAND_PREFIX_COLS,
  APV_REALTERM_COMMAND_PREFIX_SENDDLY,
  APV_REALTERM_COMMAND_PREFIX_SENDREP,
  APV_REALTERM_COMMAND_PREFIX_SENDQUIT,
  APV_REALTERM_COMMAND_PREFIX_SENDSTR,
  APV_REALTERM_COMMAND_PREFIX_SENDNUM,
  APV_REALTERM_COMMAND_PREFIX_SENDHEX,
  APV_REALTERM_COMMAND_PREFIX_SENDLIT,
  APV_REALTERM_COMMAND_PREFIX_CR,
  APV_REALTERM_COMMAND_PREFIX_LF,
  APV_REALTERM_COMMAND_PREFIX_FIRST,
  APV_REALTERM_COMMAND_PREFIX_LFNL,
  APV_REALTERM_COMMAND_PREFIX_SPY,
  APV_REALTERM_COMMAND_PREFIX_SCANPORTS,
  APV_REALTERM_COMMAND_PREFIX_I2CADD,
  APV_REALTERM_COMMAND_PREFIX_HELP,
  APV_REALTERM_COMMAND_PREFIX_INSTALL,
  APV_REALTERM_COMMAND_PREFIX_SCROLLBACK,
  APV_REALTERM_COMMAND_PREFIX_COLORS,
  APV_REALTERM_COMMAND_PREFIX_HEXCSV,
  APV_REALTERM_COMMAND_PREFIX_WINSOCK,
  APV_REALTERM_COMMAND_PREFIX_EWINSOCK,
  APV_REALTERM_COMMAND_PREFIX_INIFILE,
  APV_REALTERM_COMMAND_PREFIX_MSGBOX,
  APV_REALTERM_COMMAND_PREFIX_SCALE,
  APV_REALTERM_COMMAND_PREFIX_WINDOWSTATE,
  APV_REALTERM_COMMAND_PREFIX_CLEAR,
  APV_REALTERM_COMMAND_PREFIX_STRING1,
  APV_REALTERM_COMMAND_PREFIX_STRING2,
  APV_REALTERM_COMMAND_PREFIX_CRLF,
  APV_REALTERM_COMMAND_PREFIX_FONTNAME,
  APV_REALTERM_COMMAND_PREFIX_FONTSIZE,
  APV_REALTERM_COMMAND_PREFIX_BSYNCIS,
  APV_REALTERM_COMMAND_PREFIX_BSYNCDAT,
  APV_REALTERM_COMMAND_PREFIX_BSYNCAND,
  APV_REALTERM_COMMAND_PREFIX_BSYNCXOR,
  APV_REALTERM_COMMAND_PREFIX_BSYNCHI,
  APV_REALTERM_COMMAND_PREFIX_BSYNCLEAD,
  APV_REALTERM_COMMAND_PREFIX_BSYNCSHOWCOUNT,
  APV_REALTERM_COMMAND_PREFIX_KEYMAPVT,
  APV_REALTERM_COMMAND_PREFIX_KEYMAP,
  APV_REALTERM_COMMAND_PREFIX_CRC,
  APV_REALTERM_COMMAND_PREFIX_VERSION
  };

TCHAR                apvRealTermProcessName[APV_REALTERM_COMMAND_PREFIX_LENGTH]; // "CreateProcess" is using wide characters
TCHAR                apvWideCharConversion[APV_REALTERM_COMMAND_PREFIX_LENGTH];  // char to wchar_t character conversion

char                *apvCharString  = NULL;                                      // track the char string being converted to wchar_t
TCHAR               *apvWCharString = NULL;                                      // track the wchar_t string

STARTUPINFO          apvRealTermProcessStartUpInformation;                       // process "startup information" (Windows fills this)
PROCESS_INFORMATION  apvRealTermProcessInformation;                              // process "information" (Windows fills this)

APV_ERROR_CODE       apvMessageControl = APV_ERROR_CODE_NONE;                    // enable the message-sending loop

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

       int  main(int argc, char *argv[]);
static void ApvCAndCApplicationCommandLine(void);

/******************************************************************************/
/* main() :                                                                   */
/*  --> argc : the number of command-line arguments required                  */
/*  --> argv : the command-line arguments as strings                          */
/*  <-- 0    : always returns "no errors"                                     */
/*                                                                            */
/******************************************************************************/

int main(int argc, char *argv[])
  {
/******************************************************************************/

  DISPATCH_OBJ(objRealTerm);
  LPSTR   szResponse;

  uint8_t fieldIndex = 0;

/******************************************************************************/

  if (argc != APV_NUMBER_OF_ARGUMENTS)
    {
    ApvCAndCApplicationCommandLine();
    }
  else
    {
    // Is the requested symbol rate on of the expected choices ?
    while ((strcmp(apvSymbolRates[fieldIndex], APV_BAUDS_SELECT)) && (fieldIndex < APV_BAUDS_SET))
      {
      fieldIndex = fieldIndex + 1;
      }

    if (fieldIndex == APV_BAUDS_SET)
      {
      ApvCAndCApplicationCommandLine();
      }
    else
      {
      //_execl(APV_SERIAL_SERVER_PATH, APV_SERIAL_SERVER, NULL);

      // Build the RealTerm process path and name
#ifdef UNICODE
      apvCharString  = APV_SERIAL_SERVER_PATH; // point to the char string to be converted to wchar_t
      apvWCharString = apvWideCharConversion;  // point to the wchar_t string

      while (*apvCharString != APV_STRING_EOS)
        {
        *apvWCharString = (TCHAR)(*apvCharString);

        apvCharString  = apvCharString  + 1;
        apvWCharString = apvWCharString + 1;
        }

      *apvWCharString = (TCHAR)APV_STRING_EOS;

      wcscpy((wchar_t *)&apvRealTermProcessName[0], &apvWideCharConversion[0]);

      apvCharString  = APV_SERIAL_SERVER;      // point to the char string to be converted to wchar_t
      apvWCharString = apvWideCharConversion;  // point to the wchar_t string

      while (*apvCharString != APV_STRING_EOS)
        {
        *apvWCharString = (TCHAR)(*apvCharString);

        apvCharString  = apvCharString  + 1;
        apvWCharString = apvWCharString + 1;
        }

      *apvWCharString = (TCHAR)APV_STRING_EOS;

      wcscat((wchar_t *)&apvRealTermProcessName[0], &apvWideCharConversion[0]);
#else
      strcpy((char *)&apvRealTermProcessName[0], APV_SERIAL_SERVER_PATH);
      strcat((char *)&apvRealTermProcessName[0], APV_SERIAL_SERVER);
#endif

      // Empty the process information tables
      ZeroMemory( &apvRealTermProcessStartUpInformation, sizeof(apvRealTermProcessStartUpInformation) );
      apvRealTermProcessStartUpInformation.cb = sizeof(apvRealTermProcessStartUpInformation);
      ZeroMemory( &apvRealTermProcessInformation, sizeof(apvRealTermProcessInformation) );

/*      if (CreateProcess(&apvRealTermProcessName[0],                // RealTerm process path and name
                         NULL,                                     // no command-line
                         NULL,                                     // children do not inherit from the new process
                         NULL,                                     // children do not inherit from the new process
                         TRUE,                                     // the new process inherits from this calling process
                         0,                                        // the new process is created as normal
                         NULL,                                     // the new process inherits this process's environment
                         NULL,                                     // the new process uses this process's drive and directory
                         &apvRealTermProcessStartUpInformation,    // the new process's startup configuration
                         &apvRealTermProcessInformation) == FALSE) // the new process description
        {
        printf("\n ERROR : RealTerm process creation failed!\n");
              
        while (!_kbhit())
          ;

        return(0);
        } */

      dhInitialize(TRUE);
      dhToggleExceptions(TRUE);
     
      dhCreateObject(APV_REALTERM_SERIAL_SERVER_OBJECT_TYPE, NULL, &objRealTerm);
     
      // Assemble the RealTerm baud rate command
      strcat(apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_BAUD_INDEX], APV_BAUDS_SELECT);
     
      // Assemble the RealTerm serial port selection command
      strcat(apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_PORT_INDEX], APV_PORT_SELECT);

      // Assemble the RealTerm serial port open/closed state
      strcat(apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_CLOSED_INDEX], APV_PORT_ACTIVE_SELECT);

      // Assemble the RealTerm "send" string
      strcat(apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_SENDFNAME_INDEX], APV_PORT_SEND_FILENAME);
      strcat(apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_SENDFILE_INDEX], APV_PORT_SEND_FILENAME);

      // Assemble the RealTerm inter-character delay string
      strcat(apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_CHARDLY_INDEX], APV_PORT_CHAR_DELAY);

      // Assemble the RealTerm "send repeat" string
      strcat(apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_SENDREP_INDEX], APV_PORT_SEND_REPEAT);

      // Select the RealTerm "Send" tabsheet
      strcat(apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_TAB_INDEX], APV_REALTERM_TABSHEET_NAME_SEND);

      // Setup the RealTerm serial server
      dhCallMethod(objRealTerm, APV_REALTERM_SERIAL_SERVER_COMMAND, apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_BAUD_INDEX]);
      dhCallMethod(objRealTerm, APV_REALTERM_SERIAL_SERVER_COMMAND, apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_PORT_INDEX]);
      dhCallMethod(objRealTerm, APV_REALTERM_SERIAL_SERVER_COMMAND, apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_TAB_INDEX]);
      //dhCallMethod(objRealTerm, APV_REALTERM_SERIAL_SERVER_COMMAND, apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_SENDFNAME_INDEX]);
      dhCallMethod(objRealTerm, APV_REALTERM_SERIAL_SERVER_COMMAND, apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_CHARDLY_INDEX]);
      dhCallMethod(objRealTerm, APV_REALTERM_SERIAL_SERVER_COMMAND, apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_SENDREP_INDEX]);
      dhCallMethod(objRealTerm, APV_REALTERM_SERIAL_SERVER_COMMAND, apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_CLOSED_INDEX]);
      //dhCallMethod(objRealTerm, APV_REALTERM_SERIAL_SERVER_COMMAND, apvRealTermCommands[APV_REALTERM_COMMAND_PREFIX_SENDFILE_INDEX]);
      //dhGetValue(L"%s", &szResponse, objRealTerm, L".ResponseText");
     
      //printf("Response:\n%s\n", szResponse);
      //dhFreeString(szResponse);
     
      /******************************************************************************/
      /* Execute the APV control via the primary serial port                        */
      /******************************************************************************/

      apvMessageControl = apvMessagingControlInterface();

      /******************************************************************************/
     
      // Release the COM/ActiveX object
      SAFE_RELEASE(objRealTerm);
      dhUninitialize(TRUE); 

      // Tell the RealTerm process to stop
      TerminateProcess(apvRealTermProcessInformation.hProcess,
                       APV_REALTERM_PROESS_EXIT_CODE);
                       
      // Wait until the RealTerm process exits
      WaitForSingleObject( apvRealTermProcessInformation.hProcess, INFINITE );

      // Close the RealTerm process and thread handles
      CloseHandle( apvRealTermProcessInformation.hProcess );
      CloseHandle( apvRealTermProcessInformation.hThread );
      }
    }

/******************************************************************************/

  return(0);

/******************************************************************************/
  } /* end of main                                                            */

/******************************************************************************/
/* ApvCAndCApplicationCommandLine() :                                         */
/*                                                                            */
/* - prints out the expected form of the application command-line             */
/*                                                                            */
/******************************************************************************/

static void ApvCAndCApplicationCommandLine(void)
  {
/******************************************************************************/

  uint8_t fieldIndex  = 0;

/******************************************************************************/

  system("cls");

  printf("\n Usage :");
  printf("\n -----");
  printf("\n ApvCommandAndControlSerialInterface ");
  printf("\n   <full-path-to-the-serial-server> e.g. c:\\directory\\");
  printf("\n   <full-name-of-the-serial-server> e.g. name.ext");
  printf("\n   <possible-symbol-rate> : [");

  for (fieldIndex = 0; fieldIndex < APV_BAUDS_SET; fieldIndex++)
    {
    printf(" %s ", apvSymbolRates[fieldIndex]);

    if (fieldIndex != (APV_BAUDS_SET - 1))
      {
      printf("|");
      }
    }
  
  printf("]");

  printf("\n   <possible COM/USB serial port : this is not checked!");
  printf("\n   <send-file-name>              : name of a file to send e.g <sendfile>.<ext>");
  printf("\n   <send-delay>                  : inter-character send delay in ms");
  printf("\n   <send-repeat>                 : number of times a file should be sent. '0' == forever");
  printf("\n   <port-state>                  : [ 1 == CLOSED | 0 == OPEN ]");
  
  printf("\n\n");

/******************************************************************************/
  } /* end of ApvCAndCApplicationCommandLine                                  */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
