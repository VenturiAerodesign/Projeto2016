// Carrega a biblioteca SD
#include <SD.h>
#include <Ultrasonic.h>

Sd2Card SDcard;
SdVolume volume;
#define pino_trigger 4
#define pino_echo 5  

//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);

// Pino do Arduino conectado ao pino CS do modulo
const int chipSelect = 4;  
// Pino do Arduino conectado ao push-button

void setup()
{
  // Define o pino do push-button como entrada
  pinMode(pinobotao, INPUT);
  Serial.begin(9600);
  //Inicia a comunicacao com o modulo SD
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("Falha ao acessar o cartao !");
    Serial.println("Verifique o cartao/conexoes e reinicie o Arduino...");
    return;
  }
  Serial.println("Cartao iniciado corretamente !");
  Serial.println("Aguardando acionamento do push-button...");
  Serial.println();
}

void loop() 
{
  
  //Le as informacoes do sensor, em cm e pol
  float cmMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  //Exibe informacoes no serial monitor
  Serial.print("Distancia em cm: ");
  Serial.print(cmMsec);
  
  // Abre o arquivo arquivo.txt do cartao SD
  File dataFile = SD.open("teste3.txt", FILE_WRITE);
  // Grava os dados no arquivo
  if (dataFile) 
  {
    dataFile.print("Distância : ");
    dataFile.println(cmMsec);
    dataFile.close();
  } 
}
