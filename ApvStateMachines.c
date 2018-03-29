/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvStateMachines.c                                                         */
/* 25.03.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - a set of definitions and functions to handle generic state-machine       */
/*   states, variables, processing and transitions. All APV state-machines    */
/*   be defined as non-generic "clones" of these structures to allow a single */
/*   engine to process each state-machine requirement.                        */
/*                                                                            */
/*  State-machines' states must be defined in the form :                      */
/*                                                                            */
/*  apv<...>States_tTag                                                       */
/*   {                                                                        */
/*   APV_<...>_STATE_NULL = 0,                                                */
/*   APV_<...>_STATE_INITIALISATION,                                          */
/*        ...                                                                 */
/*   < domain state-machine specific states >                                 */
/*        ...                                                                 */
/*   APV_<...>_ERROR_REPORTER,                                                */
/*   APV_<...>_STATES // the number of states in this machine                 */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include "ApvError.h"
#include "ApvStateMachines.h"
#include "ApvMessageHandling.h"

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvExecuteStateMachine() :                                                 */
/*  --> apvDomainStateMachine : pointer to the concrete state-machine to be   */
/*                              executed                                      */
/*  <-- apvStateMachineErrors : error codes                                   */
/*                                                                            */
/* - using the defining table execute the next state of the machine and       */
/*   return the error status                                                  */
/*                                                                            */
/******************************************************************************/

APV_GENERIC_STATE_CODE apvExecuteStateMachine(apvGenericState_t *apvDomainStateMachine)
  {
/******************************************************************************/

  APV_GENERIC_STATE_CODE apvStateMachineCode = APV_STATE_MACHINE_CODE_NONE;

/******************************************************************************/

  // Execute the current and next states of the machine until the driving event 
  // has completed - which may be none, part or whole of the complete process
  while (apvStateMachineCode != APV_STATE_MACHINE_CODE_STOP)
    {
    apvStateMachineCode = (APV_GENERIC_STATE_CODE)(apvDomainStateMachine + apvDomainStateMachine->apvGenericStateVariables->apvGenericStateVariables[APV_GENERIC_STATE_VARIABLE_ACTIVE_STATE])->apvGenericStateAction(apvDomainStateMachine + apvDomainStateMachine->apvGenericStateVariables->apvGenericStateVariables[APV_GENERIC_STATE_VARIABLE_ACTIVE_STATE]);
    }

/******************************************************************************/

  return(apvStateMachineCode);

/******************************************************************************/
  } /* end of apvExecuteStateMachine                                          */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/