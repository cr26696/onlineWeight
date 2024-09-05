#include "hx711.h"

#define GapValue 213 //

//****************************************************
//初始化HX711
//****************************************************
void Init_Hx711()
{
	pinMode(HX711_SCK, OUTPUT);
	pinMode(HX711_DT, INPUT);
}


//****************************************************
//获取毛皮重量
//****************************************************
unsigned long Get_Maopi()
{
	return HX711_Read();		
} 

/**
 * 功能: 获取重量
 * @param grossWeight: 毛重
 */
unsigned long Get_Weight(unsigned long grossWeight)
{
	unsigned long netWeight = 0; //净重
	netWeight = HX711_Read() - grossWeight;				//获取实物的AD采样数值。
	netWeight = ((netWeight/GapValue) - OFFSET_VAL); 	
	return netWeight; //显示克重
}

//****************************************************
//读取HX711
//****************************************************
unsigned long HX711_Read(void)	//增益128
{
	unsigned long count; 
	unsigned char i;

	digitalWrite(HX711_SCK, LOW);
	delayMicroseconds(1);

  	count=0; 
  	while(digitalRead(HX711_DT)); 
  	for(i=0;i<24;i++)
	{ 
	  	digitalWrite(HX711_SCK, HIGH); 
		delayMicroseconds(1);
	  	count=count<<1; 
		digitalWrite(HX711_SCK, LOW); 
		delayMicroseconds(1);
	  	if(digitalRead(HX711_DT)){
			count++;
		}
			 
	} 
 	digitalWrite(HX711_SCK, HIGH);
	count ^= 0x800000;
	delayMicroseconds(1);
	digitalWrite(HX711_SCK, LOW);
	delayMicroseconds(1);
	
	return(count);
}
