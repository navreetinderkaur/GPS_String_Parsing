/**
 ******************************************************************************0
 * FILE               : main.c
 * PROGRAMMER          : NAVREETINDER KAUR
 * DESCRIPTION        : To decode a GPS string and use strtok function
 to parse the fields and print out each field
 ******************************************************************************
 * The functions in this file are used are use display utcTime, longitude with
 direction ,latitude with direction, altitude with units and checksum obtained
 by decoding the gps string.
 * For getting all the above mentioned values strtok function is used to parse
 gps string and values are displayed on LCD.
 **/
/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include "usb_device.h"
#include "string.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

static GPIO_InitTypeDef GPIO_InitStruct;
//number of comma separated values in the data..

/* USER CODE BEGIN Includes */
#include <vcp_prog8125.h>

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim15;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* USER CODE BEGIN PFP */
void HD44780_Init(void);
void HD44780_PutChar(unsigned char c);
void HD44780_GotoXY(unsigned char x, unsigned char y);
void HD44780_PutStr(char *str);
void HD44780_ClrScr(void);

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
circularQueue_t VcpQueue;						// queue used for input from vcp
/* USER CODE END 0 */

int main(void) {

	/* USER CODE BEGIN 1 */
	initializeVcpQueue(&VcpQueue);

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USB_DEVICE_Init();
	HD44780_Init();
	/*Configure GPIO pin : A0_Pin */
	__GPIOA_CLK_ENABLE()
	;
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// initialise the counter
	int i = 0;
	int count = 0;

	/* USER CODE END 2 */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	while (1) {
		/* USER CODE BEGIN 3 */

		// The gps string is defined
		char gpsString[] =
				"$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,0,M,0,*76";

		// assignment of pointers to use in pointer variables in strtok function
		const char *delim = ",";
		char* sentenceId = NULL;
		char* utcTime = NULL;
		char* latitude = NULL;
		char* latitudeDirectionMeasured = NULL;
		char* longitude = NULL;
		char* longitudeDirectionMeasured = NULL;
		char* positionFix = NULL;
		char* numberofSatellitesUsed = NULL;
		char* horizontalDilutionOfPrecision = NULL;
		char* altitude = NULL;
		char* altitudeUnits = NULL;
		char* geoidSeperation = NULL;
		char* geoidSeperationUnit = NULL;
		char* dgpsAge = NULL;
		char* checksum = NULL;

		// the string is divided into several parts using strtok function
		sentenceId = strtok(gpsString, delim);
		utcTime = strtok(NULL, delim);
		latitude = strtok(NULL, delim);
		latitudeDirectionMeasured = strtok(NULL, delim);
		longitude = strtok(NULL, delim);
		longitudeDirectionMeasured = strtok(NULL, delim);
		positionFix = strtok(NULL, delim);
		numberofSatellitesUsed = strtok(NULL, delim);
		horizontalDilutionOfPrecision = strtok(NULL, delim);
		altitude = strtok(NULL, delim);
		altitudeUnits = strtok(NULL, delim);
		geoidSeperation = strtok(NULL, delim);
		geoidSeperationUnit = strtok(NULL, delim);
		dgpsAge = strtok(NULL, delim);
		checksum = strtok(NULL, delim);

		// local variables to store string value
		char latitudeDegreeString[10];
		char latitudeMinuteString[3];
		char latitudeDecimalDegreeString[10];
		char longitudeDegreeString[4];
		char longitudeMinuteString[3];
		char longitudeDecimalDegreeString[10];
		char hourString[5];
		char minuteString[5];
		char secondString[5];
		char hour[20];
		char minute[20];
		char second[20];
		char longitudeDegreeStr[10];
		char longitudeMinuteStr[10];
		char longitudeDecimalDegreeStr[10];
		char altitudeString[10];
		char latitudeDegreeStr[10];
		char latitudeMinuteStr[10];
		char latitudeDecimalDegreeStr[10];

		// local variables
		int utcHour;
		int utcMinute;
		int utcSecond;
		int latitudeDegree;
		int latitudeMinute;
		int latitudeDecimalDegree;
		int longitudeDegree;
		int longitudeMinute;
		int longitudeDecimalDegree;
		int CheckSumDigit;
		char *latitudeStringToStore;
		char *longitudeStringToStore;
		int k;
		int l;

		// condition to check whether push button is pressed or not
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) != 0) {
			// increment counter
			i++;
			count = 1;
			// clear LCD screen
			HD44780_ClrScr();
			HAL_Delay(200);
			// curse is at position x=0 and y=0
			HD44780_GotoXY(0, 0);
		}

		// condition to check if counter has value = 1 or not
		if (i == 1 && count == 1) {
			char *stringForTime;
			stringForTime = utcTime;
			/*Seperate the hour, minutes and seconds
			 into their own NULL terminated string buffers...*/

			//Ist and 2nd will be stored in hour
			for (l = 0; l < 2; l++) {
				hourString[l] = *(stringForTime + l);
			}

			//3rd and 4th will be stored in minute
			for (k = 0, l = 2; l < 4; l++, k++) {
				minuteString[k] = *(stringForTime + l);

			}
			//5th will be excluded as it is '.' and rest will be stored in second
			for (k = 0, l = 4; l < 6; l++, k++) {
				secondString[k] = *(stringForTime + l);
			}

			// convert the string to integer values
			utcHour = atoi(hourString);
			utcMinute = atoi(minuteString);
			utcSecond = atoi(secondString);

			// To convert the hour, minute and second values to string
			sprintf(hour, "%d", utcHour);
			sprintf(minute, "%d", utcMinute);
			sprintf(second, "%d", utcSecond);

			HD44780_ClrScr();
			// To set the character cursor to column=0 and row=0
			HD44780_GotoXY(0, 0);
			HD44780_PutStr("Time: ");
			//To set the character cursor to column=0 and row=1
			HD44780_GotoXY(0, 1);
			//To display the hour value on LCD
			HD44780_PutStr(hour);
			// To set the character cursor to column=2 and row=1
			HD44780_GotoXY(2, 1);
			HD44780_PutStr("hr");
			// To set the character cursor to column=5 and row=1
			HD44780_GotoXY(5, 1);
			// To display minute value on LCD
			HD44780_PutStr(minute);
			// To set the character cursor to column=7 and row=1
			HD44780_GotoXY(7, 1);
			HD44780_PutStr("min");
			// To set the character cursor to column=11 and row=1
			HD44780_GotoXY(11, 1);
			// To display second value on LCD
			HD44780_PutStr(second);
			// To set the character cursor to column=13 and row=1
			HD44780_GotoXY(13, 1);
			HD44780_PutStr("sec");
			count = 0;

		} else if (i == 2 && count == 1) {

			/*Seperate the latitude degree, latitude minutes and latitude decimal
			 degree into their own NULL terminated string buffers...*/
			latitudeStringToStore = latitude;
			//first two characters will be stored in degree

			for (l = 0; l < 2; l++) {
				latitudeDegreeString[l] = *(latitudeStringToStore + l);
			}

			//3rd and 4th will be stored in minute
			for (k = 0, l = 2; l < 4; l++, k++) {
				latitudeMinuteString[k] = *(latitudeStringToStore + l);

			}
			//5th will be excluded as it is '.' and rest will be stored in decimal degrees
			for (k = 0, l = 5; l < 9; l++, k++) {
				latitudeDecimalDegreeString[k] = *(latitudeStringToStore + l);
			}

			// to convert string to integer value
			latitudeDegree = atoi(latitudeDegreeString);
			latitudeMinute = atoi(latitudeMinuteString);
			latitudeDecimalDegree = atoi(latitudeDecimalDegreeString);

			// to convert integer values to string
			sprintf(latitudeDegreeStr, "%d", latitudeDegree);
			sprintf(latitudeMinuteStr, "%d", latitudeMinute);
			sprintf(latitudeDecimalDegreeStr, "%d", latitudeDecimalDegree);

			HD44780_ClrScr();
			// To set the character cursor to column=0 and row=0
			HD44780_GotoXY(0, 0);
			HD44780_PutStr("Latitude: ");
			HD44780_GotoXY(0, 1);
			// To display the latitude degree value on LCD
			HD44780_PutStr(latitudeDegreeStr);
			HD44780_GotoXY(4, 1);
			// To display the latitude minute value on LCD
			HD44780_PutStr(latitudeMinuteStr);
			HD44780_GotoXY(8, 1);
			// To display the latitude decimal degree value on LCD
			HD44780_PutStr(latitudeDecimalDegreeStr);
			HD44780_GotoXY(14, 1);
			// To display the latitude direction on LCD
			HD44780_PutStr(latitudeDirectionMeasured);
			count = 0;

		} else if (i == 3 && count == 1) {

			longitudeStringToStore = longitude;
			/*Seperate the longitude degree, longitude minutes and longitude decimal
			 degree into their own NULL terminated string buffers...*/
			//first three characters will be stored in degree
			for (l = 0; l < 3; l++) {
				longitudeDegreeString[l] = *(longitudeStringToStore + l);
			}
			//4th and 5th will be stored in minute
			for (k = 0, l = 3; l < 5; k++, l++) {
				longitudeMinuteString[k] = *(longitudeStringToStore + l);

			}
			//6th will be excluded as it is '.' and rest will be stored in decimal degrees
			for (k = 0, l = 6; l < 10; k++, l++) {
				longitudeDecimalDegreeString[k] = *(longitudeStringToStore + l);
			}

			// to convert string to integer value
			longitudeDegree = atoi(longitudeDegreeString);
			longitudeMinute = atoi(longitudeMinuteString);
			longitudeDecimalDegree = atoi(longitudeDecimalDegreeString);

			// to convert integer to string
			sprintf(longitudeDegreeStr, "%d", longitudeDegree);
			sprintf(longitudeMinuteStr, "%d", longitudeMinute);
			sprintf(longitudeDecimalDegreeStr, "%d", longitudeDecimalDegree);

			HD44780_ClrScr();
			// To set the character cursor to column=0 and row=0
			HD44780_GotoXY(0, 0);
			HD44780_PutStr("Longitude: ");
			// To set the character cursor to column=0 and row=1
			HD44780_GotoXY(0, 1);
			// To display the longitude degree value on LCD
			HD44780_PutStr(longitudeDegreeStr);
			// To set the character cursor to column=4 and row=1
			HD44780_GotoXY(4, 1);
			// To display the latitude minute value on LCD
			HD44780_PutStr(longitudeMinuteStr);
			// To set the character cursor to column=8 and row=1
			HD44780_GotoXY(8, 1);
			// To display the latitude decimal degree value on LCD
			HD44780_PutStr(longitudeDecimalDegreeStr);
			// To set the character cursor to column=4 and row=1
			HD44780_GotoXY(14, 1);
			// To display the latitude direction on LCD
			HD44780_PutStr(longitudeDirectionMeasured);
			count = 0;

		} else if (i == 4 && count == 1) {

			char altitudeString[10];
			// to convert string to float value
			float Altitude = atof(altitude);
			// to convert float value to string
			sprintf(altitudeString, "%.3f", Altitude);
			HD44780_PutStr("Altitude:");
			HAL_Delay(200);
			// To set the character cursor to column=0 and row=1
			HD44780_GotoXY(0, 1);
			// To display the altitude value on LCD
			HD44780_PutStr(altitudeString);
			// To set the character cursor to column=9 and row=1
			HD44780_GotoXY(9, 1);
			// To display the altitude units on LCD
			HD44780_PutStr(altitudeUnits);
			count = 0;

		} else if (i == 5 && count == 1) {

			char checksumString[20];
			char *HexCharacter = 0;
			// to convert float value to hexadecimal
			CheckSumDigit = strtoul(checksum + 1, &HexCharacter, 16);
			// to convert hexadecimal value to string
			sprintf(checksumString, "%d%s ", CheckSumDigit, HexCharacter);
			HD44780_PutStr("Checksum:");
			HAL_Delay(200);
			// To set the character cursor to column=0 and row=1
			HD44780_GotoXY(0, 1);
			// To display checksum value on LCD
			HD44780_PutStr(checksumString);
			// counter is set to zero
			i = 0;
			count = 0;
		}
	}
}

/* USER CODE END 3 */

/** System Clock Configuration
 */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB | RCC_PERIPHCLK_TIM8
			| RCC_PERIPHCLK_ADC12;
	PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
	PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	PeriphClkInit.Tim8ClockSelection = RCC_TIM8CLK_HCLK;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
static void MX_GPIO_Init(void) {

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOF_CLK_ENABLE()
	;
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;

}

/* USER CODE BEGIN 4 */
/* FUNCTION      : RGBColorMixer
 * DESCRIPTION   : This function mixes the colors of red, green, and blue using PWM generator as follows:
 * 					Red pin   ---> 150ohms --->PA3
 * 					Ground	  ---------------->GND
 *					Green pin ---> 100ohms --->PC9
 *					Blue pin  ---> 100ohms --->PF10
 * PARAMETERS    : redIntensity   : Unsigned 8bit integer (0-255)
 greenIntensity : Unsigned 8bit integer (0-255)
 blueIntensity  : Unsigned 8bit integer (0-255)
 * RETURNS       : NULL
 */
void RGBColorMixer(uint8_t redIntensity, uint8_t greenIntensity,
		uint8_t blueIntensity) {
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, redIntensity);
	__HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_4, greenIntensity);
	__HAL_TIM_SetCompare(&htim15, TIM_CHANNEL_2, blueIntensity);
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
	}
	/* USER CODE END Error_Handler */
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */

}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
