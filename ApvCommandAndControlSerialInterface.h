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

#include "ApvError.h"

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

typedef struct apvScreenDescription_tTag
  {
  char                          apvScreenObject[APV_SCREEN_OBJECT_MAXIMUM_LENGTH];
  uint32_t                      apvScreenObjectXOffset;
  uint32_t                      apvScreenObjectYOffset;
  apvMessagingScreenObjectArc_t apvScreenObjectArcX;
  apvMessagingScreenObjectArc_t apvScreenObjectArcY;
  } apvScreenDescription_t;

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE apvMessagingControlInterface(void);
extern APV_ERROR_CODE apvCreateMenuBackDrop(HANDLE                        apvMenuStdOut,
                                            const apvScreenDescription_t *apvMenuComponents,
                                            uint32_t                      apvMenuStartX,
                                            uint32_t                      apvMenuStartY,
                                            uint8_t                       apvMenuCharacterBackgroundColour,
                                            uint8_t                       apvMenuCharacterForegroundColour);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/