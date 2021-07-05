
template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}


// Variables we need to store:
// - light levels cool/warm
// - hour1
// - hour2
// - fan duration
// - fan interval
// We'll start with those

void writeToEEPROM() {
  // Write data to EEPROM so that we can go into deep sleep
  int addr = 0;
  int addr_shift = EEPROM_writeAnything(addr, 1234); // identifier to signify data is present
  addr = addr + addr_shift;

  // Variables must be written in the same order in which they will be read later
    for (int i = 0; i < n_lights; i++) {
      addr = addr + EEPROM_writeAnything(addr, cool_level_set[i]);
      addr = addr + EEPROM_writeAnything(addr, warm_level_set[i]);
    }
    addr = addr + EEPROM_writeAnything(addr, hour1);
    addr = addr + EEPROM_writeAnything(addr, hour2);
    addr = addr + EEPROM_writeAnything(addr, fan_duration);
    addr = addr + EEPROM_writeAnything(addr, fan_interval);
   
  EEPROM.commit(); // commit changes;
}

bool readFromEEPROM() {
  int addr = 0;
  int identifier;

  addr = addr +  EEPROM_readAnything(addr, identifier); // read back value to hist_cnt
  Serial.print(addr); Serial.print("\t");  Serial.println(identifier);
  if (identifier != 1234) {
    return false;
  }

  // Variables must be read in the same order in which they were written
  for (int i = 0; i < n_lights; i++) {
    addr = addr + EEPROM_readAnything(addr, cool_level_set[i]);
    Serial.print(addr); Serial.print("\t");  Serial.println(cool_level_set[i]);
    addr = addr + EEPROM_readAnything(addr, warm_level_set[i]);
    Serial.print(addr); Serial.print("\t");  Serial.println(warm_level_set[i]);
  }
  addr = addr + EEPROM_readAnything(addr, hour1);
  Serial.print(addr); Serial.print("\t");  Serial.println(hour1);
  addr = addr + EEPROM_readAnything(addr, hour2);
  Serial.print(addr); Serial.print("\t");  Serial.println(hour2);
  addr = addr + EEPROM_readAnything(addr, fan_duration);
  Serial.print(addr); Serial.print("\t");  Serial.println(fan_duration);
  addr = addr + EEPROM_readAnything(addr, fan_interval);
  Serial.print(addr); Serial.print("\t");  Serial.println(fan_interval);
  
  
  
  return true;
}
