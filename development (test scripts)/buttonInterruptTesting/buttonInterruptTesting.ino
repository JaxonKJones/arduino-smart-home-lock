volatile long button1Calls = 0;
volatile long button2Calls = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(3, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), button1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), button2ISR, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  int button1Calls_last = 0;
  int button2Calls_last = 0;
  if(button1Calls > button1Calls_last || button2Calls > button2Calls_last){
    delay(10000);
    attachInterrupt(digitalPinToInterrupt(2), button1ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(3), button2ISR, FALLING);
    button1Calls_last = button1Calls;
    button2Calls_last = button2Calls;
  }
}

// Interrupt Service Routines
void button1ISR(){
  detachInterrupt(digitalPinToInterrupt(2));
  button1Calls += 1;
  Serial.println("Button1: " + String(button1Calls));
}

void button2ISR(){
  detachInterrupt(digitalPinToInterrupt(3));
  button2Calls += 1;
  Serial.println("Button2: " + String(button2Calls));
}