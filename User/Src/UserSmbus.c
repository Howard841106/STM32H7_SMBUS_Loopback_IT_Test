/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : UserSmbus.c
  * @brief          : SMBus logic implementation
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "common.h"

/* Private Constants ---------------------------------------------------------*/
#define SMBUS_SLAVE_ADDR   (2)
#define TEST_TIMEOUT_MS    1000U

/* Private Variables ---------------------------------------------------------*/
static uint8_t txData[] = "HELLO_SMBUS";
static uint8_t rxData[sizeof(txData)];

static volatile uint8_t slaveRxDone  = 0;
static volatile uint8_t masterTxDone = 0;
static volatile uint8_t smbusError   = 0;

/* Private Function Prototypes -----------------------------------------------*/
static void Prepare_SMBUS_Test(void);
static void Print_Buffer_ASCII(const uint8_t *buf, uint16_t len);

/* --- LED Helper Macros --- */
#define LED1_ON()          HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET)
#define LED1_OFF()         HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET)
#define LED2_ON()          HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET)
#define LED2_OFF()         HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET)
#define LED3_ON()          HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET)
#define LED3_OFF()         HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET)

/* Exported Functions Implementation -----------------------------------------*/

TestResult_t UserSmbus_RunLoopbackTest(void)
{
  HAL_StatusTypeDef status;
  uint32_t tickStart;

  printf("============== SMBus Loopback Test Start ==============\r\n");
  Prepare_SMBUS_Test();

  /* 1. 啟動從機 (I2C2) 監聽模式 */
  status = HAL_SMBUS_EnableListen_IT(&hsmbus2);
  if (status != HAL_OK) return TEST_ERROR;

  HAL_Delay(1);

  /* 2. 啟動主機 (I2C1) 發送數據 */
  status = HAL_SMBUS_Master_Transmit_IT(&hsmbus1, SMBUS_SLAVE_ADDR, txData, 
                                        sizeof(txData), SMBUS_FIRST_AND_LAST_FRAME_NO_PEC);
  if (status != HAL_OK) return TEST_ERROR;

  /* 3. 等待完成 */
  tickStart = HAL_GetTick();
  while ((slaveRxDone == 0U) || (masterTxDone == 0U))
  {
    if (smbusError != 0U) return TEST_ERROR;
    if ((HAL_GetTick() - tickStart) > TEST_TIMEOUT_MS) return TEST_TIMEOUT;
  }

  return TEST_PASS;
}

TestResult_t UserSmbus_CheckData(void)
{
  printf("TX ASCII : "); Print_Buffer_ASCII(txData, sizeof(txData)); printf("\r\n");
  printf("RX ASCII : "); Print_Buffer_ASCII(rxData, sizeof(rxData)); printf("\r\n");

  if (memcmp(txData, rxData, sizeof(txData)) == 0) return TEST_PASS;
  return TEST_MISMATCH;
}

void UserSmbus_RecoverBus(void)
{
  HAL_SMBUS_DeInit(&hsmbus1);
  HAL_SMBUS_DeInit(&hsmbus2);
  HAL_Delay(10);
  MX_I2C1_SMBUS_Init();
  MX_I2C2_SMBUS_Init();
}

void UserSmbus_Indicate_Pass(void)
{
  LED1_ON(); LED2_ON(); LED3_ON();
  printf("SMBus test PASS\r\n\r\n");
}

void UserSmbus_Indicate_Error(void)
{
  LED1_OFF(); LED2_OFF(); LED3_OFF();
  printf("SMBus test FAIL (ERROR/TIMEOUT)\r\n\r\n");
}

void UserSmbus_Indicate_Mismatch(void)
{
  LED1_ON(); LED2_OFF(); LED3_OFF();
  printf("SMBus test FAIL (MISMATCH)\r\n\r\n");
}

/* --- HAL Callbacks --- */

void HAL_SMBUS_AddrCallback(SMBUS_HandleTypeDef *hsmbus, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
  if (hsmbus->Instance == I2C2) {
    HAL_SMBUS_Slave_Receive_IT(&hsmbus2, rxData, sizeof(rxData), SMBUS_FIRST_AND_LAST_FRAME_NO_PEC);
  }
}

void HAL_SMBUS_SlaveRxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
  if (hsmbus->Instance == I2C2) slaveRxDone = 1;
}

void HAL_SMBUS_MasterTxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
  if (hsmbus->Instance == I2C1) masterTxDone = 1;
}

void HAL_SMBUS_ListenCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
  if (hsmbus->Instance == I2C2) HAL_SMBUS_EnableListen_IT(&hsmbus2);
}

void HAL_SMBUS_ErrorCallback(SMBUS_HandleTypeDef *hsmbus)
{
  smbusError = 1;
}

/* --- Private Functions --- */

static void Prepare_SMBUS_Test(void)
{
  smbusError = 0; slaveRxDone = 0; masterTxDone = 0;
  memset(rxData, 0, sizeof(rxData));
}

static void Print_Buffer_ASCII(const uint8_t *buf, uint16_t len)
{
  for (uint16_t i = 0; i < len; i++) {
    printf("%c", ((buf[i] >= 32U) && (buf[i] <= 126U)) ? buf[i] : '.');
  }
}