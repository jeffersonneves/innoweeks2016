#include <Process.h>

#define FLOWSENSOR 2  // Flow sensor pin
#define RELAY      8  // Relay pin
#define LEDPIN    13  // Board led pin

const String postAlertURL = "https://innoweeks2016bebfc726c.us1.hana.ondemand.com/utilities/aegea/xs/alertInsert.xsjs";
const String getStatusAlertURL = "https://innoweeks2016bebfc726c.us1.hana.ondemand.com/utilities/aegea/xs/getAlertStatus.xsjs/?id=";


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

  attachInterrupt(digitalPinToInterrupt(FLOWSENSOR), flow, RISING); // the interrupt is attached
  interrupts(); // enable interrupts

  Serial.begin(9600);

  Bridge.begin();
}

int sendToHANA() {
  Process p;
  String cmd;
  String ret;

  cmd =  "curl ";
  cmd += "-k ";
  cmd += "-H \"Content-Type: application/json\" ";
  cmd += "-X POST ";
  cmd += "-d '{\"title\":\"[DEMO] Low pressure alert!\",\"description\":\"Low pressure on PCP18.\",\"author\":\"Prototipo\",\"priority\":\"10\"}' ";
  cmd += postAlertURL;

  Serial.println(cmd);

  p.runShellCommand(cmd);

  while (p.available() > 0) {
    char c = p.read();
    ret += c;
  }

  Serial.println(ret);

  return ret.substring(ret.indexOf(":") + 1, ret.indexOf("}")).toInt();
}

bool getFromHANA(int alertId) {
  Process p;
  String cmd;
  String ret;

  cmd =  "curl ";
  cmd += "-k ";
  cmd += "-X GET ";
  cmd += getStatusAlertURL;
  cmd += alertId;

  Serial.println(cmd);

  p.runShellCommand(cmd);

  while (p.available() > 0) {
    char c = p.read();
    ret += c;
  }

  Serial.println(ret);

//  return ret.substring(ret.indexOf(":")+1, ret.indexOf("}")).toInt();
  return true;
}

void justDoIt () {
  bool pumpItUp = false;
  int hanaResponse;

  while (!hasFlow) {
    delay(1000); // Wait 1 second
  }

  // just checking...
  if (hasFlow) {
    hanaResponse = sendToHANA();

    pumpItUp = getFromHANA(hanaResponse);
    while (!pumpItUp) {
      delay(1000);
      pumpItUp = getFromHANA(hanaResponse);
    }

    if (pumpItUp) {
      // turn on the pump
      digitalWrite(RELAY, HIGH);

      // wait for 20 seconds
      delay(8000);

      // turn off the pump
      digitalWrite(RELAY, LOW);
    }
  }
}

void loop() {

  if (doIt) {
    justDoIt(); // start the process, since it's a single time execution demo, just one pass
    doIt = 0;
  }
  blinkLed();
}

void blinkLed() {
  digitalWrite(LEDPIN, HIGH); delay (250);
  digitalWrite(LEDPIN, LOW);  delay (250);
  digitalWrite(LEDPIN, HIGH); delay (250);
  digitalWrite(LEDPIN, LOW);  delay (250);
  digitalWrite(LEDPIN, HIGH); delay (1000);
  digitalWrite(LEDPIN, LOW);  delay (1000);
}
