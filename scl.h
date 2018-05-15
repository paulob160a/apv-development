/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* scl.h                                                                      */
/* 12.05.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/******************************************************************************/

#ifndef _SCL_H_
#define _SCL_H_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdint.h>
#ifdef _GNU_C_
#include <stdbool.h>
#else
typedef enum sclBool_t { sclFalse = 0, sclTrue } sclBool_t;
#define bool  sclBool_t
#define false sclFalse
#define true  sclTrue
#endif

/******************************************************************************/
/* Constant Definitions :                                                     */
/******************************************************************************/

#define SCL_NUMERICAL_POSITIVE '+'
#define SCL_NUMERICAL_NEGATIVE            '-'
#define SCL_NUMERICAL_POINT               '.'
#define SCL_NUMERICAL_EXPONENT            'e' // use the lower-case 'e' only
#define SCL_NUMERICAL_ZERO                '0'
#define SCL_NUMERICAL_ONE                 '1'
#define SCL_NUMERICAL_TWO                 '2'
#define SCL_NUMERICAL_THREE               '3'
#define SCL_NUMERICAL_FOUR                '4'
#define SCL_NUMERICAL_FIVE                '5'
#define SCL_NUMERICAL_SIX                 '6'
#define SCL_NUMERICAL_SEVEN               '7'
#define SCL_NUMERICAL_EIGHT               '8'
#define SCL_NUMERICAL_NINE                '9'
                                          
#define FLOAT64_SIGN_LEAVES                 2
#define FLOAT64_DIGIT_LEAVES               10
#define FLOAT64_EXPONENT_LEAVES             1
#define FLOAT64_POINT_LEAVES                1
                                          
#define FLOAT64_SIGN_TWIG_SIZE              1 // the full numbers' sign is one character only
#define FLOAT64_DIGIT_TWIG_SIZE             1 // digit part is one character only
#define FLOAT64_POINT_TWIG_SIZE             1 // optional decimal-point is one-character only
#define FLOAT64_FRACTION_MINIMUM_TWIG_SIZE  1 // the fractional part is 0 { .. } 10 chaacters
#define FLOAT64_FRACTION_MAXIMUM_TWIG_SIZE  FLOAT64_DIGIT_LEAVES
#define FLOAT64_EXPONENT_TWIG_SIZE          1 // the exponent marker is one-character
#define FLOAT64_EXP_SIGN_TWIG_SIZE          1 // the exponent sign is one-character
#define FLOAT64_EXPONENT_MINIMUM_TWIG_SIZE  0 // the exponent part is 1 { .. } 3 chaacters
#define FLOAT64_EXPONENT_MAXIMUM_TWIG_SIZE  3

#define FLOAT_64_PARSER_TWIGS               7 // a 64-bit floating-point number in decimal
                                              // representation requires seven rules

#define FLOAT64_MAXIMUM_TWIG               32

#define SCL_LEAF_WHITESPACE_SPACE           ' '

#define SCL_LEAF_SUBSTRING_END_OF_STRING    0x00 // the usual ASCIIZ

#define SCL_FLOAT64_MAXIMUM_STRING_LENGTH   64
#define SCL_FLOAT64_TEST_STRINGS            16

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef enum sclErrorCode_tTag
  {
  SCL_ERROR_CODE_NONE = 0,
  SCL_ERROR_CODE_NULL_PARAMETER,
  SCL_ERROR_CODE_PARAMETER_OUT_OF_RANGE,
  SCL_ERROR_CODE_PARSE_FAILURE,
  SCL_ERROR_CODES
  } sclErrorCode_t;

typedef sclErrorCode_t SCL_ERROR_CODE;

// ALL TOKENS ARE STRICTLY SINGLE-CHARACTER!!!
typedef struct sclTokenParseRule_tTag
  {
  char                       *legalLeaves;         // "leaves" are single-characters
  uint8_t                     numberOfLegalLeaves; // length of the list of legal "leaves" for this "twig"
  uint8_t                     minimumTwigSize;     // the minimum number of single-character leaves that can comprise this "twig"
  uint8_t                     maximumTwigSize;     // the maximum number of single-character leaves that can comprise this "twig"
  bool                        optionalTwig;        // is this "twig" strictly required ?
  bool                        forceNextTwig;       // if this twig is optional but included, the next twig is required, optional or not
  } sclTokenParseRule_t;

// A GROUP OF "TWIGS" COMPRISES A CORRECTLY-PARSED TOKEN
typedef struct sclTokenParseEnsemble_tTag
  {
  sclTokenParseRule_t **legalRules;
  uint8_t               numberOfLegalRules;
  } sclTokenParseEnsemble_t;

typedef enum sclLeafState_tTag
  {
  SCL_LEAF_STATE_SEARCH_FAILED = 0,
  SCL_LEAF_STATE_OPTION_FOUND,
  SCL_LEAF_STATE_OPTION_NOT_FOUND,
  SCL_LEAF_STATE_SEARCH_SUCCEEDED,
  SCL_LEAF_STATES
  } sclLeafState_t;

typedef struct sclFloat64Test_tTag
  {
  char testString[SCL_FLOAT64_MAXIMUM_STRING_LENGTH];
  bool testResultRequired;
  } sclFloat64Test_t;

/******************************************************************************/
/* Global Variable Declarations :                                             */
/******************************************************************************/

extern const sclTokenParseEnsemble_t float64Parser;

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern SCL_ERROR_CODE sclParse64BitFloat(const char                    *float64,
                                         const uint16_t                 float64Length,
                                         const sclTokenParseEnsemble_t *float64ParseEnsemble,
                                         double                        *float64Result);
extern SCL_ERROR_CODE sclNumericalParserPrint(const sclTokenParseEnsemble_t *parserEnsemble);
extern sclLeafState_t sclSeekNextLegalLeaf(      char               **searchIndex,
                                                 char                *subString,
                                           const uint16_t             leafStringLength,
                                                 sclTokenParseRule_t *leafList,
                                                 bool                 optionForceFlag);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
