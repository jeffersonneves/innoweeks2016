#define FLOWSENSOR 2  // Flow sensor pin
#define RELAY      7  // Relay pin
#define LEDPIN    13  // Board led pin

volatile bool hasFlow; // flag to indicate flow
int doIt = 1; // controls the execution of the demo

void flow () {    // This is the function that the interupt calls
  hasFlow = true; // If it was called it means that we have flow
}

// The setup() method runs once, when the sketch starts
void setup() {
  hasFlow = false;

  pinMode(FLOWSENSOR, INPUT); //initializes digital pin 2 as an input
  
  pinMode(RELAY, OUTPUT); //initializes relay pin as output
  digitalWrite(RELAY, LOW);

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(FLOWSENSOR), flow, RISING); // the interrupt is attached

}

void loop() {

  if (doIt){
    justDoIt(); // start the process, since it's a single time execution demo, just one pass
    doIt = 0;
  }

  digitalWrite(LEDPIN, HIGH); delay (250);
  digitalWrite(LEDPIN, LOW);  delay (250);
  digitalWrite(LEDPIN, HIGH); delay (250);
  digitalWrite(LEDPIN, LOW);  delay (250);
  digitalWrite(LEDPIN, HIGH); delay (1000);
  digitalWrite(LEDPIN, LOW);  delay (1000);
}

void justDoIt () {
  bool pumpItUp = false;

  while (!hasFlow) {
    sei();        // Enables interrupts
    delay (1000); // Wait 1 second
    cli();        // Disable interrupts
    Serial.print("hasFlow is: ");
    Serial.println((hasFlow?"true":"false"));
  }

  // just checking...
  if (false) {
    //postAlarm();

    //pumpItUp = getHANAResponse();
    while (!pumpItUp) {
      delay (500);
      //pumpItUp = getHANAResponse();
    }

    if (pumpItUp){

      // turn on the pump
      Serial.println("Liga a bomba");
      digitalWrite(RELAY, HIGH);

      // wait for 20 seconds
      delay (20000);

      // turn off the pump
      Serial.println("Desliga a bomba");
      digitalWrite(RELAY, LOW);
    }
  }
}

void postAlarm() {
  Serial.println("Postando Alarme");
  delay (500);
}

bool getHANAResponse(){
  Serial.println("Resposta enviada");
  delay (5000);
  return true;
}
