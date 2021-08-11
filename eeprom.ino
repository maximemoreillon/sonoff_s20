#define EEPROM_END_BYTE 0x0A
#define EEPROM_MAX_ITEM_LENGTH 50

void write_string_to_eeprom(String string_to_write, int start_position){
  
  for(int byte_index=0; byte_index<string_to_write.length(); byte_index++){
    
    int position_to_write = start_position + byte_index;
    byte value_to_write = string_to_write[byte_index];
    
    EEPROM.write(position_to_write, value_to_write);
  }

  // Add a line feed character to mark the end of the string
  int position_to_write = string_to_write.length() + start_position;

  // Write to the EEPROM
  EEPROM.write(position_to_write, EEPROM_END_BYTE);

  // Save what's been written
  if (EEPROM.commit()) {
    Serial.println("[EEPROM] commit successful");
  }
  else {
    Serial.println("[EEPROM] commit FAILED");
  }
}

String read_string_from_eeprom(int start_position){
  
  String output = "";
  
  for(int byte_index=0; byte_index<EEPROM_MAX_ITEM_LENGTH; byte_index++){
    
    int position_to_read = start_position + byte_index;
    
    byte value = EEPROM.read(position_to_read);
    
    if(value == EEPROM_END_BYTE) {
      break;
    }
    else {
      output.concat((char)value);
    }
  }

  if(output.length() == EEPROM_MAX_ITEM_LENGTH) return "invalid";
  
  output.replace("'","");
  return output;
  
}

void save_wifi_ssid_in_eeprom(String wifi_ssid) {
  write_string_to_eeprom(wifi_ssid, EEPROM_WIFI_SSID_ADDRESS);
}

void save_wifi_password_in_eeprom(String wifi_password) {
  write_string_to_eeprom(wifi_password, EEPROM_WIFI_PASSWORD_ADDRESS);
}

String get_wifi_ssid_from_eeprom() {
  return read_string_from_eeprom(EEPROM_WIFI_SSID_ADDRESS);
}

String get_wifi_password_from_eeprom() {
  return read_string_from_eeprom(EEPROM_WIFI_PASSWORD_ADDRESS);
}
