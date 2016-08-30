int pino_sensor = 9;
 
int estado_sensor = 0;

volatile byte rpmcount;
unsigned int rpm;
unsigned long timeold;

void rpm_fun() {
  //Each rotation, this interrupt function is run twice, so take that into consideration for 
  //calculating RPM
  //Update count
  rpmcount++;
}

void setup() {
  Serial.begin(9600);
  // Define o pino do sensor como entrada
  pinMode(pino_sensor, INPUT);
  attachInterrupt(0, rpm_fun, FALLING);
  rpmcount = 0;
  rpm = 0;
  timeold = 0;
  Serial.println("Teste sensor infravermelho Arduino");
}
 
void loop() {

  delay(100);
  
  detachInterrupt(0);

  rpm = 30*1000/(millis() - timeold)*rpmcount;
  timeold = millis();
  rpmcount = 0;

  Serial.println(rpm,DEC);

  attachInterrupt(0, rpm_fun, FALLING);
}
