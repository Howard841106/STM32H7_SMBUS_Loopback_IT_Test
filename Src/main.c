/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  TEST_PASS = 0,
  TEST_ERROR,
  TEST_TIMEOUT,
  TEST_MISMATCH
} TestResult_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SMBUS_SLAVE_ADDR   (2)
#define TEST_TIMEOUT_MS    1000U

#define LED1_ON()          HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET)
#define LED1_OFF()         HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET)

#define LED2_ON()          HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET)
#define LED2_OFF()         HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET)

#define LED3_ON()          HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET)
#define LED3_OFF()         HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t txData[] = "HELLO_SMBUS";
uint8_t rxData[sizeof(txData)];

volatile uint8_t slaveRxDone  = 0;
volatile uint8_t masterTxDone = 0;
volatile uint8_t smbusError   = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
static void Prepare_SMBUS_Test(void);
static TestResult_t Run_SMBUS_Test(void);
static TestResult_t Check_SMBUS_Data(void);
static void Recover_SMBUS_Bus(void);

static void Indicate_Test_Pass(void);
static void Indicate_Test_Error(void);
static void Indicate_Test_Mismatch(void);

static void Print_Buffer_ASCII(const uint8_t *buf, uint16_t len);
static void Print_Buffer_HEX(const uint8_t *buf, uint16_t len);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

static void Print_Buffer_ASCII(const uint8_t *buf, uint16_t len)
{
  uint16_t i;

  for (i = 0; i < len; i++)
  {
    if ((buf[i] >= 32U) && (buf[i] <= 126U))
    {
      printf("%c", buf[i]);
    }
    else
    {
      printf(".");
    }
  }
}

static void Print_Buffer_HEX(const uint8_t *buf, uint16_t len)
{
  uint16_t i;

  for (i = 0; i < len; i++)
  {
    printf("%02X ", buf[i]);
  }
}

static void Prepare_SMBUS_Test(void)
{
  smbusError   = 0;
  slaveRxDone  = 0;
  masterTxDone = 0;
  memset(rxData, 0, sizeof(rxData));
}

static TestResult_t Run_SMBUS_Test(void)
{
  HAL_StatusTypeDef status;
  uint32_t tickStart;

  printf("============== SMBus Loopback Test Start ==============\r\n");

  Prepare_SMBUS_Test();

  printf("hsmbus1.Init.PeripheralMode = %lu\r\n", hsmbus1.Init.PeripheralMode);
  printf("hsmbus1.Init.OwnAddress1    = 0x%08lX\r\n", hsmbus1.Init.OwnAddress1);
  printf("hsmbus2.Init.PeripheralMode = %lu\r\n", hsmbus2.Init.PeripheralMode);
  printf("hsmbus2.Init.OwnAddress1    = 0x%08lX\r\n", hsmbus2.Init.OwnAddress1);

  status = HAL_SMBUS_EnableListen_IT(&hsmbus2);
  if (status != HAL_OK)
  {
    printf("Enable listen failed, status=%d, state=%lu, err=0x%08lX\r\n",
           status,
           hsmbus2.State,
           hsmbus2.ErrorCode);
    return TEST_ERROR;
  }

  HAL_Delay(1);

  status = HAL_SMBUS_Master_Transmit_IT(&hsmbus1,
                                        SMBUS_SLAVE_ADDR,
                                        txData,
                                        sizeof(txData),
                                        SMBUS_FIRST_AND_LAST_FRAME_NO_PEC);
  if (status != HAL_OK)
  {
    printf("Master transmit start failed, status=%d, state=%lu, err=0x%08lX\r\n",
           status,
           hsmbus1.State,
           hsmbus1.ErrorCode);
    return TEST_ERROR;
  }

  tickStart = HAL_GetTick();
  while ((slaveRxDone == 0U) || (masterTxDone == 0U))
  {
    if (smbusError != 0U)
    {
      printf("SMBus error detected, hsmbus1 err=0x%08lX, hsmbus2 err=0x%08lX\r\n",
             hsmbus1.ErrorCode,
             hsmbus2.ErrorCode);
      return TEST_ERROR;
    }

    if ((HAL_GetTick() - tickStart) > TEST_TIMEOUT_MS)
    {
      printf("SMBus timeout, slaveRxDone=%d, masterTxDone=%d\r\n",
             slaveRxDone,
             masterTxDone);
      return TEST_TIMEOUT;
    }
  }

  return TEST_PASS;
}

static TestResult_t Check_SMBUS_Data(void)
{
  printf("TX ASCII : ");
  Print_Buffer_ASCII(txData, sizeof(txData));
  printf("\r\n");

  printf("RX ASCII : ");
  Print_Buffer_ASCII(rxData, sizeof(rxData));
  printf("\r\n");

  printf("TX HEX   : ");
  Print_Buffer_HEX(txData, sizeof(txData));
  printf("\r\n");

  printf("RX HEX   : ");
  Print_Buffer_HEX(rxData, sizeof(rxData));
  printf("\r\n");

  if (memcmp(txData, rxData, sizeof(txData)) == 0)
  {
    return TEST_PASS;
  }

  return TEST_MISMATCH;
}

static void Recover_SMBUS_Bus(void)
{
  HAL_SMBUS_DeInit(&hsmbus1);
  HAL_SMBUS_DeInit(&hsmbus2);

  HAL_Delay(10);

  MX_I2C1_SMBUS_Init();
  MX_I2C2_SMBUS_Init();
}

static void Indicate_Test_Pass(void)
{
  LED1_ON();
  LED2_ON();
  LED3_ON();
  printf("SMBus test PASS\r\n\r\n");
}

static void Indicate_Test_Error(void)
{
  LED1_OFF();
  LED2_OFF();
  LED3_OFF();
  printf("SMBus test FAIL (ERROR/TIMEOUT)\r\n\r\n");
}

static void Indicate_Test_Mismatch(void)
{
  LED1_ON();
  LED2_OFF();
  LED3_OFF();
  printf("SMBus test FAIL (MISMATCH)\r\n\r\n");
}

void HAL_SMBUS_AddrCallback(SMBUS_HandleTypeDef *hsmbus,
                            uint8_t TransferDirection,
                            uint16_t AddrMatchCode)
{
  printf("AddrCallback entered\r\n");
  (void)TransferDirection;
  (void)AddrMatchCode;

  if (hsmbus->Instance == I2C2)
  {
    if (HAL_SMBUS_Slave_Receive_IT(&hsmbus2,
                                   rxData,
                                   sizeof(rxData),
                                   SMBUS_FIRST_AND_LAST_FRAME_NO_PEC) != HAL_OK)
    {
      smbusError = 1;
    }
  }
}

void HAL_SMBUS_SlaveRxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
  printf("SlaveRxCpltCallback entered\r\n");
  if (hsmbus->Instance == I2C2)
  {
    slaveRxDone = 1;
  }
}

void HAL_SMBUS_MasterTxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
  if (hsmbus->Instance == I2C1)
  {
    masterTxDone = 1;
  }
}

void HAL_SMBUS_ListenCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
  if (hsmbus->Instance == I2C2)
  {
    if (HAL_SMBUS_EnableListen_IT(&hsmbus2) != HAL_OK)
    {
      smbusError = 1;
    }
  }
}

void HAL_SMBUS_ErrorCallback(SMBUS_HandleTypeDef *hsmbus)
{
  (void)hsmbus;
  smbusError = 1;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  TestResult_t testResult;

  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_SMBUS_Init();
  MX_I2C2_SMBUS_Init();
  MX_USART3_UART_Init();

  LED1_OFF();
  LED2_OFF();
  LED3_OFF();

  HAL_Delay(100);

  printf("\r\n");
  printf("========================================\r\n");
  printf(" STM32H7 SMBus Loopback IT Test Start\r\n");
  printf(" I2C1/SMBUS1 = Host\r\n");
  printf(" I2C2/SMBUS2 = Slave\r\n");
  printf(" Slave Addr  = 0x%02X (7-bit raw = 0x%02X)\r\n",
         SMBUS_SLAVE_ADDR,
         (SMBUS_SLAVE_ADDR >> 1));
  printf("========================================\r\n");

  while (1)
  {
    testResult = Run_SMBUS_Test();

    if ((testResult != TEST_PASS) || (smbusError != 0U))
    {
      Recover_SMBUS_Bus();
      Indicate_Test_Error();
      HAL_Delay(500);
      continue;
    }

    testResult = Check_SMBUS_Data();

    if (testResult == TEST_PASS)
    {
      Indicate_Test_Pass();
    }
    else
    {
      Indicate_Test_Mismatch();
    }

    HAL_Delay(1000);
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 275;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                              | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */