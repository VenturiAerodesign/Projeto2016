// Carrega a biblioteca SD
#include <SD.h>
#include <Ultrasonic.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

#define pino_trigger 5
#define pino_echo 6
#define sensor 2 //Define sensor como 2
#define sensorPin A1


long rpmconta=0; //Cria uma variável para armazenar as contagem
Adafruit_BMP085 bmp; //Habilita o GY-80

int sensorValue = 0; 
float Vout=0;
float P=0;
float pressao2=0;
float vcas=0;

//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);

// Pino do Arduino conectado ao pino CS do modulo
const int chipSelect = 4;  

boolean val = 0;



void setup()
{
  Serial.begin(9600);
  //Informa se o CI BMP085 está ativo no GY-80
  if (!bmp.begin()) {
  Serial.println("Sensor BMP085 não encontrado, verifique as conexões !");
  while (1) {}
  }
  //Inicia a comunicacao com o modulo SD
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("Falha ao acessar o cartao !");
    Serial.println("Verifique o cartao/conexoes e reinicie o Arduino...");
    
    return;
  }
  Serial.println("Cartao iniciado corretamente !");
  Serial.println();

  
  if (!SD.exists("teste1.txt")) {
    
    File dataFile = SD.open("teste1.txt", FILE_WRITE);
    if (dataFile) 
    {
      /* Seguindo o regulamento:
          1 linha - nome dos parâmetros
          2 linha - suas unidades
          3 linha em diante - valores
        Tudo separado por uma tabulação (tab)
      */
      dataFile.println("WOW  RPM  HP  PTstatic  VCAS  PTtotal");
      dataFile.println("bit  rpm  ft  Pa  m/s  Pa");    
      dataFile.close();
    }
  }
  }

void loop() 
{  
  /////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////
  int i=0;
  int sum=0;
  int offset=0;
  for(i=0;i<10;i++)
  {
       sensorValue = analogRead(sensorPin)-512;
       sum+=sensorValue;
  }
  offset=sum/10.0;
  
   sensorValue = analogRead(sensorPin)-offset; 
   Vout=(5*sensorValue)/1024.0;
   P=Vout-2.5;           
   float a = P*1000;
   
   if(a < 4.89 && a > -4.89) {
     vcas = 0;
     pressao2 = P*1000;
   }
   else{
     pressao2 = P*1000;
     vcas = sqrt ((2.0 * (P*1000))/1.2);
   }
  
  /////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////
  
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
  
  
  rpmconta = 0; //Zera a variável que armazena a contagem de rotações

  attachInterrupt(0, pegarpm, FALLING); //Habilita o pino 2(INT 0) para atender a...
                                      //interrupção (pegarpm) por borda de descida (FALLING).

  delay(1000); //Aguarda 1 segundo

  detachInterrupt(0); //Desabilita a interrupção

  rpmconta = rpmconta * 60; //Multiplica a contagem por 60 para pegar o RPM
  
  /////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////
  
  float pressao = bmp.readPressure(); //Le a pressao barometrica
  
  float hpm = bmp.readAltitude(); // Calcula a altitude em metros utilizando a pressao barometrica padrao
  
  float hp = hpm*3.28084;
  
  
  /////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////
  
  // Abre o arquivo arquivo.txt do cartao SD
  File dataFile = SD.open("teste1.txt", FILE_WRITE);
  // Grava os dados no arquivo
  if (dataFile) 
  {
    
    dataFile.print(val);
    dataFile.print("  ");
    dataFile.print(rpmconta);
    dataFile.print("  ");
    dataFile.print(hp);
    dataFile.print("  ");
    dataFile.print(pressao);
    dataFile.print("  ");
    dataFile.print(vcas);
    dataFile.print("  ");
    dataFile.println(pressao2);
    dataFile.close();
  }
 delay(100); 
 
    Serial.print(val);
    Serial.print("  ");
    Serial.print(rpmconta);
    Serial.print("  ");
    Serial.print(hp);
    Serial.print("  ");
    Serial.print(pressao);
    Serial.print("  ");
    Serial.print(vcas);
    Serial.print("  ");
    Serial.println(pressao2);
}

void pegarpm()
{      
rpmconta++; //Increvementa o a contagem de rotações
}
