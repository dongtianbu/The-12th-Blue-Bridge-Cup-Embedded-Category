#include "headfile.h"

uint8_t ButtonState[4] = {1, 1, 1, 1};
uint8_t ButtonLastState[4] = {1, 1, 1, 1};
uint8_t LongPress[4] = {0, 0, 0, 0};

uint8_t ButtonScan(void)
{
	for (int i = 0; i < 4; i++)
	{
		ButtonLastState[i] = ButtonState[i];
	}
	
	ButtonState[0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	ButtonState[1] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	ButtonState[2] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
	ButtonState[3] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	
	for (int i = 0; i < 4; i++)
	{
		if (ButtonLastState[i] == 1 && ButtonState[i] == 0)
		{
			HAL_Delay(20);
			TIM15 -> CNT = 0;
		}
		//长按识别
		else if (ButtonLastState[i] == 0 && ButtonState[i] == 0)
		{
			if (TIM15 -> CNT >= 10000 && LongPress[i] == 0)
			{
				LongPress[i] = 1;
				return (i + 11);
			}
			
		}
		else if (ButtonLastState[i] == 0 && ButtonState[i] == 1)
		{
			HAL_Delay(20);
			if (LongPress[i] == 1)
			{
				LongPress[i] = 0;
			}
			else
			{
				return (i + 1);
			}
		}
	}

	return 0;
}



void LedControl(uint8_t Ledx, uint8_t LedState)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	
	if (LedState)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 << (Ledx - 1), GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 << (Ledx - 1), GPIO_PIN_SET);

	}
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

void LedClose(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	for (int i = 0; i < 8; i++)
	{
		LedControl(i + 1, 0);
	}
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}



uint8_t Text[20] = {0};
uint8_t UserLcdInit[2] = {1, 1};

double CNBR = 3.5;
double VNBR = 2.0;

uint8_t NumOfCNBR = 2;
uint8_t NumOfVNBR = 4;
uint8_t NumOfIDLE = 2;

void UserLcdInit1(void)
{
	LCD_Clear(Black);
	LCD_SetTextColor(White);
	LCD_SetBackColor(Black);
	sprintf((char*)Text, "       Data         ");
	LCD_DisplayStringLine(Line1, Text);
	sprintf((char*)Text, "   CNBR:%d           ", NumOfCNBR);
	LCD_DisplayStringLine(Line3, Text);
	sprintf((char*)Text, "   VNBR:%d           ", NumOfVNBR);
	LCD_DisplayStringLine(Line5, Text);	
	sprintf((char*)Text, "   IDLE:%d           ", NumOfIDLE);
	LCD_DisplayStringLine(Line7, Text);		

}

void UserLcdInit2(void)
{
	LCD_Clear(Black);
	LCD_SetTextColor(White);
	LCD_SetBackColor(Black);
	sprintf((char*)Text, "       Para         ");
	LCD_DisplayStringLine(Line1, Text);
	
	sprintf((char*)Text, "   CNBR:            ");
	LCD_DisplayStringLine(Line3, Text);
	sprintf((char*)Text, "   VNBR:            ");
	LCD_DisplayStringLine(Line5, Text);
}

void LcdShowData(void)
{
	if (UserLcdInit[0])
	{
		UserLcdInit1();
		//进入一个界面时打开另一个界面的初始化
		UserLcdInit[0] = 0;
		UserLcdInit[1] = 1;
	}
	
}

void LcdShowPara(void)
{
	if (UserLcdInit[1])
	{
		
		UserLcdInit2();
		UserLcdInit[0] = 1;
		UserLcdInit[1] = 0;
	}
	sprintf((char*)Text, "   CNBR:%.2f        ", CNBR);
	LCD_DisplayStringLine(Line3, Text);
	sprintf((char*)Text, "   VNBR:%.2f        ", VNBR);
	LCD_DisplayStringLine(Line5, Text);
	
}

uint8_t Rx = 0;
uint8_t RxIndex = 0;
uint8_t RxPack[30] = {' '};
char DataCut1[30] = {0};
char DataCut2[30] = {0};
uint8_t DataTimeCut[30] = {' '};
uint8_t SuccessRx = 0;
uint8_t ChargeFlag = 0;

uint8_t DataInitFlag = 0;

uint8_t Temp = 0;
uint8_t TimeBuffer[20] = {0};
uint8_t Time = 0;

uint16_t InYear = 0, OutYear = 0, InMonth = 0, OutMonth = 0, InDay = 0, OutDay = 0;
uint16_t InHour = 0, OutHour = 0, InSenconds = 0, InMinSenconds = 0, OutMinSenconds = 0;
uint8_t RxFlag = 0;

char test[100] = {0};

char ParkingCarNum[4] = {0};
uint8_t TimeCut[30] = {0};

char TypeOfParking[5] = {' ', ' ', ' ', ' ', '\0'};
char NumOfCar[5] = {' ', ' ', ' ', ' ', '\0'};
uint64_t ParkingTime = 0;
uint64_t OutTime = 0;

double Price = 0;

uint8_t FlashFlag = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	TIM15 -> CNT = 0;
	RxFlag = 1;
	RxPack[RxIndex] = Rx;
	RxIndex++;
	HAL_UART_Receive_IT(&huart1, &Rx, 1);
}

void RecordParkingType(char* DataCut, char* TypeOfParking)
{
	for (int i = 0; i < 4; i++)
	{
		TypeOfParking[i] = DataCut[i];
	}
}

void RecordNumOfCar(char* DataCut, char* NumOfCar)
{
	for (int i = 0; i < 4; i++)
	{
		NumOfCar[i] = DataCut[i + 6];
	}
}

uint64_t RecordParkingTime(uint8_t* DataTimeCut)
{
	uint64_t ParkingTime = 0;
	for (int i = 0; i < 12; i++)
	{
		ParkingTime = ParkingTime * 10 + DataTimeCut[i];
	}
	return ParkingTime;
}

uint64_t RecordOutTime(uint8_t* DataTimeCut)
{
	uint64_t OutTime = 0;
	for (int i = 0; i < 12; i++)
	{
		OutTime = OutTime * 10 + DataTimeCut[i];
	}
	return OutTime;
}

void ProcessData(void)
{
	if (DataInitFlag == 0)
	{
		for (int i = 0; i < 30; i++)
		{
			RxPack[i] = ' ';
			DataTimeCut[i] = ' ';
		}
		DataInitFlag = 1;
	}
	uint8_t Format = 1;
	//接收完数据后
	if (TIM15 -> CNT >= 15 && RxFlag == 1)
	{
		//接收进停车场信息
		if (SuccessRx == 0)
		{
			for(int i = 0; i < 5; i++)
			{
				DataCut1[i] = RxPack[i];
			}
			DataCut1[5] = '\0';//第一个冒号后加\0
			if (strcmp((char*)DataCut1, "CNBR:") != 0 && strcmp((char*)DataCut1, "VNBR:") != 0)
			{
				Format = 0;
				//调试代码用
				HAL_UART_Transmit(&huart1, (uint8_t*)"CNBR1ERROR!\r\n", 14, 50);
			}
			for(int i = 6; i <= 10; i++)//到第二个冒号
			{
				DataCut1[i] = RxPack[i - 1];
			}
			
			//时间数据截取
			//末尾时间数据22
			for(int i = 11; i <= 22; i++)
			{
				//时间数据正确性判断
				if (RxPack[i - 1] >= '0' && RxPack[i - 1] <= '9')
				{
				DataTimeCut[i - 11] = RxPack[i - 1] - '0';				
				}
				else
				{
					Format = 0;
					break;
				}

			}
			
			//数据过短
			if (RxPack[21] == ' ')
			{
				sprintf(test, "TimeShortError1\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t*)test, strlen(test), 50);
				HAL_Delay(50);
				Format = 0;
			}
			//数据过长
			if (RxPack[22] != ' ')
			{
				sprintf(test, "TimeLongError1\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t*)test, strlen(test), 50);
				HAL_Delay(50);
				Format = 0;
			}

			//字符串格式判断
			if (DataCut1[4] != ':' || DataCut1[10] != ':')
			{
				HAL_UART_Transmit(&huart1, (uint8_t*)"1:ERROR!\r\n", 11, 50);
				Format = 0;
			}
			

			if (Format == 0)
			{
				//error，返回Format1Error!\r\n与后面的区分
				HAL_UART_Transmit(&huart1, (uint8_t*)"Format1Error!\r\n", 16, 50);
			}
			else if (Format == 1)
			{
				SuccessRx = 1;
				//存储各类数据
				RecordParkingType(DataCut1, TypeOfParking);
				RecordNumOfCar(DataCut1, NumOfCar);
				ParkingTime = RecordParkingTime(DataTimeCut);
				
				for (int i = 0; i < 2; i++)
				{
					InYear = InYear * 10 + DataTimeCut[i];
					InMonth = InMonth * 10 + DataTimeCut[i + 2];
					InDay = InDay * 10 + DataTimeCut[i + 4];
					InHour = InHour * 10 + DataTimeCut[i + 6];
				}
				for (int i = 0; i < 4; i++)
				{
					InMinSenconds = InMinSenconds * 10 + DataTimeCut[i + 8];//20 02 02 06 35 20
				}
				
				sprintf(test, "ParkingSuccess!\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t*)test, strlen(test), 50);
			}
			
		}
		else if (SuccessRx == 1)
		{
			for(int i = 0; i < 5; i++)
			{
				DataCut2[i] = RxPack[i];
			}
			DataCut2[5] = '\0';//第一个冒号后加\0

			if (strcmp((char*)DataCut2, "CNBR:") != 0 && strcmp((char*)DataCut2, "VNBR:") != 0)
			{
				HAL_UART_Transmit(&huart1, (uint8_t*)"CNBR2ERROR!\r\n", 14, 50);
				Format = 0;
			}
			for(int i = 6; i <= 10; i++)
			{
				DataCut2[i] = RxPack[i - 1];
			}
			//时间数据截取
			for(int i = 11; i <= 22; i++)
			{
				//时间数据正确性判断
				if (RxPack[i - 1] >= '0' && RxPack[i - 1] <= '9')
				{
				DataTimeCut[i - 11] = RxPack[i - 1] - '0';				
				}
				else
				{
					Format = 0;
					break;
				}

			}


			
			//时间数据过短
			if (RxPack[21] == ' ')
			{
				sprintf(test, "TimeShortError2\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t*)test, strlen(test), 50);
				HAL_Delay(50);
				Format = 0;
			}
			//数据过长
			if (RxPack[22] != ' ')
			{
				sprintf(test, "TimeLongError2\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t*)test, strlen(test), 50);
				HAL_Delay(50);
				Format = 0;
			}

			//字符串格式判断
			if (DataCut2[4] != ':' || DataCut2[10] != ':')
			{
				HAL_UART_Transmit(&huart1, (uint8_t*)"2:ERROR!\r\n", 11, 50);
				Format = 0;
			}
			
			if (Format == 0)
			{
				//error
				HAL_UART_Transmit(&huart1, (uint8_t*)"Format2Error\r\n", 16, 50);
			}
			else if (Format == 1)
			{
				SuccessRx = 2;
				//存储各类数据
				RecordParkingType(DataCut2, TypeOfParking);
				RecordNumOfCar(DataCut2, NumOfCar);
				OutTime = RecordOutTime(DataTimeCut);
				
				for (int i = 0; i < 2; i++)
				{
					OutYear = OutYear * 10 + DataTimeCut[i];
					OutMonth = OutMonth * 10 + DataTimeCut[i + 2];
					OutDay = OutDay * 10 + DataTimeCut[i + 4];
					OutHour = OutHour * 10 + DataTimeCut[i + 6];
				}
				for (int i = 0; i < 4; i++)
				{
					OutMinSenconds = OutMinSenconds * 10 + DataTimeCut[i + 8];//20 02 02 06 35 20
				}
				ChargeFlag = 1;
				sprintf(test, "OutSuccess!\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t*)test, strlen(test), 50);
			}
			
		}

		RxFlag = 0;

		memset(RxPack, ' ', 30);
		memset(DataCut1, ' ', 30);
		memset(DataCut2, ' ', 30);
		memset(DataTimeCut, ' ', 30);
		RxIndex = 0;
	}
	
}

void OutputCharge(void)
{
		//输出停车费
	if (ChargeFlag)
	{
		static uint64_t Hour;
		
		if (OutTime > ParkingTime)
		{
			
			Hour = (OutYear - InYear) * 365 * 24 + (OutMonth - InMonth) * 30 * 24 + (OutDay - InDay) * 24 + 
						  OutHour - InHour + 1;
			if (strcmp(TypeOfParking, "VNBR") == 0)
			{
				Price = Hour * VNBR;
				NumOfVNBR--;
				NumOfIDLE++;
				FlashFlag = 1;
				sprintf(test, "VNBR:%c%c%c%c:%llu:%.2f\r\n", NumOfCar[0], NumOfCar[1], NumOfCar[2], NumOfCar[3],
				Hour, Price);
				HAL_UART_Transmit(&huart1, (uint8_t*)test, strlen(test), 1000);
				HAL_Delay(50);
				ChargeFlag = 0;
			}
			else if (strcmp(TypeOfParking, "CNBR") == 0)
			{
				Price = Hour * CNBR;
				NumOfCNBR--;
				NumOfIDLE++;
				FlashFlag = 1;
				sprintf(test, "CNBR:%c%c%c%c:%llu:%.2f\r\n", NumOfCar[0], NumOfCar[1], NumOfCar[2], NumOfCar[3],
				Hour, Price);
				HAL_UART_Transmit(&huart1, (uint8_t*)test, strlen(test), 1000);
				HAL_Delay(50);
				ChargeFlag = 0;
			}
			
		}
		else
		{
			
			sprintf(test, "ParkingTimeError!%llu %llu\r\n", OutTime, ParkingTime);
			HAL_UART_Transmit(&huart1, (uint8_t*)test, strlen(test), 50);
			HAL_Delay(50);
//			SuccessRx  = 0;
		}

	}
}

