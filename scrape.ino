//arduino sketch to set all pins to high

void setup() {
  // put your setup code here, to run once:
  //set all pins to high
    for (int i = 0; i < 13; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, HIGH);
    }

}

void loop() {
  // put your main code here, to run repeatedly:

}
