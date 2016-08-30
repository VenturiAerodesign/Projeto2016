//Programa : Teste módulo pressão BMP085
//Autor : Adafruit
//Traducoes e comentarios : Arduino e Cia


#include <Wire.h>
#include <Adafruit_BMP085.h>

// Conectar pino Vcc do BMP085 ao pino 3.3v do Arduino(NAO UTILIZAR 5.0V !)
// Conectar pino GND do módulo ao GND do Arduino
// Conectar o pino SCL do módulo ao pino analógico 5 do Arduino
// Conectar o pino SDA do módulo ao pino analógico 4 do Arduino
// Pino EOC (End of conversion) não utilizado
// Pino XCLR é um pino de reset, também não é utilizado

Adafruit_BMP085 bmp;

void setup() 
{
  Serial.begin(9600);
  if (!bmp.begin()) {
  Serial.println("Sensor BMP085 não encontrado, verifique as conexões !");
  while (1) {}
  }
}

void loop() 
{
    Serial.print("Temperatura = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressao = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    // Calcula a altitude utilizando a pressao barometrica padrao
    // de 1013.25 milibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" metros");

  // É possivel extrair uma medição mais precisa se você souber 
  // a pressão ao nível do mar, que varia com o tempo/clima. 
  // Se ela for de 1015 milibars ,é igual a 101500 Pascals.

    Serial.print("Altitude real = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" metros");
    Serial.println();

    delay(5000);
}
