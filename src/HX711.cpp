#include "hx711.h"

int GapValue = 213;

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
unsigned long Get_Maopi(unsigned long Weight_Maopi)
{   
    if(digitalRead(D6) == LOW){
        delay(20);
        if(digitalRead(D6) == LOW){
            return HX711_Read();
        }
    }
	return Weight_Maopi;		
} 

//****************************************************
//称重
//****************************************************
unsigned long Get_Weight(unsigned long Weight_Maopi)
{
	unsigned long Weight_Shiwu = HX711_Read();
    //unsigned long Weight_Chen = HX711_Read();
	return Weight_Shiwu > Weight_Maopi? (Weight_Shiwu - Weight_Maopi)/GapValue : 0;				//获取实物的克重
	//Weight_Shiwu = ((Weight_Shiwu/GapValue)); //- OFFSET_VAL); 	
	//return Weight_Shiwu; //显示克重
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