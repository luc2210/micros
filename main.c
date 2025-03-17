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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

int t_inicial1=0;// se guarda el tiepo inicial cuando se produce el flanco de subida en el eco
int t_final1=0;// se guarda cuando se detecta un flanco de bajad aen ele eco
int encendido1=0; // esto espera a que el eco se encienda es decir cuando se detecta un flanco de subida en el edo
int distancia1 =0;
int t_inicial2=0;// se guarda el tiepo inicial cuando se produce el flanco de subida en el eco
int t_final2=0;// se guarda cuando se detecta un flanco de bajad aen ele eco
int encendido2=0; // esto espera a que el eco se encienda es decir cuando se detecta un flanco de subida en el edo
int distancia2 =0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void trigger_ON(){

GPIOC->BSRR |= (1<<6);
}

void trigger_OFF(){
GPIOC->BSRR |= (1<<(6+16));

}

void buzzerNO_SOUND(){

GPIOB->BSRR |= (1<<8);

}

void buzzerSOUND(){

GPIOB->BSRR |=(1<<(8+16));

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
void TIM2_IRQHandler(void){

	if ( TIM2->SR & TIM_SR_CC2IF){ // COMPRUBEA SI HUBO UNA CAPTURA EN EL CANAL 2
		if ( encendido1==0){
			TIM2->CNT=0; //contador a 0, para contar el tiempo entre dos eventos
			//t_inicial1=TIM2->CCR2; // SE GUARDA TODX EL RATO EL TIEMPO PARA CUANDO SE PRODUZCA EL FLANCO DE SUBIDA SE QUEDE GUARDADO EN LA VARIABLE
			TIM2->CCER &= ~(1 << 1); // Pone a 0 el bit CC2P (captura en flanco de bajada)
			encendido1=1; // SE PONE A UNO PORQUE HEMSO HECHO UN FLANCO DE BAJADA

		}else{

			encendido1=0;
			TIM2->CCER |= (1<<1); //VOLVEMOS AL FLANCO DE SUBIDA
			t_final1=TIM2->CCR2; // COGE EEL TIEMPO FINAL es decir la duraciond del proyecto
		}
		TIM2->SR &= ~(1 << 2); // Limpiar la bandera CC2IF

	}
	else if ( TIM2->SR & TIM_SR_CC3IF){ // COMPRUBEA SI HUBO UNA CAPTURA EN EL CANAL 3
			if ( encendido2==0){

				t_inicial2=TIM2->CCR3; // SE GUARDA TODX EL RATO EL TIEMPO PARA CUANDO SE PRODUZCA EL FLANCO DE SUBIDA SE QUEDE GUARDADO EN LA VARIABLE
				TIM2->CCER &= ~(1 << 9);
				encendido2=1; // SE PONE A UNO PORQUE HEMSO HECHO UN FLANCO DE BAJADA

			}else{

				t_final2=TIM2->CCR3; // COGE EEL TIEMPO FINAL
				TIM2->CCER |= (1<<9); //VOLVEMOS AL FLANCO DE SUBIDA
				encendido2=0;
				if( t_final2>t_inicial2){
				distancia2= (t_final2-t_inicial2)*	0.0343 / 2;
				}
			}
			TIM2->SR &= ~(1 << 3); // Limpiar la bandera CC3IF

		}

}

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
  /* USER CODE BEGIN 2 */

  GPIOC->MODER |= ( 2 << (6*2)); //pin c6 output trigger delantero 01
  GPIOC->MODER &= ~(2<<(7*2)); // PIN C7 MODO INPUT 11 echo delantero
  GPIOC->MODER |= ( 2 << (8*2)); //pin c8 output trigger TRASERO 01
  GPIOC->MODER &= ~(2<<(9*2)); // PIN C9 MODO INPUT 11 echo 

// PB8-ALTERNATE FUNCTION
  GPIOB->MODER |= (1 << (8*2 + 1)); //pin B8 COMO SALIDA PARA EL ZUMBADOR COLOCA UN 1
  GPIOB->MODER &= ~(1 << (8 * 2));  // Limpia bits ES DECIR LO PONE A 0
 

  //PC7 GENERARA UNA INTERRUCION AL DETECTAR UN PULSO DEL ECHO
  //LO QUE HARÁ SERÁ DETECTAR CUANDO HAYA UN FLANCO DE BAJADA, ES DECIR CUANDO TERMINA EL PULSO

  EXTI->IMR |= (1<<7); // ESTO HABILITA EL PC7 PARA INTERRUPCIONES
  EXTI->RTSR |= (1<<7); // HABILITA EL EVENTO POR FLANCO DE SUBIDA
  EXTI->FTSR |= (1<<7); // HABILITA EL EVENTO POR FLACO DE BAJADA

  EXTI->IMR |= (1<<9); // ESTO HABILITA EL PC9 PARA INTERRUPCIONES
  EXTI->RTSR |= (1<<9); // HABILITA EL EVENTO POR FLANCO DE SUBIDA
  EXTI->FTSR |= (1<<9); // HABILITA EL EVENTO POR FLACO DE BAJADA
  NVIC->ISER[0]= (1<<28); // HABILITAR TIM2 EN NVIC IRQ28

  //PONEMOS EL PREESCALADO EN 10US

  TIM2->PSC = 8-1;
  TIM2->ARR = 0xFFFF; // MAXIMO VALOR DEL CONTADOR PORQUE VAMOS A USAR TIC

  TIM2->CCMR1 &= ~(3 << 8); // LIMPIA LOS BITS DE CC2
  TIM2->CCMR1 &= ~(3 << 16);
  TIM2->CCMR1 |= (1 << 8) | (1<<16); //MODO TIC 01 EN CH2 Y CH3
  TIM2->CCER |= TIM_CCER_CC2E | TIM_CCER_CC3E; // CAPTURA EL FLANCO DE BAJADA, DESPIERTA QUE TIENES QUE EMPEZAR A CONTAR
  TIM2->DIER |= TIM_DIER_CC2IE | TIM_DIER_CC3IE; //HABILITAR INTERRUCION DE CAPTURA, QUEIRO QUE HAGAS LA INTERRUPCION
  TIM2->CR1 |= TIM_CR1_CEN; //ACTIVA EL TIMER
  NVIC->ISER[0] |= (1<<28);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	  // EVIAMOS UN PULSO ESTO LO QUE HACE ES ACTIVAR EL ECO
	  	  GPIOC->BSRR = ( 1<<6); // PONEMOS EL TRIGGER EN ALTO
	  	  for ( int i =0; i<80;i++);// ESPERAMOS 10 US PARA QUE SE CUENTE COMO UN  PULSO
	  	  GPIOC->BSRR = (1<< (6 +16)); // Y VOLVEMOS A PONERLO EN BAJO

	  	  GPIOC->BSRR = ( 1<<8); // PONEMOS EL TRIGGER EN ALTO
	  	  for ( int i =0; i<80;i++);// ESPERAMOS 10 US
	  	  GPIOC->BSRR = (1<< (8 +16)); // Y VOLVEMOS APONERLO EN BAJO


	  	  if(distancia1<5|| distancia2<5){

	  		  GPIOB->BSRR =(1<<(8));  //ACTIVA AEL ZUMBADOR DE MANERA CONTINUA

	  	  }else if ((distancia1>=5 && distancia1<=30)||(distancia2>=5 && distancia2<=30)){

	  	GPIOB->BSRR =(1<<8); // ACTIVA EL ZUMBADO DE MANERA INTERMITENTE
	  	for ( int i=0; i<100; i++);

	  	GPIOB->BSRR =(1<<(8+16));
	  	for ( int i=0; i<100; i++);

	  	  }else{

	  			GPIOA->BSRR =(1<<8);  // EL ZUMBADOR EST APAGADO

	  	  }
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

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
