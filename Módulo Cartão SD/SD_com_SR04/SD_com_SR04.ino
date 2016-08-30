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

// Parte do potenciômetro
int potPin = 0;
int valPot = 0;
int val = 0;

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




  File dataFile = SD.open("teste4.txt", FILE_WRITE);
  // Grava os dados no arquivo
  if (dataFile)
  {
    /* Seguindo o regulamento:
        1 linha - nome dos parâmetros
        2 linha - suas unidades
        3 linha em diante - valores
       Tudo separado por uma tabulação (tab)
    */
    dataFile.println("WOW  Potenciômetro");
    dataFlile.println("bit  porcentagem");
    dataFile.close();
  }
}

void loop()
{

  valPot = analogRead(potPin);
  // Faz a conversão do valor do potenciômetro para um parâmetro entre 0 e 100%
  valPot = map(valPot,0,1023,0,100);

  //Le as informacoes do sensor, em cm e pol
  float cmMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);

  if(cmMsec<= 30){

    val = 0;
    goto aqui;
  }

  else{
    val = 1;
    goto aqui;
  }
  aqui:
  // Abre o arquivo arquivo.txt do cartao SD
  File dataFile = SD.open("teste4.txt", FILE_WRITE);
  // Grava os dados no arquivo
  if (dataFile)
  {
    dataFile.print(val);
    dataFile.print("  ");
    dataFile.println(valPot);
    dataFile.close();
  }
 delay(100);
}
