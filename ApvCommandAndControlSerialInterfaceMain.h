/******************************************************************************/
/*                                                                            */
/* ApvCommandAndControlSerialInterfaceMain.c                                  */
/* 01.06.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/******************************************************************************/

#ifndef _APV_COMMAND_AND_CONTROL_SERIAL_INTERFACE_H_
#define _APV_COMMAND_AND_CONTROL_SERIAL_INTERFACE_H_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/
/******************************************************************************/
/* Constants :                                                                */
/******************************************************************************/

// The RealTerm serial server ActiveX/COM etc.,. object type
#define APV_REALTERM_SERIAL_SERVER_OBJECT_TYPE (L"realterm.realtermintf")

// The RealTerm serial server virtual "command-line" portal
#define APV_REALTERM_SERIAL_SERVER_COMMAND     (L"DoCommands(%s)")

#define APV_REALTERM_PROESS_EXIT_CODE          ((UINT)0)
#define APV_STRING_EOS                         (0)

/******************************************************************************/
/* The possible symbol rates                                                  */
/******************************************************************************/

#define APV_BAUD_9600                     "9600"
#define APV_BAUD_19200                   "19200"
#define APV_BAUD_38400                   "38400"
#define APV_BAUD_57600                   "57600"
#define APV_BAUD_76800                   "76800"
#define APV_BAUD_115200                 "115200"

#define APV_BAUDS_MAXIMUM_SYMBOL_LENGTH       7
#define APV_BAUDS_SET                         6

/******************************************************************************/
/* The RealTerm serial server command prefixes :                              */
/* - reference the command-line table : "https://realterm.sourceforge.io/" :  */
/*   Accessed 01.06.2018 @ 14:10 (European Formatting)                        */
/******************************************************************************/

#define APV_REALTERM_COMMAND_PREFIX_BAUD              "BAUD="
#define APV_REALTERM_COMMAND_PREFIX_PORT              "PORT="
#define APV_REALTERM_COMMAND_PREFIX_PORTQUIT          "PORTQUIT="
#define APV_REALTERM_COMMAND_PREFIX_DATA              "DATA="
#define APV_REALTERM_COMMAND_PREFIX_FRAMESIZE         "FRAMESIZE="
#define APV_REALTERM_COMMAND_PREFIX_CAPFILE           "CAPFILE="
#define APV_REALTERM_COMMAND_PREFIX_CAPCOUNT          "CAPCOUNT="
#define APV_REALTERM_COMMAND_PREFIX_CAPSECS           "CAPSECS="
#define APV_REALTERM_COMMAND_PREFIX_CAPTURE           "CAPTURE="
#define APV_REALTERM_COMMAND_PREFIX_CAPAUTONAME       "CAPAUTONAME="
#define APV_REALTERM_COMMAND_PREFIX_CAPPROCESS        "CAPPROCESS="
#define APV_REALTERM_COMMAND_PREFIX_CAPQUIT           "CAPQUIT="
#define APV_REALTERM_COMMAND_PREFIX_CAPHEX            "CAPHEX="
#define APV_REALTERM_COMMAND_PREFIX_CAPDIRECT         "CAPDIRECT="
#define APV_REALTERM_COMMAND_PREFIX_TSDELIMITER       "TSDELIMITER="
#define APV_REALTERM_COMMAND_PREFIX_TIMESTAMP         "TIMESTAMP="
#define APV_REALTERM_COMMAND_PREFIX_VISIBLE           "VISIBLE="
#define APV_REALTERM_COMMAND_PREFIX_DISPLAY           "DISPLAY="
#define APV_REALTERM_COMMAND_PREFIX_BIGEND            "BIGEND="
#define APV_REALTERM_COMMAND_PREFIX_FLOW              "FLOW="
#define APV_REALTERM_COMMAND_PREFIX_RTS               "RTS="
#define APV_REALTERM_COMMAND_PREFIX_DTR               "DTR="
#define APV_REALTERM_COMMAND_PREFIX_CLOSED            "CLOSED="
#define APV_REALTERM_COMMAND_PREFIX_TAB               "TAB="
#define APV_REALTERM_COMMAND_PREFIX_ECHO              "ECHO="
#define APV_REALTERM_COMMAND_PREFIX_EBAUD             "EBAUD="
#define APV_REALTERM_COMMAND_PREFIX_EDATA             "EDATA="
#define APV_REALTERM_COMMAND_PREFIX_HALF              "HALF="
#define APV_REALTERM_COMMAND_PREFIX_CAPTION           "CAPTION="
#define APV_REALTERM_COMMAND_PREFIX_SENDFILE          "SENDFILE="
#define APV_REALTERM_COMMAND_PREFIX_SENDFNAME         "SENDFNAME="
#define APV_REALTERM_COMMAND_PREFIX_CONTROLS          "CONTROLS="
#define APV_REALTERM_COMMAND_PREFIX_MONITOR           "MONITOR="
#define APV_REALTERM_COMMAND_PREFIX_CHARDLY           "CHARDLY="
#define APV_REALTERM_COMMAND_PREFIX_LINEDLY           "LINEDLY="
#define APV_REALTERM_COMMAND_PREFIX_ROWS              "ROWS="
#define APV_REALTERM_COMMAND_PREFIX_COLS              "COLS="
#define APV_REALTERM_COMMAND_PREFIX_SENDDLY           "SENDDLY="
#define APV_REALTERM_COMMAND_PREFIX_SENDREP           "SENDREP="
#define APV_REALTERM_COMMAND_PREFIX_SENDQUIT          "SENDQUIT="
#define APV_REALTERM_COMMAND_PREFIX_SENDSTR           "SENDSTR="
#define APV_REALTERM_COMMAND_PREFIX_SENDNUM           "SENDNUM="
#define APV_REALTERM_COMMAND_PREFIX_SENDHEX           "SENDHEX="
#define APV_REALTERM_COMMAND_PREFIX_SENDLIT           "SENDLIT="
#define APV_REALTERM_COMMAND_PREFIX_CR                "CR="
#define APV_REALTERM_COMMAND_PREFIX_LF                "LF="
#define APV_REALTERM_COMMAND_PREFIX_FIRST             "FIRST="
#define APV_REALTERM_COMMAND_PREFIX_LFNL              "LFNL="
#define APV_REALTERM_COMMAND_PREFIX_SPY               "SPY="
#define APV_REALTERM_COMMAND_PREFIX_SCANPORTS         "SCANPORTS="
#define APV_REALTERM_COMMAND_PREFIX_I2CADD            "I2CADD="
#define APV_REALTERM_COMMAND_PREFIX_HELP              "HELP="
#define APV_REALTERM_COMMAND_PREFIX_INSTALL           "INSTALL="
#define APV_REALTERM_COMMAND_PREFIX_SCROLLBACK        "SCROLLBACK="
#define APV_REALTERM_COMMAND_PREFIX_COLORS            "COLORS="
#define APV_REALTERM_COMMAND_PREFIX_HEXCSV            "HEXCSV="
#define APV_REALTERM_COMMAND_PREFIX_WINSOCK           "WINSOCK="
#define APV_REALTERM_COMMAND_PREFIX_EWINSOCK          "EWINSOCK="
#define APV_REALTERM_COMMAND_PREFIX_INIFILE           "INIFILE="
#define APV_REALTERM_COMMAND_PREFIX_MSGBOX            "MSGBOX="
#define APV_REALTERM_COMMAND_PREFIX_SCALE             "SCALE="
#define APV_REALTERM_COMMAND_PREFIX_WINDOWSTATE       "WINDOWSTATE="
#define APV_REALTERM_COMMAND_PREFIX_CLEAR             "CLEAR="
#define APV_REALTERM_COMMAND_PREFIX_STRING1           "STRING1="
#define APV_REALTERM_COMMAND_PREFIX_STRING2           "STRING2="
#define APV_REALTERM_COMMAND_PREFIX_CRLF              "CRLF="
#define APV_REALTERM_COMMAND_PREFIX_FONTNAME          "FONTNAME="
#define APV_REALTERM_COMMAND_PREFIX_FONTSIZE          "FONTSIZE="
#define APV_REALTERM_COMMAND_PREFIX_BSYNCIS           "BSYNCIS="
#define APV_REALTERM_COMMAND_PREFIX_BSYNCDAT          "BSYNCDAT="
#define APV_REALTERM_COMMAND_PREFIX_BSYNCAND          "BSYNCAND="
#define APV_REALTERM_COMMAND_PREFIX_BSYNCXOR          "BSYNCXOR="
#define APV_REALTERM_COMMAND_PREFIX_BSYNCHI           "BSYNCHI="
#define APV_REALTERM_COMMAND_PREFIX_BSYNCLEAD         "BSYNCLEAD="
#define APV_REALTERM_COMMAND_PREFIX_BSYNCSHOWCOUNT    "BSYNCSHOWCOUNT="
#define APV_REALTERM_COMMAND_PREFIX_KEYMAPVT          "KEYMAPVT="
#define APV_REALTERM_COMMAND_PREFIX_KEYMAP            "KEYMAP="
#define APV_REALTERM_COMMAND_PREFIX_CRC               "CRC="
#define APV_REALTERM_COMMAND_PREFIX_VERSION           "VERSION="

#define APV_REALTERM_COMMAND_PREFIX_LENGTH            256 // space for the command and it's sub-fields, numbers etc.,.

/******************************************************************************/
/* Tab Sheet Names :                                                          */
/******************************************************************************/

#define APV_REALTERM_TABSHEET_NAME_SEND "Send"

/******************************************************************************/
/* Type Definitions                                                           */
/******************************************************************************/

typedef enum apvRealTermCommandPrefixIndicies_tTag
  {
  APV_REALTERM_COMMAND_PREFIX_BAUD_INDEX = 0,
  APV_REALTERM_COMMAND_PREFIX_PORT_INDEX,
  APV_REALTERM_COMMAND_PREFIX_PORTQUIT_INDEX,
  APV_REALTERM_COMMAND_PREFIX_DATA_INDEX,
  APV_REALTERM_COMMAND_PREFIX_FRAMESIZE_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CAPFILE_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CAPCOUNT_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CAPSECS_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CAPTURE_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CAPAUTONAME_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CAPPROCESS_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CAPQUIT_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CAPHEX_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CAPDIRECT_INDEX,
  APV_REALTERM_COMMAND_PREFIX_TSDELIMITER_INDEX,
  APV_REALTERM_COMMAND_PREFIX_TIMESTAMP_INDEX,
  APV_REALTERM_COMMAND_PREFIX_VISIBLE_INDEX,
  APV_REALTERM_COMMAND_PREFIX_DISPLAY_INDEX,
  APV_REALTERM_COMMAND_PREFIX_BIGEND_INDEX,
  APV_REALTERM_COMMAND_PREFIX_FLOW_INDEX,
  APV_REALTERM_COMMAND_PREFIX_RTS_INDEX,
  APV_REALTERM_COMMAND_PREFIX_DTR_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CLOSED_INDEX,
  APV_REALTERM_COMMAND_PREFIX_TAB_INDEX,
  APV_REALTERM_COMMAND_PREFIX_ECHO_INDEX,
  APV_REALTERM_COMMAND_PREFIX_EBAUD_INDEX,
  APV_REALTERM_COMMAND_PREFIX_EDATA_INDEX,
  APV_REALTERM_COMMAND_PREFIX_HALF_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CAPTION_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SENDFILE_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SENDFNAME_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CONTROLS_INDEX,
  APV_REALTERM_COMMAND_PREFIX_MONITOR_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CHARDLY_INDEX,
  APV_REALTERM_COMMAND_PREFIX_LINEDLY_INDEX,
  APV_REALTERM_COMMAND_PREFIX_ROWS_INDEX,
  APV_REALTERM_COMMAND_PREFIX_COLS_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SENDDLY_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SENDREP_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SENDQUIT_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SENDSTR_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SENDNUM_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SENDHEX_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SENDLIT_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CR_INDEX,
  APV_REALTERM_COMMAND_PREFIX_LF_INDEX,
  APV_REALTERM_COMMAND_PREFIX_FIRST_INDEX,
  APV_REALTERM_COMMAND_PREFIX_LFNL_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SPY_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SCANPORTS_INDEX,
  APV_REALTERM_COMMAND_PREFIX_I2CADD_INDEX,
  APV_REALTERM_COMMAND_PREFIX_HELP_INDEX,
  APV_REALTERM_COMMAND_PREFIX_INSTALL_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SCROLLBACK_INDEX,
  APV_REALTERM_COMMAND_PREFIX_COLORS_INDEX,
  APV_REALTERM_COMMAND_PREFIX_HEXCSV_INDEX,
  APV_REALTERM_COMMAND_PREFIX_WINSOCK_INDEX,
  APV_REALTERM_COMMAND_PREFIX_EWINSOCK_INDEX,
  APV_REALTERM_COMMAND_PREFIX_INIFILE_INDEX,
  APV_REALTERM_COMMAND_PREFIX_MSGBOX_INDEX,
  APV_REALTERM_COMMAND_PREFIX_SCALE_INDEX,
  APV_REALTERM_COMMAND_PREFIX_WINDOWSTATE_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CLEAR_INDEX,
  APV_REALTERM_COMMAND_PREFIX_STRING1_INDEX,
  APV_REALTERM_COMMAND_PREFIX_STRING2_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CRLF_INDEX,
  APV_REALTERM_COMMAND_PREFIX_FONTNAME_INDEX,
  APV_REALTERM_COMMAND_PREFIX_FONTSIZE_INDEX,
  APV_REALTERM_COMMAND_PREFIX_BSYNCIS_INDEX,
  APV_REALTERM_COMMAND_PREFIX_BSYNCDAT_INDEX,
  APV_REALTERM_COMMAND_PREFIX_BSYNCAND_INDEX,
  APV_REALTERM_COMMAND_PREFIX_BSYNCXOR_INDEX,
  APV_REALTERM_COMMAND_PREFIX_BSYNCHI_INDEX,
  APV_REALTERM_COMMAND_PREFIX_BSYNCLEAD_INDEX,
  APV_REALTERM_COMMAND_PREFIX_BSYNCSHOWCOUNT_INDEX,
  APV_REALTERM_COMMAND_PREFIX_KEYMAPVT_INDEX,
  APV_REALTERM_COMMAND_PREFIX_KEYMAP_INDEX,
  APV_REALTERM_COMMAND_PREFIX_CRC_INDEX,
  APV_REALTERM_COMMAND_PREFIX_VERSION_INDEX,
  APV_REALTERM_COMMAND_PREFIXES
  } apvRealTermCommandPrefixIndicies_t;

/******************************************************************************/
/* Global Variable Declarations :                                             */
/******************************************************************************/

extern char apvRealTermCommands[APV_REALTERM_COMMAND_PREFIXES][APV_REALTERM_COMMAND_PREFIX_LENGTH];
extern char apvRealTermPrefixes[APV_REALTERM_COMMAND_PREFIXES][APV_REALTERM_COMMAND_PREFIX_LENGTH];

/******************************************************************************/

#endif

/******************************************************************************/
