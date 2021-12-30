void IO_setup(){

  Serial.println("[IO] IO setup");
  
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(LED_PIN, HIGH); // LED is active LOW
  digitalWrite(RELAY_PIN, LOW);
}

void turn_on(){
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("[IO] Turning on");
  mqtt_publish_state();
}

void turn_off(){
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("[IO] Turning off");
  mqtt_publish_state();
}

void toggle(){
  Serial.println("[IO] Toggling");
  if(digitalRead(RELAY_PIN)) turn_off();
  else turn_on();
}

String get_device_state(){
  if(digitalRead(RELAY_PIN)) return "on";
  else return "off";
}

void read_button()
{
  const int button_debounce_delay = 50;
  static int last_button_reading;
  static long last_button_reading_change_time;
  static int button_state = LOW; // Output
  
  int button_reading = digitalRead(BUTTON_PIN);

  // Check if reading changed
  if(button_reading != last_button_reading)
  {
    // Acknowledge button reading change
    last_button_reading = button_reading;
    
    // Record time of last button reading change
    last_button_reading_change_time = millis();
  }

  // Only acknowledge the button reading if state did not change for long enough
  if(millis()-last_button_reading_change_time > button_debounce_delay)
  {
    
    if(button_reading != button_state)
    {
      // Save reading as button state if state and reading don't match (prevents saving all the time)
      button_state = button_reading;


      // APPLICATION DEPENDENT FROM HERE
      if(button_state == LOW)
      {
        Serial.println("[IO] Button pressed");
        toggle();
      }
    }
  }
}
