#include "hx711.h"
#include "EEPROM.h"

int GapValue = 213; 
unsigned long Weight_Maopi = 0;

int MAOPI_ADDR = 1000;//防止与wificonfig冲突
int CHECK_ADDR = 1010;
//****************************************************
//初始化HX711
//****************************************************
void Init_Hx711()
{
	pinMode(HX711_SCK, OUTPUT);
	pinMode(HX711_DT, INPUT);
	if(readWeightConfiFromEEPROM<char>(CHECK_ADDR) != 0xFF){
		GapValue = readWeightConfiFromEEPROM<int>(CHECK_ADDR);
		Weight_Maopi =  readWeightConfiFromEEPROM<unsigned long>(MAOPI_ADDR);
	}
	else{
		storeWeightConfiToEEPROM(MAOPI_ADDR,(unsigned long) 8573096);
		storeWeightConfiToEEPROM(CHECK_ADDR,(int)213);
	}
	Serial.print("EEPROM毛皮重量:");
	Serial.println(Weight_Maopi);
	Serial.print("EEPROM校准值:");
	Serial.println(GapValue);
}


//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi()
{   
    if(digitalRead(ZERO_BUTTON) == LOW){
        delay(50);
        if(digitalRead(ZERO_BUTTON) == LOW){
            Weight_Maopi = HX711_Read();
			storeWeightConfiToEEPROM(MAOPI_ADDR,Weight_Maopi);
			Serial.print("毛皮重量已记录:");
			Serial.println(Weight_Maopi);
        }
    }
	//return Weight_Maopi;		
}

void modifyGapValue(){
    if(digitalRead(CHECK_MINUS_BUTTON) == LOW){
        delay(50);
        if(digitalRead(CHECK_MINUS_BUTTON) == LOW){
			GapValue = --GapValue>0? GapValue : 1;
			storeWeightConfiToEEPROM(CHECK_ADDR,GapValue);
			Serial.print("GapValue has been modified:");
			Serial.println(GapValue);
        }
    }
	if(digitalRead(CHECK_PLUS_BUTTON) == LOW){
        delay(50);
        if(digitalRead(CHECK_PLUS_BUTTON) == LOW){
			GapValue++;
			storeWeightConfiToEEPROM(CHECK_ADDR,GapValue);
			Serial.print("GapValue has been modified:");
			Serial.println(GapValue);
        }
    }
}

//****************************************************
//称重 单位 克
//****************************************************
unsigned long Get_Weight()
{
	unsigned long Weight_Shiwu = HX711_Read();
	return Weight_Shiwu > Weight_Maopi? (unsigned long)(floor((Weight_Shiwu - Weight_Maopi)/(float)GapValue)) : 0;				//获取实物的克
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

template<typename valueType>
void storeWeightConfiToEEPROM(int addr, valueType val){
    if (addr + sizeof(valueType) > EEPROM.length()) {
        Serial.println("EEPROM write out of range");
        return;
    }	
	for(unsigned int i = 0; i < sizeof(val); i++){
		EEPROM.write(addr + i, (val >> (i * 8)) & 0xFF);
	}
	EEPROM.commit();
}

template<typename valueType>
valueType readWeightConfiFromEEPROM(int addr){
    if (addr + sizeof(valueType) > EEPROM.length()) {
        Serial.println("EEPROM write out of range");
        return 0;
    }
    valueType val = 0;
    for (unsigned int i = 0; i < sizeof(val); i++) {
        val |= (valueType)(EEPROM.read(addr + i)) << (i * 8);
    }
	return val;	
}

void weightConfig(){
	Get_Maopi();
	modifyGapValue();
}