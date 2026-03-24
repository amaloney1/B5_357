/*This sketch will log the number of lengths up and down the leg
 and the time stamps for each, to determine analyze drift over time  
*/
  unsigned long previousMillis = 0;
  
  const unsigned long debounceDelay = 500;  // 500 ms debounce (to ensurue that multiple activations of swtich due to inherent bounce are not counted as multiple)
  unsigned long lastCalfChange = 0;
  unsigned long lastAnkleChange = 0;

  int prevCalfState = HIGH;
  int prevAnkleState = HIGH;
  
  int calf = 2;
  int ankle = 3;

  int calf_count = 0;
  int ankle_count = 0;
  int cycle_count = 0;

  unsigned long calf_time[20];  // array for 60 cycles 
  unsigned long ankle_time[20];
  unsigned long cycle_time[20];

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

  if(calf_state == LOW && (currentMillis - lastCalfChange > debounceDelay)){        //check if calf is reached    
    calf_time[calf_count] = currentMillis - previousMillis;  // log time from ankle to calf
    if(calf_time[calf_count] < (9*1000) || calf_time[calf_count] > (15*1000)){     // log the number translations up the leg that are outside of the +/- 3 sec time threshold
      missed++;
    }

    Serial.println("calf pressed"); // for testing
    Serial.print("time to calf: ");
    Serial.println(calf_time[calf_count]/1000.0);

    if(prevCalfState == HIGH || prevAnkleState == LOW){ //check correct alternating sequence to denote a cycle
    prevCalfState = LOW;
    }
    calf_count++;               //log calf being reached
    previousMillis = currentMillis;  //reset clock
    lastCalfChange = currentMillis;  //reset debounce timer
  }

  if(ankle_state == LOW && (currentMillis - lastAnkleChange > debounceDelay)){        //check if ankle is reached    
    ankle_time[ankle_count] = currentMillis - previousMillis; // log time from calf to ankle
    if(ankle_time[ankle_count] < (45*1000) || ankle_time[ankle_count] > (51*1000)){    // log the number translations up the leg that are out of time
      missed++;
    }

    Serial.println("ankle pressed"); // for testing
    Serial.print("time to ankle: ");
    Serial.println(ankle_time[ankle_count]/1000.0);

    if(prevAnkleState == HIGH || prevCalfState == LOW){   //check correct alternating sequence to denote a cycle
    prevAnkleState = LOW;
    }
    ankle_count++;              //log ankle being reached
    previousMillis = currentMillis;  //reset clock
    lastAnkleChange = currentMillis;  //reset debounce timer
  }

  if(prevAnkleState == LOW && prevCalfState == LOW){
    cycle_count++;
    cycle_time[cycle_count] = ankle_time[ankle_count-1] + calf_time[calf_count-1];   //store time for full cycle
    
    Serial.print("cycle num: ");
    Serial.println(cycle_count);
    Serial.print("ankle num:");
    Serial.println(ankle_count);
    Serial.print("calf num:");
    Serial.println(calf_count);
    Serial.print("cycle time: ");
    Serial.println(cycle_time[cycle_count]/1000.0);
    
    prevCalfState = HIGH;
    prevAnkleState = HIGH;
  }

  if(cycle_count == 10 && !printed){
    Serial.print("Missed: ");      //quickest check to see if the timing was appropriate
    Serial.println(missed);
    printed = true;  // prevents re-printing
  }
}