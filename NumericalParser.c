/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* NumericalParser.c                                                          */
/* 12.05.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include "scl.h"

/******************************************************************************/
/* Static Variables :                                                         */
/******************************************************************************/

 // Definition of the float64 parsers' component "leaves"
char float64Sign[FLOAT64_SIGN_LEAVES] = 
  {
  SCL_NUMERICAL_POSITIVE,
  SCL_NUMERICAL_NEGATIVE 
  };

char float64Exponent[FLOAT64_EXPONENT_LEAVES] = 
  {
  SCL_NUMERICAL_EXPONENT
  };

char float64Point[FLOAT64_POINT_LEAVES] = 
  {
  SCL_NUMERICAL_POINT
  };

char float64Digit[FLOAT64_DIGIT_LEAVES] = 
  {
  SCL_NUMERICAL_ZERO,
  SCL_NUMERICAL_ONE,
  SCL_NUMERICAL_TWO,
  SCL_NUMERICAL_THREE,
  SCL_NUMERICAL_FOUR,
  SCL_NUMERICAL_FIVE,
  SCL_NUMERICAL_SIX,
  SCL_NUMERICAL_SEVEN,
  SCL_NUMERICAL_EIGHT,
  SCL_NUMERICAL_NINE     
  };

sclTokenParseRule_t float64SignTwig = 
  {
  &float64Sign[0],
   FLOAT64_SIGN_LEAVES,
   FLOAT64_SIGN_TWIG_SIZE,
   FLOAT64_SIGN_TWIG_SIZE,
   true,
   false
  };

sclTokenParseRule_t float64DigitTwig = 
  {
  &float64Digit[0],
   FLOAT64_DIGIT_LEAVES,
   FLOAT64_DIGIT_TWIG_SIZE,
   FLOAT64_DIGIT_TWIG_SIZE,
   false,
   false
  };

sclTokenParseRule_t float64PointTwig = 
  {
  &float64Point[0],
   FLOAT64_POINT_LEAVES,
   FLOAT64_POINT_TWIG_SIZE,
   FLOAT64_POINT_TWIG_SIZE,
   true,
   true
  };

sclTokenParseRule_t float64FractionTwig = 
  {
  &float64Digit[0],
   FLOAT64_DIGIT_LEAVES,
   FLOAT64_FRACTION_MINIMUM_TWIG_SIZE,
   FLOAT64_FRACTION_MAXIMUM_TWIG_SIZE,
   true,
   false
  };

sclTokenParseRule_t float64ExponentTwig = 
  {
  &float64Exponent[0],
   FLOAT64_EXPONENT_LEAVES,
   FLOAT64_EXPONENT_TWIG_SIZE,
   FLOAT64_EXPONENT_TWIG_SIZE,
   false,
   false
  };

sclTokenParseRule_t float64ExpSignTwig = 
  {
  &float64Sign[0],
   FLOAT64_SIGN_LEAVES,
   FLOAT64_EXP_SIGN_TWIG_SIZE,
   FLOAT64_EXP_SIGN_TWIG_SIZE,
   true,
   false
  };

sclTokenParseRule_t float64ExponentDigitTwig = 
  {
  &float64Digit[0],
   FLOAT64_DIGIT_LEAVES,
   FLOAT64_EXPONENT_MINIMUM_TWIG_SIZE,
   FLOAT64_EXPONENT_MAXIMUM_TWIG_SIZE,
   false,
   false
  };

sclTokenParseRule_t *float64ParserTwigs[FLOAT_64_PARSER_TWIGS] = 
  {
  &float64SignTwig,
  &float64DigitTwig,
  &float64PointTwig,
  &float64FractionTwig,
  &float64ExponentTwig,
  &float64ExpSignTwig,
  &float64ExponentDigitTwig
  };

const sclTokenParseEnsemble_t float64Parser = 
  {
  &float64ParserTwigs[0],
   FLOAT_64_PARSER_TWIGS
  };

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/
/* sclParse64BitFloat() :                                                     */
/*  --> float64 : an EXPECTED 64-bit floating-point number represented as a   */
/*                normal 'C' string for conversion to type "double". For this */
/*                implementation a reduced range is used to limit the amount  */
/*                of special-case numerical limit-checking :                  */
/*                                                                            */
/*                [ + | - ]          +                                        */
/*                 0 .. 9            +                                        */
/*               [ .                 +                                        */
/*                 1 { 0 .. 9 } 31 ] +                                        */
/*                 e                 +                                        */
/*               [ + | - ]           +                                        */
/*                 1 { 0 .. 9 } 3 (limit 306 maximum)                         */
/*                                                                            */
/*  --> float64ParseEnsemble : the list of rules to use to parse the string   */
/*  --> float64Result        : the resulting 64-bit number (if legal)         */
/*                                                                            */
/*  <-- slcError : error codes                                                */
/*                                                                            */
/* - parse a limited 64-bit floating-point number represented as a 'C' string */
/*   and if the format is correct convert to a 'C' double type                */
/*                                                                            */
/******************************************************************************/

SCL_ERROR_CODE sclParse64BitFloat(const char                    *float64,
                                  const uint16_t                 float64Length,
                                  const sclTokenParseEnsemble_t *float64ParseEnsemble,
                                  double                        *float64Result)
  {
/******************************************************************************/

  SCL_ERROR_CODE  sclError               = SCL_ERROR_CODE_NONE;
                 
  uint8_t         rules                  = 0;

  sclLeafState_t  leafState              = SCL_LEAF_STATE_SEARCH_SUCCEEDED,
                  leafFailState          = SCL_LEAF_STATE_SEARCH_SUCCEEDED;

  char           *float64Index           = (char *)float64,
                  float64LeafEnsemble[FLOAT64_MAXIMUM_TWIG];

  bool            nextOptionForce        = false;

/******************************************************************************/

  if (float64 == NULL)
    {
    sclError = SCL_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    for (rules = 0; rules < float64ParseEnsemble->numberOfLegalRules; rules++)
      {
      printf("\n RULE [%02d] : ", rules);

      leafState = sclSeekNextLegalLeaf(&float64Index,
                                        float64LeafEnsemble,
                                        float64Length,
                                       (float64ParseEnsemble->legalRules[rules]),
                                        nextOptionForce); 
     
       // If the current leaf was optional : if it was found and its 'force' rule is true the next
       // leaf is no longer optional 
       if (((float64ParseEnsemble->legalRules[rules])->optionalTwig == true) && (leafState == SCL_LEAF_STATE_OPTION_FOUND))
         {
         nextOptionForce = (float64ParseEnsemble->legalRules[rules])->forceNextTwig;
         }
       else
         {
         nextOptionForce = false;
         }

       if (leafState == SCL_LEAF_STATE_SEARCH_SUCCEEDED)
        {
        printf("\n Mandatory Rule[%02d] : %s has succeeded ", rules, float64LeafEnsemble);
        }
       else
         {
         if (leafState == SCL_LEAF_STATE_OPTION_FOUND)
           {
           printf("\n Optional FOUND Rule[%02d] : %s has succeeded ", rules, float64LeafEnsemble);
           }
         else
           {
           if (leafState == SCL_LEAF_STATE_OPTION_NOT_FOUND)
             {
             printf("\n Optional NOT FOUND Rule[%02d] : %s has succeeded ", rules, float64LeafEnsemble);
             }
           else
             {
             if (leafState == SCL_LEAF_STATE_SEARCH_FAILED)
               {
               leafFailState = SCL_LEAF_STATE_SEARCH_FAILED;
               printf("\n Rule [%02d] : %s has failed ", rules, float64LeafEnsemble);
               rules = float64ParseEnsemble->numberOfLegalRules;
               }
             }
           }
         }
       }

    printf("\n");
    }

  if (leafFailState == SCL_LEAF_STATE_SEARCH_FAILED)
    {
    sclError = SCL_ERROR_CODE_PARSE_FAILURE;
    }

/******************************************************************************/

  return(sclError);

/******************************************************************************/
  } /* end of sclParse64BitFloat                                              */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/