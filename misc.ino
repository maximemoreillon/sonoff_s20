String get_device_name(){
  String chip_id = String(ESP.getChipId(), HEX);
  return String(DEVICE_TYPE) + "-" + chip_id;
}

String get_device_nickname(){
  return read_string_from_eeprom(EEPROM_DEVICE_NICKNAME_ADDRESS);
}
