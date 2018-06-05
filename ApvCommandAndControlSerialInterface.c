/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvCommandAndControlSerialInterface.c                                      */
/* 04.06.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - send and receive commands and responses to/from the APV controller and   */
/*   slave(s)                                                                 */
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
#include "Windows.h"
#include "ActiveXInC.h"
#include "ApvMessages.h"
#include "ApvCommandAndControlSerialInterface.h"
#include "ApvCommandAndControlSerialInterfaceMain.h"

/******************************************************************************/
/* Global Variables :                                                         */
/******************************************************************************/

const apvScreenDescription_t apvMenuBackDrop[] = 
  {
    {
    APV_MENU_BACKDROP_TITLE,
    APV_MENU_BACKDROP_TITLE_X_OFFSET,
    APV_MENU_BACKDROP_TITLE_Y_OFFSET,
    APV_MESSAGING_SCREEN_OBJECT_X_ARC_START_ABSOLUTE,
    APV_MESSAGING_SCREEN_OBJECT_Y_ARC_START_ABSOLUTE,

    },
    {
    APV_MENU_BACKDROP_TITLE_UNDERSCORE,
    APV_MENU_BACKDROP_TITLE_UNDERSCORE_X_OFFSET,
    APV_MENU_BACKDROP_TITLE_UNDERSCORE_Y_OFFSET,
    APV_MESSAGING_SCREEN_OBJECT_X_ARC_START_ABSOLUTE,
    APV_MESSAGING_SCREEN_OBJECT_Y_ARC_START_ABSOLUTE,
    },
    {
    APV_STRING_EOS // a screen description is a variable-length list ending in a print
                   // object of <end-of-string> to detect the list-end
    }
  };

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvMessagingControlInterface() :                                           */
/*  <-- : messagingControlError : error codes                                 */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvMessagingControlInterface(void)
  {
/******************************************************************************/

  APV_ERROR_CODE messagingControlError = APV_ERROR_CODE_NONE;

  HANDLE         messagingStdIn[APV_MESSAGING_CONSOLE_SCREEN_BUFFERS]                = { 0, 0, 0 },
                 messagingStdOut[APV_MESSAGING_CONSOLE_SCREEN_BUFFERS]               = { 0, 0, 0 },
                 messagingStdErr[APV_MESSAGING_CONSOLE_SCREEN_BUFFERS]               = { 0, 0, 0 },
                 
                 messagingConsoleScreenBuffers[APV_MESSAGING_CONSOLE_SCREEN_BUFFERS] = { 0, 0, 0 };

/******************************************************************************/

  system("cls");

  /******************************************************************************/
  /* Get the console handles associated with the default command processor      */
  /******************************************************************************/

  messagingStdIn[APV_MESSAGING_CONSOLE_SCREEN_BUFFER_0]  = GetStdHandle(STD_INPUT_HANDLE);
  messagingStdOut[APV_MESSAGING_CONSOLE_SCREEN_BUFFER_0] = GetStdHandle(STD_OUTPUT_HANDLE);
  messagingStdErr[APV_MESSAGING_CONSOLE_SCREEN_BUFFER_0] = GetStdHandle(STD_ERROR_HANDLE);

/******************************************************************************/

  messagingControlError = apvCreateMenuBackDrop( messagingStdOut[APV_MESSAGING_CONSOLE_SCREEN_BUFFER_0],
                                                (const apvScreenDescription_t *)&apvMenuBackDrop[0],
                                                 APV_MENU_BACKDROP_START_X,
                                                 APV_MENU_BACKDROP_START_Y,
                                                 (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY),
                                                 BACKGROUND_BLUE);

  while (!_kbhit())
    ;

/******************************************************************************/

  return(messagingControlError);

/******************************************************************************/
  } /* end of apvMessagingControlInterface                                    */

/******************************************************************************/
/* apvCreateMenuBackDrop() :                                                  */
/*  --> menuStdOut     : console handle of the current console buffer's       */
/*                       'stdout'                                             */
/*  --> menuComponents : list of menu strings and their screen positions      */
/*                                                                            */
/* - draw the APV command and control backdrop onto the selected screen       */
/*   buffer                                                                   */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvCreateMenuBackDrop(HANDLE                  apvMenuStdOut,
                                     apvScreenDescription_t *apvMenuComponents,
                                     uint32_t                apvMenuStartX,
                                     uint32_t                apvMenuStartY,
                                     uint8_t                 apvMenuCharacterBackgroundColour,
                                     uint8_t                 apvMenuCharacterForegroundColour)
  {
/******************************************************************************/

  APV_ERROR_CODE             menuError          = APV_ERROR_CODE_NONE;
                             
  uint32_t                   stylusX            = apvMenuStartX,
                             stylusY            = apvMenuStartY,
                             stylusObjectLength = 0; // the last object length is used to compute drawing 
                                                     // positions based on the last object final character

  CONSOLE_SCREEN_BUFFER_INFO menuBackDropDescription;
  COORD                      menuBackDropWindowBufferSize,
                             menuObjectOutputLocation;

  SMALL_RECT                 menuBackDropWindowSize;

  DWORD                      menuObjectCharactersWritten = 0;

#ifdef UNICODE
  wchar_t                    menuBackDropWideObject[APV_SCREEN_OBJECT_MAXIMUM_LENGTH];
  uint8_t                    menuObjectSize              = 0;
#endif

/******************************************************************************/

  if (GetConsoleScreenBufferInfo( apvMenuStdOut,
                                 &menuBackDropDescription) != true)
    {
    menuError = APV_ERROR_CODE_CONFIGURATION_ERROR;
    }
  else
    {
    // Reset the stylus initial position to the upper left as returned by the information call
    stylusX = menuBackDropDescription.dwCursorPosition.X;
    stylusY = menuBackDropDescription.dwCursorPosition.Y;

    // Resize the window
    menuBackDropWindowBufferSize.X = APV_MENU_BACKDROP_MAXIMUM_X + 1; // the screen buffer must be bigger than the enclosed window!
    menuBackDropWindowBufferSize.Y = APV_MENU_BACKDROP_MAXIMUM_Y + 1;
      
    menuBackDropWindowSize.Top    = APV_MENU_BACKDROP_START_Y;
    menuBackDropWindowSize.Left   = APV_MENU_BACKDROP_START_X;
    menuBackDropWindowSize.Bottom = APV_MENU_BACKDROP_MAXIMUM_Y;
    menuBackDropWindowSize.Right  = APV_MENU_BACKDROP_MAXIMUM_X;

    if (SetConsoleScreenBufferSize(apvMenuStdOut,
                                   menuBackDropWindowBufferSize) != true)
      {
      menuError = APV_ERROR_CODE_CONFIGURATION_ERROR;
      }
    else
      {
      if (SetConsoleWindowInfo( apvMenuStdOut,
                                TRUE,
                               &menuBackDropWindowSize) != true)
        {
        menuError = APV_ERROR_CODE_CONFIGURATION_ERROR;
        }
      else
        {
        if (SetConsoleTextAttribute(apvMenuStdOut,
                                    (apvMenuCharacterBackgroundColour | apvMenuCharacterForegroundColour)) != true)
          {
          menuError = APV_ERROR_CODE_CONFIGURATION_ERROR;
          }
        else
          {
          // Clear to the new background colour
          system("cls");

          /******************************************************************************/
          /* Create the menu backdrop :                                                 */
          /******************************************************************************/

          // Position the stylus according to the objects' 'arc'
          while (apvMenuComponents->apvScreenObject[0] != APV_STRING_EOS)
            {
            switch(apvMenuComponents->apvScreenObjectArcX)
              {
              case APV_MESSAGING_SCREEN_OBJECT_X_ARC_END_ABSOLUTE   : stylusX = apvMenuComponents->apvScreenObjectXOffset + strlen((const char *)&apvMenuComponents->apvScreenObject[0]);
                                                                      break;

              case APV_MESSAGING_SCREEN_OBJECT_X_ARC_END_RELATIVE   : stylusX = stylusX + apvMenuComponents->apvScreenObjectXOffset + strlen((const char *)&apvMenuComponents->apvScreenObject[0]);
                                                                      break;

              case APV_MESSAGING_SCREEN_OBJECT_X_ARC_START_RELATIVE : stylusX = stylusX + apvMenuComponents->apvScreenObjectXOffset;
                                                                      break;

              default                                               :
              case APV_MESSAGING_SCREEN_OBJECT_X_ARC_START_ABSOLUTE : stylusX = apvMenuComponents->apvScreenObjectXOffset;
                                                                      break;
              }

            switch(apvMenuComponents->apvScreenObjectArcX)
              {
              case APV_MESSAGING_SCREEN_OBJECT_Y_ARC_END_ABSOLUTE   : stylusY = apvMenuComponents->apvScreenObjectYOffset + strlen((const char *)&apvMenuComponents->apvScreenObject[0]);
                                                                      break;

              case APV_MESSAGING_SCREEN_OBJECT_Y_ARC_END_RELATIVE   : stylusY = stylusY + apvMenuComponents->apvScreenObjectYOffset + strlen((const char *)&apvMenuComponents->apvScreenObject[0]);
                                                                      break;

              case APV_MESSAGING_SCREEN_OBJECT_Y_ARC_START_RELATIVE : stylusY = stylusY + apvMenuComponents->apvScreenObjectYOffset;
                                                                      break;

              default                                               :
              case APV_MESSAGING_SCREEN_OBJECT_Y_ARC_START_ABSOLUTE : stylusY = apvMenuComponents->apvScreenObjectYOffset;
                                                                      break;
              }


            // Write the menu object to the screen buffer if there is room
            if (((stylusX + strlen((const char *)&apvMenuComponents->apvScreenObject[0])) < menuBackDropWindowSize.Right) &&
                ((stylusY + strlen((const char *)&apvMenuComponents->apvScreenObject[0])) < menuBackDropWindowSize.Bottom))
              {
              menuObjectOutputLocation.X = stylusX;
              menuObjectOutputLocation.Y = stylusY;

#ifdef UNICODE
              for (menuObjectSize = 0; menuObjectSize < strlen((const char *)&apvMenuComponents->apvScreenObject[0]); menuObjectSize++)
                {
                menuBackDropWideObject[menuObjectSize] = (wchar_t)apvMenuComponents->apvScreenObject[menuObjectSize];
                }
#endif

              WriteConsoleOutputCharacter( apvMenuStdOut,
#ifdef UNICODE
                                          (LPCTSTR)&menuBackDropWideObject[0],
#else
                                          (LPCTSTR)&apvMenuComponents->apvScreenObject[0],
#endif
                                           strlen((const char *)&apvMenuComponents->apvScreenObject[0]),
                                           menuObjectOutputLocation,
                                          &menuObjectCharactersWritten);
              }

            // Iterate over the whole menu backdrop definition
            apvMenuComponents = apvMenuComponents + 1;
            }
          }
        }
      }
    }

/******************************************************************************/

  return(menuError);

/******************************************************************************/
  } /* end of apvCreateMenuBackDrop                                           */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
