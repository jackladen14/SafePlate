
int end = 0;
int volatile sensmux1 = 0;
int volatile sensmux2 = 0;
int volatile outmux = 0;
int volatile sensval = 0;
float volatile sensvolt = 0.0;
int sensmax = 16;
int outmax = 8;
int volatile values[16][8];
int valuemodifier = 5;

void setup() {
  Serial.begin(115200, SERIAL_8N1);     //matches Sparkfun Serial
  pinMode(2, OUTPUT);                   //configure these pins for outputs
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(2, LOW); //initialize all pins to 0
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);      
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  analogRead(A0);
  while(!Serial){
    
  }
}

void set_outputs(int val, int pin1, int pin2, int pin3){     //pin 3 is A, pin2 is B, pin1 is C
  switch(val){                                               //sets specified mux logic based on input 0-7
    case 0:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, LOW);
      break;
    case 1:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, HIGH);
      break;
    case 2:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
      digitalWrite(pin3, LOW);
      break;
    case 3:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
      digitalWrite(pin3, HIGH);
      break;
    case 4:
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, LOW);
      break;
    case 5:
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, HIGH);
      break;
    case 6:
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, HIGH);
      digitalWrite(pin3, LOW);
      break;
    case 7:
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, HIGH);
      digitalWrite(pin3, HIGH);
      break;
  }
}

void ClearADC(void){             //clears the ADC of residual if any
sensmux1 = 0;
sensmux2 = 0;
set_outputs(sensmux1, 7, 8, 9);
set_outputs(sensmux2, 2, 3, 4);
analogRead(A0);
analogRead(A0);
  
}


void loop() {
  // put your main code here, to run repeatedly:
    for(int i = 0; i < outmax; i++){       //first for loop sets the output muxes
      outmux = i;
      set_outputs((7-outmux), 11,12,13);
      
      for(int j = 0; j< sensmax; j++){     //second for loop sets the reciever muxes
        sensmux2 = j / 8;                  
        set_outputs(sensmux2, 2, 3, 4);        
        sensmux1 = j % 8;
        set_outputs(sensmux1, 7,8,9);
      
        delayMicroseconds(100);            //delay to reduce possible residual
        //delay(2000);
        sensval = analogRead(A0);

        sensvolt = valuemodifier*(sensval * (5.0/1023.0));  //converts adc input to voltage,
        // multiplying it so that more precise voltages are read
        values[j][i] = (int)sensvolt;  //stores voltage as integer for easier UART output
      }
    }
    Serial.println(30);    //30 can not be achieved so it is the start of data number
    for(int i =0; i<outmax; i++){
      for(int j = 0; j<sensmax; j++){
        if(Serial.availableForWrite() < 12){    //prevents TX buffer from filling and data loss
          Serial.flush();
        }
        Serial.print(' ');        //spaces provide a break between ints allowing Sparkfun to read
        Serial.print(values[j][i]); 

        //Serial.print('  ');
      }
      Serial.println();    
    }
  Serial.flush();
  delay(5000);   //5 second delay prevents Sparkfun from overflowing

  while(Serial.available()> 0){
    char x = Serial.read();    //clears rx buffer if anything is in it
  }
  /*Serial.println();  
  Serial.println();  
  Serial.println();  
  Serial.println();  
  */
  ClearADC();
}






















