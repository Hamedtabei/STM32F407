/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "iwdg.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define ST7789_READ_DISPLAY 	  0x0B
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define ST7789_WIDTH			240
#define ST7789_HEIGHT			240

#define ST7789_RST_PORT			GPIOD
#define ST7789_RST_PIN			GPIO_PIN_0

#define ST7789_DC_PORT			GPIOD
#define ST7789_DC_PIN			GPIO_PIN_1

#define ST7789_CS_PORT			GPIOD
#define ST7789_CS_PIN			GPIO_PIN_2

#define ST7789_BLK_PORT			GPIOD
#define ST7789_BLK_PIN			GPIO_PIN_3

#define ST7789_SPI_PORT 		hspi3

/* Sleep Out */
#define ST7789_SLPOUT					 0x11
/* Normal Display Mode On*/
#define ST7789_NORON				  	 0x13
/* Display Inversion Off*/
#define ST7789_INVOFF  					 0x20
#define ST7789_INVON   					 0x21
/* Display On*/
#define ST7789_DISPON 	 				 0x29
/* Column Address Set */
#define ST7789_CASET   					 0x2A
/* Row Address Set */
#define ST7789_RASET   					 0x2B
/* Memory Write	*/
#define ST7789_RAMWR					 0x2c
/* Idle Mode Off*/
#define ST7789_IDMOFF					 0x38
/* Interface Pixel Format*/
#define ST7789_COLMOD					 0x3A
/* Write Display Brightness	*/
#define ST7789_WRDISBV					 0x51
/* Porch Setting*/
#define ST7789_PORCTRL					 0xB2

/** MADCTL
 * Memory Data Access Control Register (0x36H)
 * MAP:     D7  D6  D5  D4  D3  D2  D1  D0
 * param:   MY  MX  MV  ML  RGB MH  -   -
 *
 */

/* Page Address Order ('0': Top to Bottom, '1': the opposite) */
#define ST7789_MADCTL_MY  		0x80
/* Column Address Order ('0': Left to Right, '1': the opposite) */
#define ST7789_MADCTL_MX  		0x40
/* Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode) */
#define ST7789_MADCTL_MV  		0x20
/* Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = the opposite) */
#define ST7789_MADCTL_ML  		0x10
/* RGB/BGR Order ('0' = RGB, '1' = BGR) */
#define ST7789_MADCTL_RGB 		0x00
/* Display Data Latch Data Order ('0' = LCD Refresh Left to Right, '1' = the opposite)*/
#define ST7789_MADCTL_MH  		0x00


/* Advanced options */
#define ST7789_COLOR_MODE_16bit 0x55    //  RGB565 (16bit)
#define ST7789_COLOR_MODE_18bit 0x66    //  RGB666 (18bit)

#define WHITE 					0xFFFF
#define BLACK      				0x0000
#define BLUE        			0x001F
#define RED         			0xF800
#define MAGENTA     			0xF81F
#define GREEN       			0x07E0
#define CYAN        			0x7FFF
#define YELLOW      			0xFFE0
#define GRAY        			0X8430
#define BRED        			0XF81F
#define GRED        			0XFFE0
#define GBLUE       			0X07FF
#define BROWN       			0XBC40
#define BRRED       			0XFC07
#define DARKBLUE    			0X01CF
#define LIGHTBLUE   			0X7D7C
#define GRAYBLUE    			0X5458
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
  MX_SPI3_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */


  HAL_SPI_StateTypeDef spi_status = HAL_SPI_GetState(&hspi3);

  HAL_Delay(10);
  HAL_GPIO_WritePin(ST7789_BLK_PORT, ST7789_BLK_PIN, GPIO_PIN_RESET);

  HAL_Delay(10);
  HAL_GPIO_WritePin(ST7789_RST_PORT, ST7789_RST_PIN, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(ST7789_RST_PORT, ST7789_RST_PIN, GPIO_PIN_SET);
  HAL_Delay(20);

  ST7789_SendCommand(ST7789_COLMOD);					//	Set color mode
  ST7789_SendSmallData(ST7789_COLOR_MODE_18bit);


  spi_status = HAL_SPI_GetState(&hspi3);

  ST7789_SendCommand(ST7789_PORCTRL);				//	Porch control
  {
  	uint8_t data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
  	ST7789_SendData(data, sizeof(data));
  }

  /* Internal LCD Voltage generator settings */
  ST7789_SendCommand(0XB7);				//	Gate Control
  ST7789_SendSmallData(0x35);			//	Default value
  ST7789_SendCommand(0xBB);				//	VCOM setting
  ST7789_SendSmallData(0x19);			//	0.725v (default 0.75v for 0x20)
  ST7789_SendCommand(0xC0);				//	LCMCTRL
  ST7789_SendSmallData (0x2C);			//	Default value
  ST7789_SendCommand (0xC2);				//	VDV and VRH command Enable
  ST7789_SendSmallData (0x01);			//	Default value
  ST7789_SendCommand (0xC3);				//	VRH set
  ST7789_SendSmallData (0x12);			//	+-4.45v (defalut +-4.1v for 0x0B)
  ST7789_SendCommand (0xC4);				//	VDV set
  ST7789_SendSmallData (0x20);			//	Default value
  ST7789_SendCommand (0xC6);				//	Frame rate control in normal mode
  ST7789_SendSmallData (0x0F);			//	Default value (60HZ)
  ST7789_SendCommand (0xD0);				//	Power control
  ST7789_SendSmallData (0xA4);			//	Default value
  ST7789_SendSmallData (0xA1);			//	Default value

  /**************** Division line ****************/

  ST7789_SendCommand(0xE0);
  {
	 uint8_t data[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
	 ST7789_SendData(data, sizeof(data));
  }

  ST7789_SendCommand(0xE1);
  {
	  uint8_t data[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
	  ST7789_SendData(data, sizeof(data));
  }

  ST7789_SendCommand(ST7789_INVON);
  ST7789_SendCommand(ST7789_SLPOUT);
  ST7789_SendCommand(ST7789_NORON);
  ST7789_SendCommand(ST7789_DISPON);

  HAL_Delay(50);
  ST7789_Fill_Color(WHITE);
  HAL_Delay(2000);
  ST7789_Fill_Color(BLACK);
  HAL_Delay(2000);
  ST7789_Fill_Color(YELLOW);


 /*
  ST7789_SendCommand(ST7789_Write_Display_Brightness);
  ST7789_SendSmallData(ST7789_WHITE);

  ST7789_SendCommand(ST7789_Idle_Mode_Off);

  ST7789_SendCommand(ST7789_Memory_Write);
  HAL_Delay(10);

  uint8_t data = 0x00;
  ST7789_SendSmallData(data);

*/


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void ST7789_SendCommand(uint8_t cmd){
	 HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(ST7789_DC_PORT, ST7789_DC_PIN, GPIO_PIN_RESET);  // DC reset for sending command

	 HAL_SPI_Transmit(&hspi3, &cmd, sizeof(cmd), HAL_MAX_DELAY);

	 HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_SET);
}

void ST7789_SendSmallData(uint8_t data){
	HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(ST7789_DC_PORT, ST7789_DC_PIN, GPIO_PIN_SET);  // DC set for sending data

		 HAL_SPI_Transmit(&hspi3, &data, sizeof(data), HAL_MAX_DELAY);

		 HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_SET);
}

void ST7789_SendData(uint8_t *buff, size_t buff_size)
{
	HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ST7789_DC_PORT, ST7789_DC_PIN, GPIO_PIN_SET);

	// split data in small chunks because HAL can't send more than 64K at once

	while (buff_size > 0) {
		uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;

			HAL_SPI_Transmit(&ST7789_SPI_PORT, buff, chunk_size, HAL_MAX_DELAY);

		buff += chunk_size;
		buff_size -= chunk_size;
	}

	 HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_SET);
}


void ST7789_SetAddressWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
	HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_RESET);
/* Write to RAM */
	ST7789_SendCommand(ST7789_RAMWR);

	/* Column Address set */
	ST7789_SendCommand(ST7789_CASET);
	{
		uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
		ST7789_SendData(data, sizeof(data));
	}

	/* Row Address set */
	ST7789_SendCommand(ST7789_RASET);
	{
		uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
		ST7789_SendData(data, sizeof(data));
	}

	 HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_SET);
}

void ST7789_Fill_Color(uint16_t color)
{
	uint16_t i;
	ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
	HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_RESET);

	uint16_t j;
	for (i = 0; i < ST7789_WIDTH; i++)
			for (j = 0; j < ST7789_HEIGHT; j++) {
				uint8_t data[] = {color >> 8, color & 0xFF};
				ST7789_SendData(data, sizeof(data));
			}


	 HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_SET);
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
