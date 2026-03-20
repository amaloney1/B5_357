/*This sketch will log the number of lengths up and down the leg
 and the time stamps for each, to determine analyze drift over time  
*/
  unsigned long previousMillis = 0;
  
  const unsigned long debounceDelay = 500;  // 50 ms debounce (to ensurue that multiple activations of swtich due to inherent bounce are not counted as multiple)
  unsigned long lastCalfChange = 0;
  unsigned long lastAnkleChange = 0;
  
  int calf = 2;
  int ankle = 3;

  int calf_count = 0;
  int ankle_count = 0;
  int cycle_count = 0;

  unsigned long calf_time[10];  // array for 60 cycles 
  unsigned long ankle_time[10];
  unsigned long cycle_time[10];

  int missed = 0;

  bool printed = false;

void setup() {
    Serial.begin(9600); //initialize serial communication
    pinMode(calf, INPUT_PULLUP);
    pinMode(ankle, INPUT_PULLUP);
}

void loop() {

unsigned long currentMillis = millis();

  int calf_state = digitalRead(calf);   //read and store the state of switch at calf
  int ankle_state = digitalRead(ankle);    //read and store the state of switch at ankle

  if(calf_state == LOW && (currentMillis - lastCalfChange > debounceDelay) && calf_count < 10){        //check if calf is reached    
    calf_time[calf_count] = currentMillis - previousMillis;  // log time from ankle to calf
    if(calf_time[calf_count] != 12){     // log the number translations up the leg that are out of time
      missed++;
    }
    calf_count++;               //log calf being reached
    previousMillis = currentMillis;  //reset clock
    lastCalfChange = currentMillis;  //reset debounce timer
    Serial.println("calf pressed"); // for testing
  }

  if(ankle_state == LOW && (currentMillis - lastAnkleChange > debounceDelay) && ankle_count < 10){        //check if ankle is reached    
    ankle_time[ankle_count] = currentMillis - previousMillis; // log time from calf to ankle
    if(ankle_time[ankle_count] != 48){    // log the number translations up the leg that are out of time
      missed++;
    }
    ankle_count++;              //log ankle being reached
    previousMillis = currentMillis;  //reset clock
    lastAnkleChange = currentMillis;  //reset debounce timer
    Serial.println("ankle pressed");  //for testing
  }

  if(calf_count > cycle_count && ankle_count > cycle_count && cycle_count <10){
    cycle_time[cycle_count] = ankle_time[cycle_count] + calf_time[cycle_count];   //store time for full cycle
    cycle_count++;  //store cycle number
  }

  if(cycle_count == 10 && !printed){
    Serial.print("Missed: ");      //quickest check to see if the timing was appropriate
    Serial.println(missed);
    printed = true;  // prevents re-printing
  }
}