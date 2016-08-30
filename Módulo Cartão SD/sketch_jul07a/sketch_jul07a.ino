// Carrega a biblioteca SD
#include <SD.h>

Sd2Card SDcard;
SdVolume volume;

// Pino do Arduino conectado ao pino CS do modulo
const int chipSelect = 4;  
// Pino do Arduino conectado ao push-button
int pinobotao = 3;
int contador = 0;

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
  int valor = digitalRead(pinobotao);
  // Verifica se o push-button foi acionado
  if (valor == 1)
  {
    // Delay caso o botão continue pressionado
    while(digitalRead(pinobotao) == 1)
    {
      delay(100);
    }
    // Incrementa o contador e envia os dados para o
    // Serial monitor
    contador=contador++;
    Serial.print("Botao pressionado. Contador : ");
    Serial.println(contador);
  
    // Abre o arquivo arquivo.txt do cartao SD
    File dataFile = SD.open("teste1.txt", FILE_WRITE);
    // Grava os dados no arquivo
    if (dataFile) 
    {
      dataFile.print("Contador : ");
      dataFile.println(contador);
      dataFile.close();
    }  
    else 
    {
      // Mensagem de erro caso ocorra algum problema
      // na abertura do arquivo
      Serial.println("Erro ao abrir teste1.txt !");
    } 
  }
}
