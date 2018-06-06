/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvCommandAndControlSerialInterface.h                                      */
/* 04.06.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - send and receive commands and responses to/from the APV controller and   */
/*   slave(s)                                                                 */
/*                                                                            */
/******************************************************************************/

#ifndef _APV_COMMAND_AND_CONTROL_SERIAL_INTERFACE_H_
#define _APV_COMMAND_AND_CONTROL_SERIAL_INTERFACE_H_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include "Windows.h"
#include "ApvError.h"
#ifdef WIN32_BOOL
#include "stdbool.h"
#else
#include <stdbool.h>
#endif

/******************************************************************************/
/* Constants :                                                                */
/******************************************************************************/

#define APV_SCREEN_OBJECT_MAXIMUM_LENGTH (32) // all screen strings must be 31 characters or less

/******************************************************************************/
/* The main command and control menu backdrop description :                   */
/******************************************************************************/

#define APV_MENU_BACKDROP_START_X                    (0)
#define APV_MENU_BACKDROP_START_Y                    (0)

#define APV_MENU_BACKDROP_MAXIMUM_X                  (120)
#define APV_MENU_BACKDROP_MAXIMUM_Y                   (60)

#define APV_MENU_BACKDROP_TITLE_X_OFFSET             (1)
#define APV_MENU_BACKDROP_TITLE_Y_OFFSET             (1)
#define APV_MENU_BACKDROP_TITLE                     "Apv Command Interface : "

#define APV_MENU_BACKDROP_TITLE_UNDERSCORE_X_OFFSET  (1)
#define APV_MENU_BACKDROP_TITLE_UNDERSCORE_Y_OFFSET  (2)
#define APV_MENU_BACKDROP_TITLE_UNDERSCORE          "---------------------"

#define APV_MENU_BACKDROP_REQUEST_SIGN_ON_X_OFFSET   (3)
#define APV_MENU_BACKDROP_REQUEST_SIGN_ON_Y_OFFSET   (4)
#define APV_MENU_BACKDROP_REQUEST_SIGN_ON           "Arduino Sign-On : "

#define APV_MENU_BACKDROP_EXIT_OPTION               "Exit this menu : "
#define APV_MENU_BACKDROP_EXIT_OPTION_X_OFFSET      (3)
#define APV_MENU_BACKDROP_EXIT_OPTION_Y_OFFSET      (APV_MENU_BACKDROP_MAXIMUM_Y - 2)

/******************************************************************************/
/* The main command and control menu option selection  :                      */
/******************************************************************************/

#define APV_STYLUS_UNPLACED                        ((uint32_t)0xffffffff) // this value indicates the stylus/cursor is/has become indeterminate

#define APV_MENU_OPTION_SELECTION_FORWARD_KEY      (VK_DOWN)
#define APV_MENU_OPTION_SELECTION_REVERSE_KEY      (VK_UP)
#define APV_MENU_SELECTION_EXECUTE_KEY             (VK_RETURN)

#define APV_MENU_CURSOR_PERCENTAGE_CELL_SIZE       (1)

#define APV_MENU_INPUT_PEEK_BUFFERS                32 // the menu input function can examine up to 32 "events" in the console window buffer - these 
                                                      // "events" are not removed from the keyboard buffer until a later blocking call

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/
/* The default and two further screen buffers can be used for "seamless"      */
/* drawing on a hidden console window buffer                                  */
/******************************************************************************/

typedef enum apvMessagingScreenBuffers_tTag
  {
  APV_MESSAGING_CONSOLE_SCREEN_BUFFER_DEFAULT = 0,
  APV_MESSAGING_CONSOLE_SCREEN_BUFFER_0       = 1,
  APV_MESSAGING_CONSOLE_SCREEN_BUFFER_1       = 2,
  APV_MESSAGING_CONSOLE_SCREEN_BUFFERS
  } apvMessagingScreenBuffers_t;

/******************************************************************************/
// The screen object "arc" tells the 'stylus' to draw absolute or relative to */
// the 'starting' (first character) or 'ending' (last character) position of  */
/* the last object                                                            */
/******************************************************************************/

typedef enum apvMessagingScreenObjectArc_tTag
  {
  APV_MESSAGING_SCREEN_OBJECT_X_ARC_START_ABSOLUTE = 0,
  APV_MESSAGING_SCREEN_OBJECT_Y_ARC_START_ABSOLUTE,
  APV_MESSAGING_SCREEN_OBJECT_X_ARC_END_ABSOLUTE,
  APV_MESSAGING_SCREEN_OBJECT_Y_ARC_END_ABSOLUTE,
  APV_MESSAGING_SCREEN_OBJECT_X_ARC_START_RELATIVE, 
  APV_MESSAGING_SCREEN_OBJECT_Y_ARC_START_RELATIVE,
  APV_MESSAGING_SCREEN_OBJECT_X_ARC_END_RELATIVE,
  APV_MESSAGING_SCREEN_OBJECT_Y_ARC_END_RELATIVE,
  APV_MESSAGING_SCREEN_OBJECT_ARCS
  } apvMessagingScreenObjectArc_t;

/******************************************************************************/
/* For the linked-list of screen descriptions it is useful to iterate each    */
/* screen option structure                                                    */
/******************************************************************************/

typedef enum apvCommandMenuOption_tTag
  {
  APV_MENU_OPTION_TITLE_INDEX = 0,
  APV_MENU_OPTION_TITLE_UNDERSCORE_INDEX,
  APV_MENU_OPTION_SIGN_ON_INDEX,
  APV_MENU_OPTION_EXIT_INDEX,
  APV_MENU_OPTION_TERMINATOR_INDEX,
  APV_MENU_OPTION_INDICES
  } apvCommandMenuOption_t;

/******************************************************************************/
/* The linked-list of menu option descriptions                                */
/******************************************************************************/

typedef struct apvScreenDescription_tTag
  {
  const struct apvScreenDescription_tTag *nextScreenObject;
  const struct apvScreenDescription_tTag *lastScreenObject;
  char                                    apvScreenObject[APV_SCREEN_OBJECT_MAXIMUM_LENGTH];
  uint32_t                                apvScreenObjectXOffset;
  uint32_t                                apvScreenObjectYOffset;
  apvMessagingScreenObjectArc_t           apvScreenObjectArcX;
  apvMessagingScreenObjectArc_t           apvScreenObjectArcY;
  bool                                    apvScreenObjectCursorTarget;                                // can the user cursor visit this location ?
  void                                   *apvMenuSpecificOption;                                      // an option used to drive further execution or condition the return value
  void                                   *(*apvScreenObjectExecutionTarget)(void *menuSpeificOption); // on selection execute this menu option
  } apvScreenDescription_t;

/******************************************************************************/
/* Console keypresses are counted twice, at key-press and key-release         */
/******************************************************************************/

typedef enum apvMenuReadConsoleInputOccurences_tTag
  {
  APV_MENU_READ_CURSOR_INPUT_OCCURENCE_KEY_PRESSED = 0,
  APV_MENU_READ_CURSOR_INPUT_OCCURENCE_KEY_RELEASED,
  APV_MENU_READ_CURSOR_INPUT_OCCURENCES
  } apvMenuReadConsoleInputOccurences_tT;

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE  apvMessagingControlInterface(void);
extern APV_ERROR_CODE  apvCreateMenuBackDrop(HANDLE                        apvMenuStdOut,
                                            const apvScreenDescription_t *apvMenuComponents,
                                            uint8_t                       apvMenuCharacterBackgroundColour,
                                            uint8_t                       apvMenuCharacterForegroundColour);
extern APV_ERROR_CODE  apvExecuteMenuOptions(HANDLE                        apvMenuStdOut,
                                            HANDLE                        apvMenuStdIn,
                                            const apvScreenDescription_t *apvMenuComponents);
extern void           *apvMenuOptionRequestArduinoSignOn(void);
extern void           *apvMenuOptionExitOption(void);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/