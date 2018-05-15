/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* InterpreterMain.c                                                          */
/* 12.05.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "scl.h"

/******************************************************************************/
/* Constant Definitions :                                                     */
/******************************************************************************/

#define ARGV_0              (0)
#define ARGV_1              (ARGV_0 + 1)
#define NUMBER_OF_ARGUMENTS (ARGV_1 + 1)

#define INTERPRETER         (argv[ARGV_0])
#define FLOAT_64_STRING     (argv[ARGV_1])

#define SCL_FLOAT64_TEST_STRINGS_LOCAL 19

/******************************************************************************/
/* Static Variable Definitions :                                              */
/******************************************************************************/

// 64-bit floating-pont test strings and the expected result of parsing i.e are 
// the strings correctly formed ?
sclFloat64Test_t sclFloat64TestStrings[SCL_FLOAT64_TEST_STRINGS_LOCAL][SCL_FLOAT64_MAXIMUM_STRING_LENGTH] = 
  {
    { "7.123456e1",        true  },         
    { "5.432e222",         true  },          
    { "1e0",               true  },               
    { "+1e0",              true  },              
    { "-1e0",              true  },              
    { "1.0e0",             true  },             
    { "9.9e0",             true  },             
    { "1e101",             true  },             
    { "-3.0001e-001",      true  },      
    { "0.000e+0",          true  },          
    { "+7.002300230023e0", false },
    { "1.e1",              false }, 
    { "     ",             false },
    { "8.0e12",            true, },
    { "8.0e-12",           true, },
    { "8   .0e12",         true  },
    { "8  1 2 .0e12",      false },
    { ".123e1",            false },
    { "9$.123e1",          false }
  };

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

int main(int argc, char *argv[]);

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/

int main(int argc, char *argv[])
  {
/******************************************************************************/

  SCL_ERROR_CODE sclParserErrorCode = SCL_ERROR_CODE_NONE;

  double         float64ParseResult = 0.0;
  uint16_t       testStrings        = 0;

/******************************************************************************/

  system("cls");

  printf("\n SIMPLE COMMAND INTERPRETER : ");
  printf("\n --------------------------   ");

  if (NUMBER_OF_ARGUMENTS != argc)
    {
    printf("\n Usage : SimpleCommandInterpreter <StringToInterpret>");
    printf("\n");
    exit(0);
    }

  sclParserErrorCode = sclNumericalParserPrint(&float64Parser);

  for (testStrings = 0; testStrings < SCL_FLOAT64_TEST_STRINGS_LOCAL; testStrings++)
    {
    printf("\n Test String [%02d][%s] ", testStrings, sclFloat64TestStrings[testStrings]);
    printf("\n");

    sclParserErrorCode = sclParse64BitFloat(&sclFloat64TestStrings[testStrings][0].testString[0],
                                             strlen(sclFloat64TestStrings[testStrings][0].testString),
                                            &float64Parser,
                                            &float64ParseResult);

    if (sclParserErrorCode == SCL_ERROR_CODE_PARSE_FAILURE)
      {
      if (sclFloat64TestStrings[testStrings][0].testResultRequired == false)
        {
        printf("\n PASS : BADLY-FORMED STRING DETECTED ");
        }
      else
        {
        printf("\n FAIL : BADLY-FORMED !NOT! STRING DETECTED ");
        }
      }
    else
      {
      if (sclFloat64TestStrings[testStrings][0].testResultRequired == true)
        {
        printf("\n PASS : WELL-FORMED STRING DETECTED ");
        sscanf((const char *)&(sclFloat64TestStrings[testStrings][0]).testString[0], "%lf", &float64ParseResult);
        printf("\n In C : %e", float64ParseResult);
        }
      else
        {
        printf("\n FAIL : WELL-FORMED !NOT! STRING DETECTED ");
        }
      }

    printf("\n");
    }

/******************************************************************************/

  while (!_kbhit())
    ;

/******************************************************************************/

  return(0);

/******************************************************************************/
  } /* end of main                                                            */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/