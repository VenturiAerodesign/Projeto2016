// Carrega a biblioteca SD
#include <SD.h>

Sd2Card SDcard;
SdVolume volume;

// Pino do Arduino conectado ao pino CS do modulo
const int chipSelect = 4;

// Parte do potenciômetro
int potPin = 0;
int valPot = 0;

void setup()
{
  // Define o pino do push-button como entrada
  pinMode(potPin, INPUT);
  
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
  valPot = analogRead(potPin);
  // Faz a conversão do valor do potenciômetro para um parâmetro entre 0 e 100%
  valPot = map(valPot,0,1023,0,100);
             
  
  // Abre o arquivo arquivo.txt do cartao SD
  File dataFile = SD.open("teste2.txt", FILE_WRITE);
  // Grava os dados no arquivo
  if (dataFile)
  {
    dataFile.print("Potenciômetro : ");
    dataFile.println(valPot);
    dataFile.close();
  }  
  delay(100);
}
