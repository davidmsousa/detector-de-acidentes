#include "main.h"
#include "stm32f0xx_hal.h"
#include "i2c.h"   //importa biblioteca i2c
#include "gpio.h"  //importa biblioteca gpio
#include "sd_hal_mpu6050.h"
#include <stdio.h>


I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

SD_MPU6050 mpuCarrinho; // se não der certo esse jeito tenta o outro

const int MPU=0x68;  //endereço I2C do MPU6050
                     //usar MPU=0x69 só no caso de for conectar duas MPUs
//int AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;  //Variaveis para armazenar valores dos sensores

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);

void conexaoBluetooth(int16_t *ax,int16_t *ay, int16_t *az, int16_t *gx, int16_t *gy, int16_t *gz)
{
	  HAL_UART_Transmit(&huart1,(uint8_t*)ax,sizeof(ax),100);
	  HAL_Delay(100);
	  HAL_UART_Transmit(&huart1,(uint8_t*)ay,sizeof(ay),100);
	  HAL_Delay(100);
	  HAL_UART_Transmit(&huart1,(uint8_t*)az,sizeof(az),100);
	  HAL_Delay(100);
	  HAL_UART_Transmit(&huart1,(uint8_t*)gx,sizeof(gx),100);
	  HAL_Delay(100);
 	  HAL_UART_Transmit(&huart1,(uint8_t*)gy,sizeof(gy),100);
 	  HAL_Delay(100);
	  HAL_UART_Transmit(&huart1,(uint8_t*)gz,sizeof(gz),100);
	  HAL_Delay(100);
}




int main(void)
{

  HAL_Init();
  SystemClock_Config(); //configurar sistema de clock

  //Inicializa todos os periféricos configurados
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();

  while (1) //fica fazendo loop infinito / equivalente ao void loop() do arduino
  {

      SD_MPU6050_ReadAccelerometer(&hi2c1,&mpuCarrinho);
  	  int16_t a_x = mpuCarrinho.Accelerometer_X;
  	  int16_t a_y = mpuCarrinho.Accelerometer_Y;
  	  int16_t a_z = mpuCarrinho.Accelerometer_Z;

  	  SD_MPU6050_ReadGyroscope(&hi2c1,&mpuCarrinho);
  	  int16_t g_x = mpuCarrinho.Gyroscope_X;
  	  int16_t g_y = mpuCarrinho.Gyroscope_Y;
  	  int16_t g_z = mpuCarrinho.Gyroscope_Z;

  	  conexaoBluetooth (a_x, a_y, a_z, g_x, g_y, g_z);

      HAL_Delay(500);
  }


}




/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C1 init function */
/*void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    //Configure Analogue filter

  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    //Configure Digital filter

  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}*/

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 38400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
/*static void MX_GPIO_Init(void)
{

  // GPIO Ports Clock Enable
  __HAL_RCC_GPIOA_CLK_ENABLE();

}*/

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
