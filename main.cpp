#include "mbed.h"
#include "TFC.h"

#include "common.h"
#include "Spices.h"

#define MAX_CAMERA_THRESHOLD 2500

uint32_t i,t = 0;
//uint32_t j;
float ReadPot0, ReadPot1;

void TFC_Race()
{
	
}
	
void TFC_TickerUpdate()
{
	int i;

	for(i=0; i<NUM_TFC_TICKERS; i++)
	{
		if(TFC_Ticker[i]<0xFFFFFFFF)
		{
			TFC_Ticker[i]++;
		}
	}
}

void TFC_LED_Servo_Test()
{
	//just tests the switches and LED's
	if(TFC_PUSH_BUTTON_0_PRESSED)
		TFC_BAT_LED0_ON;
	else
		TFC_BAT_LED0_OFF;

	if(TFC_PUSH_BUTTON_1_PRESSED)
		TFC_BAT_LED3_ON;
	else
		TFC_BAT_LED3_OFF;

	//test the servo motor
	TFC_HBRIDGE_DISABLE;

	//just move the servos with the on-board potentiometers
	if(TFC_Ticker[0]>=20) //every 40mS...
	{
		TFC_Ticker[0]=0;//reset the Ticker
		//update the Servos
		ReadPot0 = TFC_ReadPot(0);
		//ReadPot1 = TFC_ReadPot(1);
		TFC_SetServo(0,ReadPot0);
		//TFC_SetServo(1,ReadPot1);
		TERMINAL_PRINTF("Pot0 = %1.2f\r\n",ReadPot0);
		//TERMINAL_PRINTF("Pot1=%1.2f\r\n",ReadPot1);
	}
	//Let's put a pattern on the LEDs
	if(TFC_Ticker[1]>=125) //every 250mS...cycle through LEDs
	{
		TFC_Ticker[1]=0;
		t++;
		if(t>4)
		{
			t=0;
		}
		TFC_SetBatteryLED_Level(t);
	}

	TFC_SetMotorPWM(0,0); //Make sure motors are off
	TFC_HBRIDGE_DISABLE;
}

void TFC_Motor_Test()
{
	//test the motor
	TFC_HBRIDGE_ENABLE;

	ReadPot0=TFC_ReadPot(0);
	ReadPot1=TFC_ReadPot(1);
	//TERMINAL_PRINTF("Pot0 = %1.2f\r\n",ReadPot0);
	ReadPot1 = 2;
	TERMINAL_PRINTF("Pot1 = %1.2f\r\n",ReadPot1);
	TFC_SetMotorPWM(ReadPot0,ReadPot1);

	//Let's put a pattern on the LEDs
	if(TFC_Ticker[1]>=125)
	{
		TFC_Ticker[1]=0;
		t++;
		if(t>4)
		{
			t=0;
		}
		TFC_SetBatteryLED_Level(t);
	}
			
}

void TFC_Camera_Test()
{
	float ReadPot0;
	//test the Camera
	if(TFC_Ticker[0]>10&&TFC_LineScanImageReady>0) //125=250ms,20=40ms,1000=2000ms...
	{
		TFC_Ticker[0]=0;
		TFC_LineScanImageReady=0; //must reset to 0 after detecting non-zero

		if(t==0)
			t=4;
		else
			t--;

		TFC_SetBatteryLED_Level(t);
		
		ReadPot0 = (TFC_ReadPot(0) + 1)*1250;

		for(i=0;i<128;i++){
			
			//camera ONE (front connector)
			if((float)TFC_LineScanImage0[i]<ReadPot0)
				TERMINAL_PRINTF("|");
			else if((float)TFC_LineScanImage0[i]>ReadPot0)
				TERMINAL_PRINTF("_");

			/*camera ONE (front connector)
			if(TFC_LineScanImage0[i]<MAX_CAMERA_THRESHOLD)
				TERMINAL_PRINTF("|");
			else if(TFC_LineScanImage0[i]>MAX_CAMERA_THRESHOLD)
				TERMINAL_PRINTF("_");*/
					
			if(i==127) //when last data reached put in line return
				TERMINAL_PRINTF("  ");
				//TERMINAL_PRINTF("\r\n");
		}
		TERMINAL_PRINTF("%1.0f\r\n",ReadPot0);
		//TERMINAL_PRINTF("Pot1=%1.2f\r\n",ReadPot1);
	}
}

int main()
{
	//TERMINAL TYPE
	PC.baud(115200);//work swith Excel and TeraTerm
	//PC.baud(9600);//works with USBSerialMonitorLite: https://play.google.com/store/apps/details?id=jp.ksksue.app.terminal;doesn'twork>9600
	TFC_TickerObj.attach_us(&TFC_TickerUpdate,2000);//update ticker array every 2mS (2000uS)
	
	TFC_Init();
	
	for(;;)
	{

		switch(TFC_GetDIP_Switch())
		{
			case 1: //1000b
			//Test LED and Servo
			TFC_LED_Servo_Test();
			break;
			
			case 2: //0100b
			//Test Motor
			TFC_Motor_Test();
			break;
			
			case 4:  //0010b
			//Test Camera
			TFC_Camera_Test();
			break;
			
			case 8: //0001b
			//Race Mode
			MasterControlProgram();
			break;
			
			/*case 9: //1001b
			//Super_Race Mode
			TFC_Super_Race();
			break;*/
			
		}
	}//end of infinite for loop
}

