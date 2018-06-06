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
/* The "apvMenuBackDrop[]" table describes the layout and possible actions of */
/* the menu items. When the edit/select cursor is enabled it is placed at the */
/* first editable/selectable option so it is worth noting the table is NEVER  */
/* SORTED in the code, so to make the menu look coherent the entries should   */
/* be placed in the order in which the cursor should appear on the console!   */
/* The last entry in a table MUST always be an empty entry with the entry     */
/* title set to the empty string - this acts as the "end-of-table" marker. By */
/* definition this cannot be tested for so do not forget it! At LEAST one     */
/* entry MUST allow the menu to be exitted!                                   */
/******************************************************************************/

const apvScreenDescription_t apvMenuBackDrop[] = 
  {
    {
    &apvMenuBackDrop[APV_MENU_OPTION_TITLE_UNDERSCORE_INDEX],
    &apvMenuBackDrop[APV_MENU_OPTION_TERMINATOR_INDEX],
     APV_MENU_BACKDROP_TITLE,
     APV_MENU_BACKDROP_TITLE_X_OFFSET,
     APV_MENU_BACKDROP_TITLE_Y_OFFSET,
     APV_MESSAGING_SCREEN_OBJECT_X_ARC_START_ABSOLUTE,
     APV_MESSAGING_SCREEN_OBJECT_Y_ARC_START_ABSOLUTE,
     false,
     (void *)NULL,
     NULL
    },
    {
    &apvMenuBackDrop[APV_MENU_OPTION_SIGN_ON_INDEX],
    &apvMenuBackDrop[APV_MENU_OPTION_TITLE_INDEX],
     APV_MENU_BACKDROP_TITLE_UNDERSCORE,
     APV_MENU_BACKDROP_TITLE_UNDERSCORE_X_OFFSET,
     APV_MENU_BACKDROP_TITLE_UNDERSCORE_Y_OFFSET,
     APV_MESSAGING_SCREEN_OBJECT_X_ARC_START_ABSOLUTE,
     APV_MESSAGING_SCREEN_OBJECT_Y_ARC_START_ABSOLUTE,
     false,
     (void *)NULL,
     NULL
    },
    {
    &apvMenuBackDrop[APV_MENU_OPTION_EXIT_INDEX],
    &apvMenuBackDrop[APV_MENU_OPTION_TITLE_UNDERSCORE_INDEX],
     APV_MENU_BACKDROP_REQUEST_SIGN_ON,
     APV_MENU_BACKDROP_REQUEST_SIGN_ON_X_OFFSET,
     APV_MENU_BACKDROP_REQUEST_SIGN_ON_Y_OFFSET,
     APV_MESSAGING_SCREEN_OBJECT_X_ARC_START_ABSOLUTE,
     APV_MESSAGING_SCREEN_OBJECT_Y_ARC_START_ABSOLUTE,
     true,
     (void *)NULL,
     apvMenuOptionRequestArduinoSignOn
    },
    {
    &apvMenuBackDrop[APV_MENU_OPTION_TERMINATOR_INDEX],
    &apvMenuBackDrop[APV_MENU_OPTION_SIGN_ON_INDEX],
     APV_MENU_BACKDROP_EXIT_OPTION,
     APV_MENU_BACKDROP_EXIT_OPTION_X_OFFSET,
     APV_MENU_BACKDROP_EXIT_OPTION_Y_OFFSET,
     APV_MESSAGING_SCREEN_OBJECT_X_ARC_START_ABSOLUTE,
     APV_MESSAGING_SCREEN_OBJECT_Y_ARC_START_ABSOLUTE,
     true,
     (void *)true, // signal the menu to exit
     apvMenuOptionExitOption
    },
    { // Screen descriptor MANDATORY array terminator
    &apvMenuBackDrop[APV_MENU_OPTION_TITLE_INDEX],
    &apvMenuBackDrop[APV_MENU_OPTION_EXIT_INDEX],
     APV_STRING_EOS, // a screen description is a variable-length list ending in a print
                     // object of <end-of-string> to detect the list-end
     0,
     0,
     APV_MESSAGING_SCREEN_OBJECT_X_ARC_START_ABSOLUTE,
     APV_MESSAGING_SCREEN_OBJECT_Y_ARC_START_ABSOLUTE,
     false           // this field is needed when moving around the menu
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
                                                 (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY),
                                                 BACKGROUND_BLUE);

  messagingControlError = apvExecuteMenuOptions( messagingStdOut[APV_MESSAGING_CONSOLE_SCREEN_BUFFER_0],
                                                 messagingStdIn[APV_MESSAGING_CONSOLE_SCREEN_BUFFER_0],
                                                (const apvScreenDescription_t *)&apvMenuBackDrop[0]);

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
/*  --> apvMenuCharacterBackgroundColour : console background colour [1]      */
/*  --> apvMenuCharacterForegroundColour : console character/forground colour */
/*                                                                            */
/* - draw the APV command and control backdrop onto the selected screen       */
/*   buffer                                                                   */
/*                                                                            */
/* [1] : see "Character Attributes" Ref: "https://docs.microsoft.com/en-us/   */
/*       windows/console/console-screen-buffers" accessed 08.06.18 @ 12:00    */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvCreateMenuBackDrop(      HANDLE                  apvMenuStdOut,
                                     const apvScreenDescription_t *apvMenuComponents,
                                           uint8_t                 apvMenuCharacterBackgroundColour,
                                           uint8_t                 apvMenuCharacterForegroundColour)
  {
/******************************************************************************/

  APV_ERROR_CODE             menuError          = APV_ERROR_CODE_NONE;
                             
  uint32_t                   stylusX            = 0,
                             stylusY            = 0,
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
                 (stylusY < menuBackDropWindowSize.Bottom))
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
/* apvExecuteMenuOptions() :                                                  */
/*  --> menuStdOut     : console handle of the current console buffer's       */
/*                       'stdout'                                             */
/*  --> menuStdIn      : console handle of the current console buffer's       */
/*                       'stdin'                                              */
/*  --> menuComponents : list of menu strings and their screen positions      */
/*                                                                            */
/* - allow the selection cursor to move around the input-enabled menu options */
/*   and on selection execute an operation                                    */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvExecuteMenuOptions(HANDLE                        apvMenuStdOut,
                                     HANDLE                        apvMenuStdIn,
                                     const apvScreenDescription_t *apvMenuComponents)
  {
/******************************************************************************/

  APV_ERROR_CODE          executeMenuError     = APV_ERROR_CODE_NONE;
                
  apvScreenDescription_t *apvMenuComponents_p  = (apvScreenDescription_t *)apvMenuComponents;

  uint32_t                stylusX              = APV_STYLUS_UNPLACED,
                          stylusY              = APV_STYLUS_UNPLACED,
                          stylusObjectLength   = 0; // the last object length is used to compute drawing 
                                                   // positions based on the last object final character
                          
  CONSOLE_CURSOR_INFO     cursorDefinition;
  COORD                   cursorPosition;
  INPUT_RECORD            cursorInputEvent[APV_MENU_INPUT_PEEK_BUFFERS];
  INPUT_RECORD            cursorInputEventsRead[APV_MENU_INPUT_PEEK_BUFFERS];

  DWORD                   cursorInputEventsStored    = 0,
                          cursorInputEventsAvailable = 0;
  uint8_t                 cursorInputEventIndex      = 0,
                          readConsoleInputIndex      = 0;
  WORD                    cursorInputIdentifiedEvent = 0;

  bool                    menuExitOption             = false;

/******************************************************************************/

  if (apvMenuComponents == NULL)
    {
    executeMenuError = APV_ERROR_CODE_CONFIGURATION_ERROR;
    }
  else
    {
    // Place the selection cursor on the first entry that allows input
    while (apvMenuComponents_p->apvScreenObject[0] != APV_STRING_EOS)
      {
      if (apvMenuComponents_p->apvScreenObjectCursorTarget == true)
        {
        stylusX = apvMenuComponents_p->apvScreenObjectXOffset + strlen((const char *)&apvMenuComponents_p->apvScreenObject[0]) + 1; // the stylus/cursor will be placed just clear of the entry text
        stylusY = apvMenuComponents_p->apvScreenObjectYOffset;

        break;
        }

      apvMenuComponents_p = apvMenuComponents_p + 1;
      }

    if ((stylusX == APV_STYLUS_UNPLACED) || (stylusY == APV_STYLUS_UNPLACED))
      { // This menu description is faulty, no entries can be edited or selected!
      executeMenuError = APV_ERROR_CODE_CONFIGURATION_ERROR;
      }
    else
      {
      // Define an underline cursor
      cursorDefinition.dwSize   = APV_MENU_CURSOR_PERCENTAGE_CELL_SIZE;
      cursorDefinition.bVisible = true;

      if (SetConsoleCursorInfo(apvMenuStdOut,
                               (const CONSOLE_CURSOR_INFO *)&cursorDefinition) != true)
        {
        executeMenuError = APV_ERROR_CODE_CONFIGURATION_ERROR;
        }
      else
        {
        cursorPosition.X = stylusX;
        cursorPosition.Y = stylusY;

        SetConsoleCursorPosition(apvMenuStdOut,
                                 cursorPosition);
        }

      // Loop around the editable/selectable menu items
      while (menuExitOption == false)
        {
        // Look for navigation or edit/selection keypresses, mouse operations (collectively "events")
        if (PeekConsoleInput( apvMenuStdIn,
                             (PINPUT_RECORD)&cursorInputEvent[0],
                              APV_MENU_INPUT_PEEK_BUFFERS,
                             &cursorInputEventsAvailable) == true)
          {
          if (cursorInputEventsAvailable != 0)
            { // Some sort of input has occurred
            for (cursorInputEventIndex = 0; cursorInputEventIndex < cursorInputEventsAvailable; cursorInputEventIndex++)
              {
              switch (cursorInputEvent[cursorInputEventIndex].EventType)
                {
                case FOCUS_EVENT               : 
                case KEY_EVENT                 : 
                case MENU_EVENT                : 
                case MOUSE_EVENT               :
                case WINDOW_BUFFER_SIZE_EVENT  :
                default                        : cursorInputIdentifiedEvent = cursorInputEvent[cursorInputEventIndex].EventType;
                                                 break;
                }

              if (cursorInputIdentifiedEvent == KEY_EVENT)
                { // A keypress at the input location has been detected : navigation, selection or execution ?
                  // Note that any keypress code appears TWICE : press and release ?
                ReadConsoleInput(  apvMenuStdIn,
                                 (PINPUT_RECORD)&cursorInputEventsRead[0],
                                  APV_MENU_INPUT_PEEK_BUFFERS,
                                 &cursorInputEventsStored);

                /******************************************************************************/
                /* Only action the keypress on release                                        */
                /******************************************************************************/

                if (cursorInputEventsRead[0].Event.KeyEvent.bKeyDown == false)
                  {
                  switch(cursorInputEventsRead[0].Event.KeyEvent.wVirtualKeyCode)
                    {
                    case APV_MENU_OPTION_SELECTION_FORWARD_KEY  : do
                                                                    {
                                                                    apvMenuComponents_p = apvMenuComponents_p->nextScreenObject;
                                                                    }
                                                                  while (apvMenuComponents_p->apvScreenObjectCursorTarget == false);
                
                                                                  stylusX = apvMenuComponents_p->apvScreenObjectXOffset + strlen((const char *)&apvMenuComponents_p->apvScreenObject) + 1; // the stylus/cursor will be placed just clear of the entry text
                                                                  stylusY = apvMenuComponents_p->apvScreenObjectYOffset;
                
                                                                  cursorPosition.X = stylusX;
                                                                  cursorPosition.Y = stylusY;
                                                           
                                                                  SetConsoleCursorPosition(apvMenuStdOut,
                                                                                           cursorPosition);
                
                                                                  break;
                
                    case  APV_MENU_OPTION_SELECTION_REVERSE_KEY : do
                                                                    {
                                                                    apvMenuComponents_p = apvMenuComponents_p->nextScreenObject;
                                                                    }
                                                                  while (apvMenuComponents_p->apvScreenObjectCursorTarget == false);
                
                                                                  stylusX = apvMenuComponents_p->apvScreenObjectXOffset + strlen((const char *)&apvMenuComponents_p->apvScreenObject) + 1; // the stylus/cursor will be placed just clear of the entry text
                                                                  stylusY = apvMenuComponents_p->apvScreenObjectYOffset;
                
                                                                  cursorPosition.X = stylusX;
                                                                  cursorPosition.Y = stylusY;
                                                           
                                                                  SetConsoleCursorPosition(apvMenuStdOut,
                                                                                           cursorPosition);
                
                                                                  break;
                
                    case APV_MENU_SELECTION_EXECUTE_KEY :         menuExitOption = (bool)((uint32_t *)(apvMenuComponents_p->apvScreenObjectExecutionTarget(apvMenuComponents_p->apvMenuSpecificOption)));

                                                                  break;

                    default                                     : break;
                    }
                  }
                }
              else
                { // For now, discard any events that are not keyboard-originated
                ReadConsoleInput(  apvMenuStdIn,
                                 (PINPUT_RECORD)&cursorInputEventsRead[0],
                                  APV_MENU_INPUT_PEEK_BUFFERS,
                                 &cursorInputEventsStored);
                }
              }
            }
          }
        }
      }
    }

/******************************************************************************/

  return(executeMenuError);

/******************************************************************************/
  } /* end of apvExecuteMenuOptions                                           */

/******************************************************************************/
/* Menu option functions :                                                    */
/******************************************************************************/
/* apvMenuOptionRequestArduinoSignOn() :                                      */
/*  --> menuSpecificOption : the received value can be cast from the 'void *' */
/*      used as a neutral value carrier                                       */
/*  <-- : the returned value can be cast from the 'void *' used as a neutral  */
/*        value carrier                                                       */
/*                                                                            */
/* - call the local Arduino (via the primary serial port) and request it to   */
/*   respond with the sign-on message                                         */
/*                                                                            */
/******************************************************************************/

void *apvMenuOptionRequestArduinoSignOn(void *menuSpecificOption)
  {
/******************************************************************************/

  return(menuSpecificOption);

/******************************************************************************/
  } /* end of apvMenuOptionRequestArduinoSignOn                               */

/******************************************************************************/
/* apvMenuOptionExitOption() :                                                */
/*  --> menuSpecificOption : the received value can be cast from the 'void *' */
/*      used as a neutral value carrier                                       */
/*  <-- : the returned value can be cast from the 'void *' used as a neutral  */
/*        value carrier                                                       */
/* - execute any actions to be completed when exitting a menu                 */
/*                                                                            */
/******************************************************************************/

void *apvMenuOptionExitOption(void *menuSpecificOption)
  {
/******************************************************************************/

  // Signal the menu to exit
  return(menuSpecificOption);

/******************************************************************************/
  } /* end of apvMenuOptionExitOption                                         */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
