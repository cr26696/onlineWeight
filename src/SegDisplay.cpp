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
void SegWrite(String writePart, String str){
  //Serial.println("[SegDisplay] SegWrite："+ str);
  int i = 1;//默认起始Str
  int SegDisEndNumAddr = 8;//默认结束
  int strlen = str.length();
  if(writePart == "full"){
    if(strlen != 8) Serial.println("[SegDisplay]fullContent error"+str);
  }else {
    //半边控制
    if(strlen != 4){Serial.println("[SegDisplay]" + writePart + "Content error,content: " + str);} 
    else{
      if(writePart == "upper") i = 5;
      else if(writePart == "lower") SegDisEndNumAddr = 4;
      else Serial.println("[SegDisplay]writePart error");
    }
  }
  for(;i <= SegDisEndNumAddr; i++,strlen--){
    Write_Max7219(i,SegCharToNum(str[strlen - 1]));
  }
}
