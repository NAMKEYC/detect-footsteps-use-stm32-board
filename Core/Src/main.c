/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include <l3gd20_process.h>
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include "../../Drivers/driver_oled/Inc/fonts.h"
#include "../../Drivers/driver_oled/Inc/ssd1306.h"
#include "../../Drivers/button_driver/Inc/button.h"
#include <sys_button.h>
#include <string.h>
#include <stdio.h>
#include <l3gd20_process.h>
#include "../../Drivers/L3GD20_Driver/Inc/l3gd20.h"
#include "../../Drivers/LSM303_Driver/Inc/lsm303.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  START,
  POSITIVE_SIGNAL,
  NEGATIVE_SIGNAL,
} thresh_hold_t;
typedef enum
{
  ON,
  READY,
  DISPLAY,
  OFF
} device_state_t;
typedef enum
{
  INIT,
  THRESH_ABOVE,
  THRESH_BELOW,
  COMPLETED_FOOT
} state_foot_t; // initialize footstep machine
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
spi_cs_t cs1;
ang_device_t l3gd2;
state_foot_t state_foot;
float angleratez = 0;
uint8_t flag = 0;
uint8_t count_step = 0;
uint32_t count_time_1 = 0;
uint32_t count_time_2 = 0;
uint8_t state = 0;
uint32_t count_time = 0;    //
uint32_t debounce_time = 0; //       initialize variables
uint32_t press_but = 0;     //
uint8_t clear_flag = 1;
button_t button1; //
char foot_count[] = "  ";
LSM303DLHC_TypeDef lsm303;
device_state_t device_state;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == htim2.Instance) // timer 2 to determine footsteps distance
  {
    flag++;
  }
  else if (htim->Instance == htim3.Instance)
  {
    count_time++;      // Increase time interrupt of timer with 50ms
    debounce_time = 1; // Turn on debounce time flag
  }
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  l3gd20_cs_init(&cs1, GPIOE, GPIO_PIN_3);
  l3gd20_init(&cs1);
  lsm303_init(&lsm303, &hi2c1);
  ssd1306_init();
  device_state = OFF;
  state_foot = INIT;
  button_init(&button1, GPIOA, GPIO_PIN_0);
  ssd1306_gotoxy(0, 0);
  ssd1306_puts("HELLO OWNER", &Font_7x10, 1);
  ssd1306_gotoxy(0, 20);
  ssd1306_puts("PRESS BUTTON TO", &Font_7x10, 1);
  ssd1306_gotoxy(0, 40);
  ssd1306_puts("START PROGRAM", &Font_7x10, 1);
  ssd1306_updatescreen(); // display
  HAL_Delay(2000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    while (press_but == 1)
    {
      HAL_TIM_Base_Start_IT(&htim3); // turn on timer 3 for counting
      if (debounce_time == 1)        // check debounce times with flag
      {
        //============= Turn on device ==================//
        if (device_state == OFF)
        {
          device_state = ON;
          ssd1306_clear();
        }
        button_get_event(&button1, count_time); // get button event HOLD or CLICK
        if (button1.event == CLICK && device_state == ON)
        {
          HAL_TIM_Base_Stop(&htim3); // turn off timer 3
          button1.event = NOEVENT;
          debounce_time = 0;
          press_but = 0;
          break;
        }
        //============= Check hold button ==================//
        if (button1.event == HOLD)
        {
          press_but = 0;
          debounce_time = 0;
          count_time = 0;
          button1.event = NOEVENT;
          device_state = OFF;
          HAL_TIM_Base_Stop(&htim3);
          break;
        }
      }
    }
    switch (device_state)
    {
    case ON:
    {
      //============= Get device direction==================//
      lsm303dlhc_getaccelerometer(&lsm303);
      lsm303dlhc_getdirection(&lsm303);
      if (lsm303.direct == OTHER)
      {
        ssd1306_clear();
        ssd1306_gotoxy(20, 30);
        ssd1306_puts("WRONG DIRECTION", &Font_7x10, 1);
        ssd1306_updatescreen();
      }
      else
      {
        ssd1306_clear();
        clear_flag = 1;
        device_state = READY;
      }
    }
    case READY:
    {
      if (clear_flag == 1)
      {
        ssd1306_gotoxy(10, 10);
        ssd1306_puts("Device is ready", &Font_7x10, 1);
        ssd1306_gotoxy(20, 30);
        ssd1306_puts("Waiting...", &Font_7x10, 1);
        ssd1306_updatescreen();
        clear_flag = 0;
      }
      //============= Read angular rate ==================//
      l3gd20_main(&l3gd2, &cs1);
      if (l3gd2.anglerate_z != 0) // Check device when reading is ready
      {
        ssd1306_clear();
        device_state = DISPLAY;
        break;
      }
      break;
    }
    case DISPLAY:
    {
      l3gd20_main(&l3gd2, &cs1);
      //============= Check device direction ==================//
      lsm303dlhc_getaccelerometer(&lsm303);
      lsm303dlhc_getdirection(&lsm303);
      if (lsm303.direct == OTHER)
      {
        ssd1306_clear();
        device_state = ON;
        break;
      }
      //============= Detect footstep algorithm ==================//
      switch (state_foot)
      {
      case INIT:
      {

        if (l3gd2.anglerate_z > 200) // if the threshold value above 250
        {
          count_time_1 = HAL_GetTick();  // value time at present
          HAL_TIM_Base_Start_IT(&htim2); // turn on timer2 interrupt
          state_foot = THRESH_ABOVE;     // switch to state A
        }
        break;
      }
      case THRESH_ABOVE:
      {
        if (l3gd2.anglerate_z < -200) // if the threshold value below -250
        {
          state_foot = THRESH_BELOW; // switch to state B
        }
        break;
      }
      case THRESH_BELOW:
      {
        if (l3gd2.anglerate_z > -200) // if the threshold value below -250
        {
          count_time_2 = HAL_GetTick();               // value time at present
          if (abs(count_time_2 - count_time_1) > 500) // if the previous time value minus the later time value is over 500
          {                                           //
            state_foot = COMPLETED_FOOT;              //(noise filtering) switch to state
          }
          else // if there is noise return to state INIT
          {
            count_time_2 = 0;
            count_time_1 = 0;
            flag = 0;
            HAL_TIM_Base_Stop_IT(&htim2);
            HAL_Delay(20);
            state_foot = INIT;
          }
        }
        break;
      }
      case COMPLETED_FOOT:
      {
        if (flag == 1) // if footstep cycle equals 800ms
        {              // counter footstep number
          if (l3gd2.anglerate_z > -200)
          {
            count_step++;
            flag = 0;
            count_time_1 = 0;
            count_time_2 = 0;
            HAL_TIM_Base_Stop_IT(&htim2);
            state_foot = INIT; // switch to state INIT
          }
        }
        else if (flag > 1) //  Eliminating the case that the step cycle is above 800ms
        {
          flag = 0;
          count_time_1 = 0;
          count_time_2 = 0;
          HAL_TIM_Base_Stop_IT(&htim2);
          state_foot = INIT; // SWITCH TO STATE INIT
        }
        break;
      }
      }
      //============= Display OLED ==================//
      HAL_Delay(1);
      foot_count[0] = count_step / 10 + '0';
      foot_count[1] = count_step % 10 + '0';
      ssd1306_gotoxy(0, 0);
      ssd1306_puts("FOOT:", &Font_7x10, 1);
      ssd1306_gotoxy(0, 20);
      ssd1306_puts(foot_count, &Font_7x10, 1);
      ssd1306_updatescreen(); // display
      break;
    }
    //============= Turn off device ==================//
    case OFF:
    {
      ssd1306_clear();
      count_step = 0;
      break;
    }
    }
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
}

/**
 * @brief I2C2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 16000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 15999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 49;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PE3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) // function interrupt callback
{
  if (GPIO_Pin == button1.GPIO_Pin)
  {
    press_but = 1; // Turn on interrupt button flag
  }
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
