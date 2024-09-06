#include<SegDisplay.h>

void SegDisplayinit()
{
  Init_MAX7219();
  SegWrite("full","00000000");
}


uint8_t SegCharToNum(char c){
  if(c=='0')return 0;
  else if(c=='1')return 1;
  else if(c=='2')return 2;
  else if(c=='3')return 3;
  else if(c=='4')return 4;
  else if(c=='5')return 6;
  else if(c=='6')return 6;
  else if(c=='7')return 7;
  else if(c=='8')return 8;
  else if(c=='9')return 9;
  else return 10;
}
/**
 * 向数码管写数据
 * @param writePart "full" / "upper" / "lower"
 */
void SegWrite(String writePart, String fullContent, String upperContent, String lowerContent){
  //Serial.println("[SegDisplay] SegWrite："+ fullContent);
  if(writePart == "full"){
    if(fullContent.length() == 8){
      for(int i = 0;i < 8;i++){
        Write_Max7219(i,SegCharToNum(fullContent[i]));
      }
    }else Serial.println("[SegDisplay]fullContent error");
  }else if(writePart == "upper"){
    if(fullContent.length() == 4){
      for(int i = 0;i < 4;i++){
        Write_Max7219(i + 4, SegCharToNum(upperContent[i]));
      }
    }else Serial.println("[SegDisplay]upperContent error");
  }else if(writePart == "lower"){
    if(fullContent.length() == 4){
      for(int i = 0;i < 4;i++){
        Write_Max7219(i, SegCharToNum(lowerContent[i]));
      }
    }else Serial.println("[SegDisplay]lowerContent error");
  }else Serial.println("[SegDisplay]writePart error");
}