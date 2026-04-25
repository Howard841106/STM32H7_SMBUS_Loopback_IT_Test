/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : UserSmbus.h
  * @brief          : SMBus logic and test functions
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __USER_SMBUS_H
#define __USER_SMBUS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported Types ------------------------------------------------------------*/
typedef enum
{
  TEST_PASS = 0,
  TEST_ERROR,
  TEST_TIMEOUT,
  TEST_MISMATCH
} TestResult_t;

/* Exported Functions Prototypes ---------------------------------------------*/

/* 核心測試流程 */
TestResult_t UserSmbus_RunLoopbackTest(void);
TestResult_t UserSmbus_CheckData(void);
void UserSmbus_RecoverBus(void);

/* 指示燈與狀態輸出 */
void UserSmbus_Indicate_Pass(void);
void UserSmbus_Indicate_Error(void);
void UserSmbus_Indicate_Mismatch(void);

#ifdef __cplusplus
}
#endif

#endif /* __USER_SMBUS_H */