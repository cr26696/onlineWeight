/* 
  EEPROM.cpp - esp8266 EEPROM emulation

  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef EEPROM_h
#define EEPROM_h

#include <stddef.h>
#include <stdint.h>
#include <string.h>

class EepromVariable {
public:
    uint8_t address; // 在 EEPROM 中的起始地址
    uint8_t length;  // 数据的长度，包含起始地址字节

    // 构造函数
    EepromVariable(uint8_t addr, uint8_t len)
        : address(addr), length(len) {}
};
/**
 * 功能: 向 EEPROM 中添加一个变量
 * @param length: 数据的长度(uint8_t)
 * @param Variables: 变量对象数组(EepromVariable)
 */
void addVariable(uint8_t length,std::vector<EepromVariable> &Variables) {
    uint8_t address = 0;
    uint8_t variableCount = Variables.size();
    for (uint8_t i = 0; i < variableCount; i++) {
        address += Variables[i].length; // 计算下一个可用地址
    }
    Variables[variableCount].address = address;
    Variables[variableCount].length = length;
}
//----------------------------------------------------
class EEPROMClass {
public:
  EEPROMClass(uint32_t sector);
  EEPROMClass(void);

  void begin(size_t size);
  uint8_t read(int const address);
  void write(int const address, uint8_t const val);
  bool commit();
  bool end();

  uint8_t * getDataPtr();
  uint8_t const * getConstDataPtr() const;

  template<typename T> 
  T &get(int const address, T &t) {
    if (address < 0 || address + sizeof(T) > _size)
      return t;

    memcpy((uint8_t*) &t, _data + address, sizeof(T));
    return t;
  }

  template<typename T> 
  const T &put(int const address, const T &t) {
    if (address < 0 || address + sizeof(T) > _size)
      return t;
    if (memcmp(_data + address, (const uint8_t*)&t, sizeof(T)) != 0) {
      _dirty = true;
      memcpy(_data + address, (const uint8_t*)&t, sizeof(T));
    }

    return t;
  }

  size_t length() {return _size;}

  uint8_t& operator[](int const address) {return getDataPtr()[address];}
  uint8_t const & operator[](int const address) const {return getConstDataPtr()[address];}

protected:
  uint32_t _sector;
  uint8_t* _data = nullptr;
  size_t _size = 0;
  bool _dirty = false;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
extern EEPROMClass EEPROM;
#endif

#endif