/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* InterpreterUtilities.c                                                     */
/* 12.05.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "scl.h"

/******************************************************************************/
/* Constant Definitions :                                                     */
/******************************************************************************/
/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/

SCL_ERROR_CODE sclNumericalParserPrint(const sclTokenParseEnsemble_t *parserEnsemble)
  {
/******************************************************************************/

  SCL_ERROR_CODE sclError = SCL_ERROR_CODE_NONE;
  uint8_t        twigs    = 0,
                 leaves   = 0;

/******************************************************************************/

  if (parserEnsemble == NULL)
    {
    sclError = SCL_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    if (parserEnsemble->numberOfLegalRules == 0)
      {
      sclError = SCL_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
      }
    else
      {
      for (twigs = 0; twigs < parserEnsemble->numberOfLegalRules; twigs++)
        {
        printf("\n TWIG_[%02d] : ", twigs);
        printf("\n --------- ");
        printf("\n");
        printf("\n Number of possible leaf characters : %d", (parserEnsemble->legalRules[twigs])->numberOfLegalLeaves);

        for (leaves = 0; leaves < (parserEnsemble->legalRules[twigs])->numberOfLegalLeaves; leaves++)
          {
          printf(" [%c]", (parserEnsemble->legalRules[twigs])->legalLeaves[leaves]);
          }

        printf("\n");
        printf("\n Twig size range : %d to %d", (parserEnsemble->legalRules[twigs])->minimumTwigSize, (parserEnsemble->legalRules[twigs])->maximumTwigSize);
        printf("\n - this  Twig Mandated   : %c ", ((parserEnsemble->legalRules[twigs])->optionalTwig  == true) ? 'Y' : 'N');
        printf("\n - force Twig Mandate    : %c ", ((parserEnsemble->legalRules[twigs])->forceNextTwig == true) ? 'Y' : 'N');
        printf("\n");
        }
      }
    }

/******************************************************************************/

  return(sclError);

/******************************************************************************/
  } /* end of sclNumericalParserPrint                                         */

/******************************************************************************/
/* sclSeekNextLegalLeaf() :                                                   */
/*  <-> searchIndex      : leaf string start address and final search address */
/*  >-> subString        : return the complete leaf ensemble found            */
/*  --? leafStringLength : the distance to search before giving up            */
/*  --> leafList         : list of legal leaves to terminate the search       */
/*  --> optionForceFlag  : if this leaf was optional :                        */
/*                             [ false ==  it is still optional |             */
/*                                true ==  it is now mandatory ]              */
/*  <-- leafState        : search success or failure code                     */
/*                                                                            */
/*  - search a leaf string for the first possible legal leaf or terminate on  */
/*    end-of-string or terminate after 'n' search failures                    */
/******************************************************************************/

sclLeafState_t sclSeekNextLegalLeaf(      char               **searchIndex,
                                          char                *subString,
                                    const uint16_t             leafStringLength,
                                          sclTokenParseRule_t *leafList,
                                          bool                 optionForceFlag)
  {
/******************************************************************************/

  sclLeafState_t leafState   = SCL_LEAF_STATE_SEARCH_FAILED;
  uint16_t       tokens      = 0,
                 extraLeaves = 0,
                 leaves      = 0;

/******************************************************************************/

  // The substring defaults to "empty"
  subString[0] =  SCL_LEAF_SUBSTRING_END_OF_STRING;

  // Skip past the white space
  while ((**searchIndex == SCL_LEAF_WHITESPACE_SPACE) && (leaves < leafStringLength) && (**searchIndex != SCL_LEAF_SUBSTRING_END_OF_STRING))
    {
    *searchIndex = *searchIndex + 1;
     tokens      =  tokens      + 1;
    } 

  // Exit if there is no more string left
  if ((tokens != leafStringLength) && (**searchIndex != SCL_LEAF_SUBSTRING_END_OF_STRING))
    {
    for (leaves = 0; leaves < leafList->numberOfLegalLeaves; leaves++)
      {
      if (**searchIndex == leafList->legalLeaves[leaves])
        { // A matching leaf has been found
        break;
        }
      }
    
    if (leaves == leafList->numberOfLegalLeaves)
      { // No matching leaf has been found - for an optional leaf this is not necessarily a failure
      if (leafList->optionalTwig == true)
        {
        if (optionForceFlag == false)
          {
          leafState =  SCL_LEAF_STATE_OPTION_NOT_FOUND;
          }
        else
          {
          leafState =  SCL_LEAF_STATE_SEARCH_FAILED;
          }
        }
      }
    else
      { // A matching leaf has been found - see if the leaf has more characters
      if (leafList->maximumTwigSize > 1)
        {
        subString[0] = **searchIndex;

        *searchIndex = *searchIndex + 1;

        for (extraLeaves = 1; extraLeaves < leafList->maximumTwigSize; extraLeaves++)
          {
          if ((**searchIndex != leafStringLength) && (**searchIndex != SCL_LEAF_SUBSTRING_END_OF_STRING))
            {
            for (leaves = 0; leaves <= leafList->numberOfLegalLeaves; leaves++)
              {
              if (leaves == leafList->numberOfLegalLeaves)
                { // The leaf has ended
                subString[extraLeaves] = SCL_LEAF_SUBSTRING_END_OF_STRING;
                extraLeaves                = leafList->maximumTwigSize;
                break;
                }

              if (**searchIndex == leafList->legalLeaves[leaves])
                { // A matching leaf has been found
                subString[extraLeaves]     = **searchIndex;
                subString[extraLeaves + 1] = SCL_LEAF_SUBSTRING_END_OF_STRING;

                *searchIndex           =  *searchIndex + 1;

                 break;
                }
              }
            }
          else
            {
            break;
            }
          }

        if (leafList->optionalTwig == true)
          {
          leafState = SCL_LEAF_STATE_OPTION_FOUND;
          }
        else
          {
          leafState = SCL_LEAF_STATE_SEARCH_SUCCEEDED;
          }
        }
      else
        { // Create a single-character substring
        subString[0] = **searchIndex;
        subString[1] =   SCL_LEAF_SUBSTRING_END_OF_STRING;
     
        *searchIndex = *searchIndex + 1;

        if (leafList->optionalTwig == true)
          {
          leafState = SCL_LEAF_STATE_OPTION_FOUND;
          }
        else
          {
          leafState = SCL_LEAF_STATE_SEARCH_SUCCEEDED;
          }
        }
      }
    }

/******************************************************************************/

  return(leafState);

/******************************************************************************/
  } /* end of sclSeekNextLegalLeaf                                            */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/