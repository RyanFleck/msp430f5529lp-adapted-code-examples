/*
 * Button enables green LED. Polling version.
 * To get serial feedback use COM4 to as serial port.
 */

int run;
int toggle;
int printed;

void setup() {
  // put your setup code here, to run once:
  pinMode(GREEN_LED, OUTPUT);
  pinMode(P1_1, INPUT_PULLUP);

  run = 0;

  
  Serial.begin(9600);
  Serial.println("Command one");
}

void loop() {

  // Read the input pin.
  run = digitalRead(P1_1);

  // Check if message for change to current status has been printed.
  if( toggle != printed ){
    Serial.print("Value of run is ");
    Serial.println(run);
    printed = toggle;
  }

  // Do the toggling.
  if(run == 0){
    digitalWrite(GREEN_LED, HIGH);
    toggle = 1;
  }else{
    digitalWrite(GREEN_LED, LOW);
    toggle = 2;
  }

}
