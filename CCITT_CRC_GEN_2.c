/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* CCITT_CRC_GEN_2.c                                                          */
/* 20.03.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - implements CCITT-CRC16. The initial CRC register value at compute (tx ?) */
/*   and check (rx ?) is 0xFFF. The two-byte CRC remainder is added to the    */
/*   message as :                                                             */
/*                                                                            */
/*    message{n]     = HIGH_BYTE                                              */
/*    message{n + 1] = LOW_BYTE                                               */
/*                                                                            */
/* - at the check-side a successful CRC check results in a CRC register value */
/*  of 0x0000                                                                 */
/*                                                                            */
/******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <conio.h>
#include "ar19937.h"
#include "ApvError.h"
#include "ApvCrcGenerator.h"
#include "ApvMessageHandling.h"
#include "ApvCommsUtilities.h"
#include "ApvUtilities.h"

/******************************************************************************/

#define INITIAL_RANDOM_SEED         ((uint32_t)0xFEDCBA98)
                                    
#define RANDOM_BYTE_MASK            ((uint16_t)0x000000FF)
#define RANDOM_MASK_SHIFT           8
#define RANDOM_SHIFT_LIMIT          (sizeof(uint16_t) * RANDOM_MASK_SHIFT)
                                    
#define MAXIMUM_ENTROPY             APV_MESSAGING_MAXIMUM_UNSTUFFED_MESSAGE_LENGTH
#define CRC_ENTROPY                   2

#define APV_RING_BUFFER_TEST_STRING "The rain in Spain falls mainly on the citizens who are happy!"

#define _EXIT                       'x'

/******************************************************************************/

uint8_t               informationStream[MAXIMUM_ENTROPY    + CRC_ENTROPY];
uint8_t               informationStreamIn[MAXIMUM_ENTROPY  + CRC_ENTROPY];
uint8_t               informationStreamOut[MAXIMUM_ENTROPY + CRC_ENTROPY];

apvMessageStructure_t newMessage;

/******************************************************************************/

int main(void);

/******************************************************************************/

int main(void)
  {
/******************************************************************************/

    time_t        Now              = 0;
  uint32_t        information      = 0,
                  iterations       = 0;
  uint8_t         randomMaskShift  = 0,
                  randomInfo       = 0;
                                   
  uint16_t        entropy          = 0;
  uint16_t        crcRegister      = APV_CRC_GENERATOR_INITIAL_VALUE,
                  crcMemory        = 0;
  uint16_t        ringBufferLength = (APV_COMMS_RING_BUFFER_MAXIMUM_LENGTH + 1),
                  tokensLoaded     = MAXIMUM_ENTROPY,
                  tokensUnLoaded   = MAXIMUM_ENTROPY,
                  bufferToLoad     = 0,
                  tokensToLoad     = 0,
                  tokensToUnLoad   = 0;

  apvRingBuffer_t ringBuffer0;

  APV_ERROR_CODE  crcError         = APV_ERROR_CODE_NONE,
                  apvError         = APV_ERROR_CODE_NONE;

  int             keyPressed       = 0;
  bool            exit             = false;

/******************************************************************************/

  time(&Now);
  init_genrand((unsigned long)Now);
  
  // Build a random "message" and successively compute the CRC
  for (entropy = 0; entropy < MAXIMUM_ENTROPY; entropy++)
    {
    information = genrand_int32();

    randomInfo = (uint8_t)((information & (RANDOM_BYTE_MASK << randomMaskShift)) >> randomMaskShift);

    informationStream[entropy] = randomInfo;

    apvComputeCrc(randomInfo, &crcRegister);

    randomMaskShift = randomMaskShift + RANDOM_MASK_SHIFT;

    if (randomMaskShift >= RANDOM_SHIFT_LIMIT)
      {
      randomMaskShift = 0;
      }
    }

  // Load the CRC result into the last two bytes of the message
  informationStream[MAXIMUM_ENTROPY + 1] = (uint8_t)(crcRegister & RANDOM_BYTE_MASK);
  informationStream[MAXIMUM_ENTROPY]     = (uint8_t)((crcRegister & (RANDOM_BYTE_MASK << RANDOM_MASK_SHIFT)) >> RANDOM_MASK_SHIFT);

  printf("\n Iterative Message Buffer CRC : %04x", crcRegister);

  crcMemory = crcRegister;

  // Now check the message by computing the remainder CRC
  crcRegister = APV_CRC_GENERATOR_INITIAL_VALUE;

  for (entropy = 0; entropy < (MAXIMUM_ENTROPY + CRC_ENTROPY); entropy++)
    {
    randomInfo = informationStream[entropy];
    apvComputeCrc(randomInfo, &crcRegister);

    printf("\n message[%03d] = %02x : crc = %04x", entropy, randomInfo, crcRegister);
    }
    
  if (crcRegister == 0)
    {
    printf("\n CRC check is correct : %04x", crcRegister);
    }
  else
    {
    printf("\n CRC check is wrong   : %04x", crcRegister);
    }

  // Flush the divisor
  randomInfo = 0;

  apvComputeCrc(randomInfo, &crcRegister);
  apvComputeCrc(randomInfo, &crcRegister);

/******************************************************************************/
/* Do the whole thing again using a function. "Outgoing" messages' CRCs can   */
/* be computed en-bloc                                                        */
/******************************************************************************/

  // Clear out the previously computed CRC
  informationStream[MAXIMUM_ENTROPY + 1] = 0;
  informationStream[MAXIMUM_ENTROPY]     = 0;

  crcError = apvBlockComputeCrc(&informationStream[0], MAXIMUM_ENTROPY, &crcRegister);

  printf("\n Block Message Buffer CRC : %04x", crcRegister);

  if (crcMemory == crcRegister)
    {
    printf("\n CORRECT : Iterative and Block methods equal  : %04x -v- %04x", crcMemory, crcRegister);
    }
  else
    {
    printf("\n WRONG  : Iterative and Block methods differ : %04x -v- %04x", crcMemory, crcRegister);
    }

/******************************************************************************/
/* Now arbitrarily corrupt a message byte and show the CRC results are unique */
/******************************************************************************/

  informationStream[5] = informationStream[5] ^ 0x55;

  // Clear out the previously computed CRC
  informationStream[MAXIMUM_ENTROPY + 1] = 0;
  informationStream[MAXIMUM_ENTROPY]     = 0;

  crcError = apvBlockComputeCrc(&informationStream[0], MAXIMUM_ENTROPY, &crcRegister);

  printf("\n Block Message Buffer CRC : %04x", crcRegister);

  if (crcMemory == crcRegister)
    {
    printf("\n WRONG  : Iterative and Block methods equal  : %04x -v- %04x", crcMemory, crcRegister);
    }
  else
    {
    printf("\n CORRECT : Iterative and Block methods differ : %04x -v- %04x", crcMemory, crcRegister);
    }

/******************************************************************************/
/* Test the message framing                                                   */
/******************************************************************************/

  // Arbitrarily insert a SOM token
  informationStream[23] = APV_MESSAGING_START_OF_MESSAGE;

  // Using the previously constructed message first
  apvError = apvFrameMessage(&newMessage,
                              APV_COMMS_PLANE_SPI_RADIO_0,
                              APV_SIGNAL_PLANE_CONTROL_5,
                             &informationStream[0],
                              MAXIMUM_ENTROPY);


  if (apvError == APV_ERROR_CODE_NONE)
    {
    printf("\n BASIC : Message-framing has worked");
    }
  else
    {
    printf("\n BASIC : Message-framing has failed");
    }

  // Now using a message consisting of exclusively 0x7e (SOM)
  for (entropy = 0; entropy < MAXIMUM_ENTROPY; entropy++)
    {
    informationStream[entropy] = APV_MESSAGING_START_OF_MESSAGE;
    }

  apvError = apvFrameMessage(&newMessage,
                              APV_COMMS_PLANE_SPI_RADIO_0,
                              APV_SIGNAL_PLANE_CONTROL_5,
                             &informationStream[0],
                              MAXIMUM_ENTROPY);

  if (apvError == APV_ERROR_CODE_NONE)
    {
    printf("\n SOM : Message-framing has worked");
    }
  else
    {
    printf("\n SOM : Message-framing has failed");
    }

/******************************************************************************/
/* Test the ring-buffer handlers                                              */
/******************************************************************************/

  apvError = apvRingBufferInitialise(NULL,
                                     APV_COMMS_RING_BUFFER_MINIMUM_LENGTH);

  if (apvError == APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR)
    {
    printf("\n CORRECT : ring-buffer initialisation has failed : NULL pointer");
    }
  else
    {
    printf("\n WRONG: ring-buffer initialisation has passed : NULL pointer");
    }

   apvError = apvRingBufferInitialise(&ringBuffer0,
                                       0);

   if (apvError == APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR)
    {
    printf("\n CORRECT : ring-buffer initialisation has failed : buffer length < 2");
    }
  else
    {
    printf("\n WRONG: ring-buffer initialisation has passed : buffer length < 2");
    }

   apvError = apvRingBufferInitialise(&ringBuffer0,
                                       (APV_COMMS_RING_BUFFER_MAXIMUM_LENGTH + 1));

   if (apvError == APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR)
    {
    printf("\n CORRECT : ring-buffer initialisation has failed : buffer length > 64");
    }
  else
    {
    printf("\n WRONG: ring-buffer initialisation has passed : buffer length > 64");
    }
   
   while (ringBufferLength > APV_COMMS_RING_BUFFER_MAXIMUM_LENGTH)
     {
     printf("\n Enter a ring-buffer length [ 0 .. 64 ] : ");
     scanf("%hu", &ringBufferLength);
     }

   apvError = apvRingBufferInitialise(&ringBuffer0,
                                       ringBufferLength);

   if (apvError == APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR)
    {
    printf("\n WRONG : ring-buffer initialisation has failed : buffer length rounding = %04x", ringBuffer0.apvCommsRingBufferLength);
    }
  else
    {
    printf("\n CORRECT: ring-buffer initialisation has passed : buffer length rounding = %04x", ringBuffer0.apvCommsRingBufferLength);
    }

   // Test the ring-buffer loading function
   bufferToLoad = ringBufferLength;
   tokensToLoad = 5;

   while (tokensLoaded > 0)
     {
     tokensLoaded = apvRingBufferLoad(&ringBuffer0,
                                      &informationStream[0],
                                       tokensToLoad,
                                       true);

     printf("\n --> %02d tokens loaded", tokensLoaded);
     }

   // Similarly the ring-buffer unloading function
   tokensToUnLoad = 3;

   while (tokensUnLoaded > 0)
     {
     tokensUnLoaded = apvRingBufferUnLoad(&ringBuffer0,
                                          &informationStream[0],
                                           tokensToUnLoad,
                                           true);
     printf("\n --> %02d tokens unloaded", tokensUnLoaded);
     }

/******************************************************************************/
/* Test buffer loading and unloading together using random numbers for the    */
/* number of tokens to load/unload                                            */
/******************************************************************************/

   iterations = 0;

   while (exit == false)
     {
     printf("\n ITERATION [%08d]", iterations++);

     tokensToLoad = genrand_int32();

     if (tokensToLoad > ringBuffer0.apvCommsRingBufferLength)
       {
       tokensToLoad = tokensToLoad % ringBuffer0.apvCommsRingBufferLength;
       }

     for (entropy = 0; entropy < tokensToLoad; entropy++)
       {
       informationStreamIn[entropy] = (uint8_t)genrand_int32();
       }

     tokensLoaded = MAXIMUM_ENTROPY;

     printf("\n\n --> %03d tokens to load", tokensToLoad);

//     while (tokensLoaded > 0)
       {
       tokensLoaded = apvRingBufferLoad(&ringBuffer0,
                                        &informationStreamIn[0],
                                         tokensToLoad,
                                         true);
     
       printf("\n --> %02d tokens loaded", tokensLoaded);

       printf("\n");

       for (entropy = 0; entropy < tokensLoaded; entropy++)
         {
         printf(" %02x", informationStreamIn[entropy]);
         }

       apvRingBufferPrint(&ringBuffer0);
       }
     
     // Similarly the ring-buffer unloading function
     while (tokensToUnLoad == tokensToLoad)
       {
       tokensToUnLoad = genrand_int32();

       if (tokensToUnLoad > ringBuffer0.apvCommsRingBufferLength)
         {
         tokensToUnLoad = tokensToUnLoad % ringBuffer0.apvCommsRingBufferLength;
         }
       }

     tokensUnLoaded = MAXIMUM_ENTROPY;

     printf("\n --> %03d tokens to unload", tokensToUnLoad);

//     while (tokensUnLoaded > 0)
       {
       tokensUnLoaded = apvRingBufferUnLoad(&ringBuffer0,
                                            &informationStreamOut[0],
                                             tokensToUnLoad,
                                             true);
       printf("\n --> %02d tokens unloaded", tokensUnLoaded);

       printf("\n");

       for (entropy = 0; entropy < tokensUnLoaded; entropy++)
         {
         printf(" %02x", informationStreamOut[entropy]);
         }

       apvRingBufferPrint(&ringBuffer0);
       }

     while (!_kbhit())
       ;

     keyPressed = _getch();

     if (keyPressed == _EXIT)
       {
       exit = true;
       }
     }

/******************************************************************************/


   while (!_kbhit())
    ;

/******************************************************************************/

  return(0);

/******************************************************************************/
  } /* end of main */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

#if (0)

/*
 * The basic idea of a pre-generated table is that we, for each leftmost
 * byte in the current CRC16, pre-calculate the XOR pattern with a 0x00 input
 * byte. We can then create the next CRC by using the old rightmost byte as
 * the new leftmost byte, the new data as the rightmost byte, and XOR this new
 * 16-bit word with the contents of the lookup table. We index our lookup
 * table using the old leftmost byte, as it is what controls the XOR
 * operations in the CRC.
 */

int main(void) {
	uint16_t i;
	int j;
	int xor_flag;
	uint16_t result;

	/* Print header */
	printf("/*\n"
			" * This file is autogenerated by %s\n"
			" * See http://github.com/zqad/crc16-ccitt"
			" for more information\n"
			" */\n\n", __FILE__);

	printf("#include <stdint.h>\n\n");
	printf("uint16_t crc16_ccitt_table[] = {\n\t");
	for (i = 0; i < 256; i++) {
		result = i << 8;
		for (j = 0; j < 8; j++) {
			/* Flag for XOR if leftmost bit is set */
			xor_flag = result & 0x8000;

			/* Shift CRC */
			result <<= 1;

			/* Perform the XOR */
			if (xor_flag)
				result ^= 0x1021;
		}
		printf("0x%04hX,", result);

		/* Pretty formatting */
		if ((i + 1) % 8)
			printf(" ");
		else if (i != 255)
			printf("\n\t");
	}
	printf("\n};\n");
}

#endif