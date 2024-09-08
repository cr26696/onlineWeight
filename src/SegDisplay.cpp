#include<SegDisplay.h>
//构造函数
SegDisplay::SegDisplay()
{
  Init_MAX7219();
}
//完全刷新
void SegDisplay::refresh(){
  for(int i = 0; i < 8; i++){
    if(_needRefresh[i]){
      Write_Max7219(i+1,_numbers[i]);
      _needRefresh[i] = false;
    }
  }
}
/**
 * 从字符串识别改变哪些变量
 * @param str 要识别的字符串，长度8
 */
void SegDisplay::updateParam(String str){
  byte length = str.length();
  if(length == 8){
    for(int i = 0 ; i<8; i++)
    {
      _needRefresh[i] = str[i] != _numbers[length - i - 1];
      _numbers[length - i - 1] = str[i];
    }
  }else{
    Serial.println("[SegDisplay]String length error");
  }
}
/**
 * @param Content 写内容,8位String
 */
void SegDisplay::display(String Content){
  updateParam(Content);
}
/**
 * 向数码管写数据
 * @param writePart 操作部分 "upper" / "lower"
 * @param str 写入内容,4位String
 */
void SegDisplay::display(String writePart, String str){
  if(writePart == "upper"){
    str = "0000" + str;
  }
  updateParam(Content);
}
