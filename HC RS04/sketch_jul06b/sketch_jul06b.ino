//Programa: Conectando Sensor Ultrassonico HC-SR04 ao Arduino
//Autor: FILIPEFLOP
 
//Carrega a biblioteca do sensor ultrassonico
#include <Ultrasonic.h>
 
//Define os pinos para o trigger e echo
#define pino_trigger 4
#define pino_echo 5
 
//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);
 
void setup()
{
  Serial.begin(9600);
}
 
void loop()
{
  aqui:
  //Le as informacoes do sensor, em cm
  float cmMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  /*
  Essa parte que é importante.
  Pois, quando a distância do sensor até o solo for maior que a sua posição no avião
  (aqui eu chuto 30 cm)
  ele dará como resposta 1
  caso contrário 0
  */
  if(cmMsec<= 30){
    
    Serial.print("0");
    delay(100);
    goto aqui;
  }
  
  else{
    Serial.print("1");
    delay(100);
    goto aqui;
  }
}
