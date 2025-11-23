/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Delay.h"
#include "OLED.h"
#include "MadgwickAHRS.h"
#include "MPU6050.h"
#include "software_iic.h"
#include "stdio.h"
#include "string.h"
#include "Huidu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
MPU6050Params mpu6050 = {
        .MPU6050dt = 10,
        .preMillis = 0,
        .MPU6050ERROE = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
};

SensorMsg msg = {
        .A = {0.0f, 0.0f, 0.0f},
        .G = {0.0f, 0.0f, 0.0f}
};

int16_t AX, AY, AZ, GX, GY, GZ;
float x,y,z;
float Yaw = 0;
float Pitch = 0;
float Roll = 0;
float Yaw_copy = 0; // 用于记录Yaw角度的副�?????????
float Pitch_copy = 0; // 用于记录Pitch角度的副�?????????
float Roll_copy = 0; // 用于记录Roll角度的副�?????????
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
unsigned char Digtal;
unsigned char Anolog[8]={0};
unsigned char rx_buff[256]={0};
unsigned char Normal[8]={0};
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
  MX_TIM5_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_TIM15_Init();
  /* USER CODE BEGIN 2 */
  

  DWT_Init();
//  HAL_TIM_PWM_Start(&htim15,TIM_CHANNEL_1);
//  HAL_TIM_PWM_Start(&htim15,TIM_CHANNEL_2);
//  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
//  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
//  __HAL_TIM_SetCompare(&htim15,TIM_CHANNEL_1,1000);
//  __HAL_TIM_SetCompare(&htim15,TIM_CHANNEL_2,1000); 
//  __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_3,1000);
//  __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_4,1000);
	
//	Huidu_Init();
//	Huidu_GetNum_Start();

  HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim5,TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim8,TIM_CHANNEL_ALL);
  

  OLED_Init();
  OLED_Clear();



  //OLED_ShowString(0,0,"Hello!",OLED_8X16);
  OLED_Update();

  int encoder_a = 0;
  int encoder_b;
  int encoder_c;
  int encoder_d;

	Delay_ms(100);
	MPU6050_Init();
	begin(1000.0f / (float)mpu6050.MPU6050dt);    // Initialize the Madgwick filter with the sample frequency
  dataGetERROR();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // encoder_a = htim2.Instance->CNT;
    // encoder_b = htim4.Instance->CNT;
    // encoder_c = htim5.Instance->CNT;
    // encoder_d = htim8.Instance->CNT;

    if(uwTick - mpu6050.preMillis >= mpu6050.MPU6050dt) {
			mpu6050.preMillis = uwTick;
			dataGetAndFilter();		                            
			updateIMU(msg.G[0], msg.G[1], msg.G[2], msg.A[0], msg.A[1], msg.A[2]);
		}
		Yaw = getYaw();
		Pitch = getPitch();
		Roll = getRoll();

     OLED_ShowFloatNum(4*16,0,Yaw,4,2,OLED_8X16);
     OLED_ShowFloatNum(4*16,1*16,Pitch,4,2,OLED_8X16);
     OLED_ShowFloatNum(4*16,2*16,Roll,4,2,OLED_8X16);
		int a = Ping();
    //OLED_ShowNum(4*6,0,a,4,OLED_8X16);
    
    
    encoder_a = __HAL_TIM_GET_COUNTER(&htim2);
    encoder_b = __HAL_TIM_GET_COUNTER(&htim4);
    encoder_c = __HAL_TIM_GET_COUNTER(&htim5);
    encoder_d = __HAL_TIM_GET_COUNTER(&htim8);
    
//    OLED_ShowSignedNum(0,1*16,encoder_a,5,OLED_8X16);
//    OLED_ShowSignedNum(0,2*16,encoder_b,5,OLED_8X16);
//    OLED_ShowSignedNum(0,3*16,encoder_c,5,OLED_8X16);
//    OLED_ShowSignedNum(0,0*16,encoder_d,5,OLED_8X16);
    OLED_Update();



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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 32;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void dataGetERROR() {
    for(uint8_t i = 0; i < 100; ++i) {
        getMPU6050Data();
        mpu6050.MPU6050ERROE[0] += msg.A[0];
        mpu6050.MPU6050ERROE[1] += msg.A[1];
        mpu6050.MPU6050ERROE[2] += msg.A[2] - 9.79;
        mpu6050.MPU6050ERROE[3] += msg.G[0];
        mpu6050.MPU6050ERROE[4] += msg.G[1];
        mpu6050.MPU6050ERROE[5] += msg.G[2];
        Delay_ms(10);
    }
    for(uint8_t i = 0; i < 6; ++i) {
        mpu6050.MPU6050ERROE[i] /= 100.0f;
    }
}

void getMPU6050Data() {
    MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);		// Get data from MPU6050
    msg.A[0] = (float)((float)AX / (float)32768) * 16 * 9.8*(90.0/75.0)*(180.0/38.8);
    msg.A[1] = (float)((float)AY / (float)32768) * 16 * 9.8*(90.0/75.0)*(180.0/38.8);
    msg.A[2] = (float)((float)AZ / (float)32768) * 16 * 9.8*(90.0/75.0)*(180.0/38.8);
    msg.G[0] = (float)((float)GX / (float)32768) * 2000 * 3.5 *(90.0/172.0)*(90.0/84.0)*(180.0/38.8);
    msg.G[1] = (float)((float)GY / (float)32768) * 2000 * 3.5 *(90.0/172.0)*(90.0/84.0)*(180.0/38.8);
    msg.G[2] = (float)((float)GZ / (float)32768) * 2000 * 3.5 *(90.0/172.0)*(90.0/84.0)*(180.0/38.8);

}

void dataGetAndFilter() {
    getMPU6050Data();
    msg.A[0] -= mpu6050.MPU6050ERROE[0];
    msg.A[1] -= mpu6050.MPU6050ERROE[1];
    msg.A[2] -= mpu6050.MPU6050ERROE[2];
    msg.G[0] -= mpu6050.MPU6050ERROE[3];
    msg.G[1] -= mpu6050.MPU6050ERROE[4];
    msg.G[2] -= mpu6050.MPU6050ERROE[5];
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

#ifdef  USE_FULL_ASSERT
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
