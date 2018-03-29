/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvStateMachines.h                                                         */
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

#ifndef _APV_STATE_MACHINES_
#define _APV_STATE_MACHINES_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

// The generic form of the state-machine description
typedef enum apvGenericStates_tTag
  {
  APV_GENERIC_STATE_NULL = 0,
  APV_GENERIC_STATE_INITIALISATION,
  APV_GENERIC_STATE_ERROR_REPORTER,
  APV_GENERIC_STATES
  } apvGenericStates_t;

typedef uint32_t  apvGenericVariable_t; // generic state-variable type

typedef void     *apvGenericResults_t;

// This is the template for the domain-specific state-variable array
typedef enum apvGenericStateVariable_tTag
  {
  APV_GENERIC_STATE_VARIABLE_NUMBER_OF_STATES = 0, // "static" field for the life of the state machine
  APV_GENERIC_STATE_VARIABLE_FIRST_STATE,          // "static" field for the life of the state machine
  APV_GENERIC_STATE_VARIABLE_LAST_STATE,           // "static" field for the life of the state machine
  APV_GENERIC_STATE_VARIABLE_ACTIVE_STATE,
  APV_GENERIC_STATE_VARIABLE_0,
  APV_GENERIC_STATE_VARIABLE_1,
  APV_GENERIC_STATE_VARIABLE_2,
  APV_GENERIC_STATE_VARIABLE_3,
  APV_GENERIC_STATE_VARIABLE_4,
  APV_GENERIC_STATE_VARIABLE_5,
  APV_GENERIC_STATE_VARIABLE_6,
  APV_GENERIC_STATE_VARIABLE_7,
  APV_GENERIC_STATE_VARIABLE_8,
  APV_GENERIC_STATE_VARIABLE_9,
  APV_GENERIC_STATE_VARIABLE_10,
  APV_GENERIC_STATE_VARIABLE_11,
  APV_GENERIC_STATE_VARIABLE_CACHE_LENGTH
  } apvGenericStateVariable_t;

// State-machine codes
typedef enum apvGenericStateCodes_tTag
  {
  APV_STATE_MACHINE_CODE_NONE = APV_ERROR_CODE_NONE,
  APV_STATE_MACHINE_CODE_STOP,
  APV_STATE_MACHINE_CODE_RUN,
  APV_STATE_MACHINE_CODE_ERROR,
  APV_STATE_MACHINE_CODES
  } apvGenericStateCodes_t;

typedef apvGenericStateCodes_t APV_GENERIC_STATE_CODE;

// A generic set of variables to carry information between states
typedef struct apvGenericStateVariables_tTag
  {
  apvGenericVariable_t apvGenericStateVariables[APV_GENERIC_STATE_VARIABLE_CACHE_LENGTH];
  } apvGenericStateVariables_t;

// Holds the state of an in-progress attempt to de-frame a low-level message
typedef struct apvGenericState_tTag
  {
  apvGenericStates_t           apvGenericCurrentState;
  apvGenericStates_t           apvGenericNextState;
  APV_ERROR_CODE             (*apvGenericStateAction)(struct apvGenericState_tTag *apvGenericStateMachine);
  apvGenericStateVariables_t  *apvGenericStateVariables;
  apvGenericResults_t          apvGenericResults;
  } apvGenericState_t;

// The generic state-machine
typedef apvGenericState_t apvGenericStateMachine[APV_GENERIC_STATES];

// Definite state-machine types
typedef enum apvStateMachineType_tTag
  {
  APV_GENERIC_STATE_MACHINE_TYPE = 0,
  APV_MESSAGING_STATE_MACHINE_TYPE,
  APV_STATE_MACHINE_TYPES
  } apvStateMachineType_t;

typedef apvStateMachineType_t APV_STATE_MACHINE_TYPE;

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern APV_GENERIC_STATE_CODE apvExecuteStateMachine(apvGenericState_t *apvDomainStateMachine);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/