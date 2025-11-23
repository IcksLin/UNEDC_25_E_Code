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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "Serial.h"
#include "stdio.h"
#include "Delay.h"
#include "Servo.h"
#include "packet_parser.h"
#include "My_Interrupt.h"
#include "fuzzypid.h"
#include "IncrementalPID.h"
#include "menu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
float my_kp = 0.64;
float my_ki = 0.03f;
float my_kd = 0.02;
float my_kp_x = 0.32;
float my_ki_x = 0.04f;
float my_kd_x = 0.02;

int16_t change_flag = 0;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart9;

/* USER CODE BEGIN PV */
#if 1
#if (__ARMCC_VERSION >= 6010050)                  
__asm(".global __use_no_semihosting\n\t");        
__asm(".global __ARM_use_no_argv \n\t");          

#else
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    
};

#endif

int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

FILE __stdout;

int fputc(int ch, FILE *f) {
    HAL_UART_Transmit(&huart9, (uint8_t *)&ch, 1, HAL_MAX_DELAY); 
    return ch;
}
#endif
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM6_Init(void);
static void MX_UART9_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_UART9_Init();
  /* USER CODE BEGIN 2 */
  DWT_Init();
 
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);//x
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);//y
  Servo_Init(&htim2,TIM_CHANNEL_3,TIM_CHANNEL_4);
  //串口
  Delay_ms(500);
  UART_StartReceiveIT();
  Packet_t received_packet;
  OLED_Init();
  OLED_Clear();
  Delay_ms(100);
  //中断
  HAL_TIM_Base_Start_IT(&htim6);
  
  init_menu();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int16_t Key_index = 0;
  
  init_all_pid_new();//标准PID

  //增量式PID
  /*******获取目标点位(start)*******/
  // //等待串口数据
  // while (blue_brick.x+blue_brick.y==0)
  // {
  //   /* code */
  //   OLED_ShowString(0,0,"waiting",OLED_8X16);
  //   OLED_Update();
  //   if (UART_GetPacket(&received_packet)) {
  //       ParsePacket((char*)received_packet.data);
  //   }
  // }
  // OLED_Clear();

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    OLED_Clear();
    // OLED_ShowNum(0, 0*16, uwTick, 8,OLED_8X16);
    

    if (UART_GetPacket(&received_packet)) {
        // 处理完整数据包（如转发或解析
        //HAL_UART_Transmit(&huart9, received_packet.data, received_packet.length, HAL_MAX_DELAY);
        ParsePacket((char*)received_packet.data);
       

    }

    // get_menu_action();//该函数会清楚标志位
    cl_action  = get_menu_action();

    //模式1调节x的PID参数
    if(mode_inter_flag==0){
          current_menu = menu_navigate(current_menu,cl_action);
          OLED_DrawMenu(current_menu);

    }else{
      switch (current_menu->id)
      {
        /*******id为0的菜单（start）**********/
        case 0:
          switch (cl_action)
          {
            case 0:
                change_flag += 1;
                change_flag = (change_flag >= 3) ? 0 : change_flag;  // 0→1→2→0...
            break;
            case 1:
              switch (change_flag) {
                case 0: my_kp_x -= 0.01; break;
                case 1: my_ki_x -= 0.01; break;
                case 2: my_kd_x -= 0.01; break;
                default: break;
              }
            break;
            case 2:
              switch (change_flag) {
                  case 0: my_kp_x += 0.01; break;
                  case 1: my_ki_x += 0.01; break;
                  case 2: my_kd_x += 0.01; break;
                  default: break;
              }
            break;
            case 3:
              //菜单标志位置0，返回菜单选择
              mode_inter_flag = 0;
            
            default:
            break;
          }
        /**********可视化部分(start)************/
          OLED_ShowString(0,0,"clock:",OLED_6X8);
          OLED_ShowNum(8*6,0*8,uwTick,8,OLED_6X8);

          OLED_ShowString(0,1*8,"blue:",OLED_6X8);
          OLED_ShowNum(8*6,1*8,blue_brick.x,3,OLED_6X8);
          OLED_ShowNum(8*6+3*8+2,1*8,blue_brick.y,3,OLED_6X8);

          OLED_ShowString(0,2*8,"I:",OLED_6X8);
          OLED_ShowFloatNum(8*6,2*8,pid_y.integral,5,5,OLED_6X8);
          // OLED_ShowNum(8*6+3*8+2,2*8,incr_pid_x.integral,3,OLED_6X8);

          OLED_ShowString(0,3*8,"x_pid_out:",OLED_6X8);
          OLED_ShowFloatNum(8*6+8,3*8,pid_x.output,4,2,OLED_6X8);

          OLED_ShowString(0,4*8,"y_pid_out:",OLED_6X8);
          OLED_ShowFloatNum(8*6+8,4*8,pid_y.output,4,2,OLED_6X8);

          OLED_ShowString(0,5*8,"x_rang:",OLED_6X8);
          OLED_ShowString(0,6*8,"change_index:",OLED_6X8);
          OLED_ShowNum(3*8+4*8+4*8,6*8,change_flag,1,OLED_6X8);

          OLED_ShowString(0,7*8,"pid:",OLED_6X8);
          OLED_ShowFloatNum(3*8,7*8,pid_x.Kp,1,2,OLED_6X8);
          OLED_ShowFloatNum(3*8+4*8,7*8,pid_x.Ki,1,4,OLED_6X8);
          OLED_ShowFloatNum(3*8+4*8+6*8,7*8,pid_x.Kd,1,2,OLED_6X8);

          set_pid_index(&pid_x,my_kp_x,my_kd_x,my_ki_x,0.01);
          set_pid_index(&pid_y,my_kp_x,my_kd,my_ki,0.01);
        /**********可视化部分(end)************/

        break;
      /*******id为0的菜单（end）**********/
        default:
          break;
      }


    }

    // OLED_ShowString(0,3*16,current_menu->name,OLED_8X16);
    // OLED_ShowNum(0,2*16,temp,2,OLED_8X16);

    /**********按键(start)*************/
// for (int i = 0; i < KEY_NUM; i++) {
//     if (keys[i].short_press_flag == 1) {
//         switch (i) {
//             case 0:  // 按键0：切换修改对象
//                 change_flag += 1;
//                 change_flag = (change_flag >= 3) ? 0 : change_flag;  // 0→1→2→0...
//                 keys[i].short_press_flag = KEY_RELEASED;
//                 break;

//             case 1:  // 按键1：减
//                 switch (change_flag) {
//                     case 0: my_kp -= 0.01; break;
//                     case 1: my_ki -= 0.01; break;
//                     case 2: my_kd -= 0.01; break;
//                     default: break;
//                 }
//                 keys[i].short_press_flag = KEY_RELEASED;
//                 break;

//             case 2:  // 按键2：加
//                 switch (change_flag) {
//                     case 0: my_kp += 0.01; break;
//                     case 1: my_ki += 0.01; break;
//                     case 2: my_kd += 0.01; break;
//                     default: break;
//                 }
//                 keys[i].short_press_flag = KEY_RELEASED;
//                 break;

//             case 3:  // 按键3：不操作（原代码 target_point.y+=10 被移除）
//                 keys[i].short_press_flag = KEY_RELEASED;
//                 break;

//             default:
//                 break;
//         }
//     }
// }



    /**********按键(end)*************/

    // if(green_point.x!=999&&green_point.y!=999){
    //   x_move = green_point.x - target_point.x;
    //   y_move = green_point.y - target_point.y;
    // }else{
    //   x_move = y_move = 0;
    // }



    /*******获取目标点位(end)*********/
    // Onto.x = 1500 + 200*sin((float)uwTick/1000.0);
    // Onto.y = 1500 + 200*cos((float)uwTick/1000.0);
    // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,y_angle-1*pid_y.output);
    // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,x_angle-1*pid_x.output);
    // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,1500+pid_x.output);
    // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,1500+pid_y.output);


    // OLED_ShowString(0,0,"clock:",OLED_6X8);
    // OLED_ShowNum(8*6,0*8,uwTick,8,OLED_6X8);

    // OLED_ShowString(0,1*8,"blue:",OLED_6X8);
    // OLED_ShowNum(8*6,1*8,blue_brick.x,3,OLED_6X8);
    // OLED_ShowNum(8*6+3*8+2,1*8,blue_brick.y,3,OLED_6X8);

    // OLED_ShowString(0,2*8,"I:",OLED_6X8);
    // OLED_ShowFloatNum(8*6,2*8,pid_y.integral,5,5,OLED_6X8);
    // // OLED_ShowNum(8*6+3*8+2,2*8,incr_pid_x.integral,3,OLED_6X8);

    // OLED_ShowString(0,3*8,"x_pid_out:",OLED_6X8);
    // OLED_ShowFloatNum(8*6+8,3*8,pid_x.output,4,2,OLED_6X8);

    // OLED_ShowString(0,4*8,"y_pid_out:",OLED_6X8);
    // OLED_ShowFloatNum(8*6+8,4*8,pid_y.output,4,2,OLED_6X8);

    // OLED_ShowString(0,5*8,"x_rang:",OLED_6X8);
    // OLED_ShowString(0,6*8,"change_index:",OLED_6X8);
    // OLED_ShowNum(3*8+4*8+4*8,6*8,change_flag,1,OLED_6X8);

    // OLED_ShowString(0,7*8,"pid:",OLED_6X8);
    // OLED_ShowFloatNum(3*8,7*8,pid_x.Kp,1,2,OLED_6X8);
    // OLED_ShowFloatNum(3*8+4*8,7*8,pid_x.Ki,1,4,OLED_6X8);
    // OLED_ShowFloatNum(3*8+4*8+6*8,7*8,pid_x.Kd,1,2,OLED_6X8);

    // set_pid_index(&pid_x,my_kp_x,my_kd_x,my_ki_x,0.01);
    // set_pid_index(&pid_y,my_kp,my_kd,my_ki,0.01);


    OLED_Update();
    Delay_ms(200);
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
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 200-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 20000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 200-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 10000-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief UART9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART9_Init(void)
{

  /* USER CODE BEGIN UART9_Init 0 */

  /* USER CODE END UART9_Init 0 */

  /* USER CODE BEGIN UART9_Init 1 */

  /* USER CODE END UART9_Init 1 */
  huart9.Instance = UART9;
  huart9.Init.BaudRate = 115200;
  huart9.Init.WordLength = UART_WORDLENGTH_8B;
  huart9.Init.StopBits = UART_STOPBITS_1;
  huart9.Init.Parity = UART_PARITY_NONE;
  huart9.Init.Mode = UART_MODE_TX_RX;
  huart9.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart9.Init.OverSampling = UART_OVERSAMPLING_16;
  huart9.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart9.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart9.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart9) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart9, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart9, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart9) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART9_Init 2 */

  /* USER CODE END UART9_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : PF2 PF3 PF4 PF5 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PC4 PC5 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
