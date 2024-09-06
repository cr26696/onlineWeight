#include <Arduino.h>
#include "MAX7219.h"

#define SEGDISPLAY_Debug 0;
//****************************************************
// 初始化MAX7219
//****************************************************
void Init_MAX7219()
{
  pinMode(Max7219_pinCLK, OUTPUT);
  pinMode(Max7219_pinDIN, OUTPUT);
  pinMode(Max7219_pinCS, OUTPUT);
  Serial.println("Init_MAX7219");
  Write_Max7219(0x09, 0xff); // 译码方式：BCD码
  Write_Max7219(0x0a, 0x03); // 亮度
  Write_Max7219(0x0b, 0x07); // 扫描界限；4个数码管显示
  Write_Max7219(0x0c, 0x01); // 掉电模式：0，普通模式：1
  Write_Max7219(0x0f, 0x00); // 显示测试：1；测试结束，正常显示：0
}
//--------------------------------------------
// 功能：向MAX7219(U3)写入字节
// 入口参数：DATA
// 出口参数：无
// 说明：
void Write_Max7219_byte(byte DATA)
{
  uint8_t i;
  {
    digitalWrite(Max7219_pinCS, LOW);
    // delayMicroseconds(1);

    // Serial.println("[Max7219_instruct] CSpullLow");
    for (i = 8; i >= 1; i--)
    {
      digitalWrite(Max7219_pinCLK, LOW);
      // delayMicroseconds(1);
      digitalWrite(Max7219_pinDIN, (DATA & 0x80) ? HIGH : LOW);
      DATA <<= 1;
      digitalWrite(Max7219_pinCLK, HIGH);
      // delayMicroseconds(1);
    }
    // digitalWrite(Max7219_pinCS, HIGH);
    // Serial.println("[Max7219_instruct] CSpullHigh");
  }
}
/**
 * 向MAX7219驱动器写入一个字节的数据
 * 
 * @param address MAX7219驱动器的地址，用于指定目标数码管(byte)
 * @param dat 要显示的数字(byte)
 */
  void Write_Max7219(byte address, byte dat)
  {
    digitalWrite(Max7219_pinCS, LOW);
    // Serial.print("[MAX7219]address: ");
    // Serial.print(address);
    // Serial.print(" ,data: ");
    // Serial.println(dat);
    Write_Max7219_byte(address); // 写入地址，即数码管编号
    Write_Max7219_byte(dat);     // 写入数据，即数码管显示数字
    digitalWrite(Max7219_pinCS, HIGH);
  }
