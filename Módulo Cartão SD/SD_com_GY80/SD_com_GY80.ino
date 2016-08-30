 /*
 Teste com o GY 80 e o SD Card
 */
 
// Carrega as bibliotecas
 #include <Wire.h>
 #include <SD.h>

Sd2Card SDcard;
SdVolume volume;

// Pino do Arduino conectado ao pino CS do modulo
const int chipSelect = 4;

//Define o endereco I2C do BMP085 - 0x77 ou 119 em decimal
#define BMP085_ADDRESS 0x77  

// Coisas do L3G4200D
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

// Coisas do ADXL345
#define Register_ID 0
#define Register_2D 0x2D
#define Register_X0 0x32
#define Register_X1 0x33
#define Register_Y0 0x34
#define Register_Y1 0x35
#define Register_Z0 0x36
#define Register_Z1 0x37

//Endereco I2C do L3G4200D
int L3G4200D_Address = 105;

// Endereco I2C do sensor : 83 em decimal ou 0x53
int ADXAddress = 0x53;

const unsigned char OSS = 0;  // Oversampling Setting

// Calibration values
// Variaveis do BMP 085
int ac1;
int ac2;
int ac3;
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1;
int b2;
int mb;
int mc;
int md;

// VARIAVES IMPORTANTES DO BMP 085
float temperature;
float pressure;
float atm;
float altitude;

// Variaveis do L3G4200D
int x;
int y;
int z;

// Variaveis do ADXL345
int reading = 0;
int val=0;
int X0,X1,X_out;
int Y0,Y1,Y_out;
int Z1,Z0,Z_out;
double Xg,Yg,Zg;

long b5; 

void setup () {
   Serial.begin(9600);
   Wire.begin();
   // Inicializa o BMP085
   bmp085Calibration();
   // Configura o L3G4200 para 200, 500 ou 2000 graus/seg
   setupL3G4200D(2000); 
   delay(1000);
   // Configuração do ADXL345
   
   // enable to measute g data
   Wire.beginTransmission(ADXAddress);
   Wire.write(Register_2D);
   Wire.write(8);                //measuring enable
   Wire.endTransmission();     // stop transmitting
   //Inicia a comunicacao com o modulo SD
   if (!SD.begin(chipSelect)) 
   {
     Serial.println("Falha ao acessar o cartao !");
     Serial.println("Verifique o cartao/conexoes e reinicie o Arduino...");
     return;
   }
   Serial.println("Cartao iniciado corretamente !");
   Serial.println();
   File dataFile = SD.open("GY80.txt", FILE_WRITE);
  // Grava os dados no arquivo
  if (dataFile)
  {
    dataFile.println("Temperatura  Pressao  Atm Padrao  Altitude  X(acel.)  Y(acel.)  Z(acel.)  X(giro.)  Y(giro.)  Z(giro.)");
    dataFile.println("C  Pa  Atm  m  m  m  m  graus/s  graus/s  graus/s");
    dataFile.close();
  }  
}

void loop() {
    // Executa a função do BMP 085
  BMP085();
  // Executa a função do giroscópio L3G4200D
  Giroscopio();
  // Executa a função do acererômetro ADXL345
  Acelerometro();
  // Mostra os dados obtidos no Serial Monitor
  ShowTime();
  delay(100);
  
}

/*
///////////////////
//////////////////
///////BMP085////
////////////////
///////////////
*/
void BMP085() {
    // Chama a rotina que calcula a temperatura
  // Esta rotina DEVE ser executada primeiro
  float temperature = bmp085GetTemperature(bmp085ReadUT());
  // Chama a rotina que calcula a pressao
  float pressure = bmp085GetPressure(bmp085ReadUP());
  // Chama a rotina que calcula atmosfera
  float atm = pressure / 101325; 
  // Chama a rotina que calcula a altitude
  float altitude = calcAltitude(pressure);
}

// Armazena todos os valores de calibracao do BMP085 em 
// variaveis globais. Valores de calibracao sao exigidos para
// calcular temperatura e pressao
// Esta funcao deve ser chamada/acionada no inicio do programa
void bmp085Calibration()
{
  ac1 = bmp085ReadInt(0xAA);
  ac2 = bmp085ReadInt(0xAC);
  ac3 = bmp085ReadInt(0xAE);
  ac4 = bmp085ReadInt(0xB0);
  ac5 = bmp085ReadInt(0xB2);
  ac6 = bmp085ReadInt(0xB4);
  b1 = bmp085ReadInt(0xB6);
  b2 = bmp085ReadInt(0xB8);
  mb = bmp085ReadInt(0xBA);
  mc = bmp085ReadInt(0xBC);
  md = bmp085ReadInt(0xBE);
}

// Calcula a temperatura em graus C
float bmp085GetTemperature(unsigned int ut)
{
  long x1, x2;

  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  float temp = ((b5 + 8)>>4);
  temp = temp /10;

  return temp;
}

// Calcula a pressao. Os valores de calibracao devem ser conhecidos
// b5 tambem eh exigido, logo a funcao bmp085GetTemperature(...) deve
// ser executada primeiro.
// Funcao retorna pressao em unidades de Pa
long bmp085GetPressure(unsigned long up){
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;

  b6 = b5 - 4000;
  // Calcula B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

  // Calcula B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;

  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;

  long temp = p;
  return temp;
}

// Read 1 byte from the BMP085 at 'address'
char bmp085Read(unsigned char address)
{
  unsigned char data;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 1);
  while(!Wire.available())
    ;

  return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int bmp085ReadInt(unsigned char address)
{
  unsigned char msb, lsb;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2)
    ;
  msb = Wire.read();
  lsb = Wire.read();

  return (int) msb<<8 | lsb;
}

// Read the uncompensated temperature value
unsigned int bmp085ReadUT(){
  unsigned int ut;

  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();

  // Wait at least 4.5ms
  delay(5);

  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(0xF6);
  return ut;
}

// Read the uncompensated pressure value
unsigned long bmp085ReadUP(){

  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;

  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();

  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<OSS));

  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  msb = bmp085Read(0xF6);
  lsb = bmp085Read(0xF7);
  xlsb = bmp085Read(0xF8);

  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);

  return up;
}

void writeRegister(int deviceAdress, byte address, byte val) {
  Wire.beginTransmission(deviceAdress); // start transmission to device 
  Wire.write(address);       // send register address
  Wire.write(val);         // send value to write
  Wire.endTransmission();     // end transmission
}

int readRegister(int deviceAddress, byte address){

  int v;
  Wire.beginTransmission(deviceAddress);
  Wire.write(address); // register to read
  Wire.endTransmission();

  Wire.requestFrom(deviceAddress, 1); // read a byte

  while(!Wire.available()) {
    // waiting
  }

  v = Wire.read();
  return v;
}

float calcAltitude(float pressure)
{
  float A = pressure/101325;
  float B = 1/5.25588;
  float C = pow(A,B);
  C = 1 - C;
  C = C /0.0000225577;

  return C;
}

/*
///////////////////
//////////////////
////Giroscópio///
////////////////
///////////////
*/
void Giroscopio () {
    // Atualiza os valores de X, Y e Z
  getGyroValues();  
}

void getGyroValues()
{
  // Rotina para leitura dos valores de X, Y e Z
  byte xMSB = readRegister2(L3G4200D_Address, 0x29);
  byte xLSB = readRegister2(L3G4200D_Address, 0x28);
  x = ((xMSB << 8) | xLSB);

  byte yMSB = readRegister2(L3G4200D_Address, 0x2B);
  byte yLSB = readRegister2(L3G4200D_Address, 0x2A);
  y = ((yMSB << 8) | yLSB);

  byte zMSB = readRegister2(L3G4200D_Address, 0x2D);
  byte zLSB = readRegister2(L3G4200D_Address, 0x2C);
  z = ((zMSB << 8) | zLSB);
}

int setupL3G4200D(int scale)
{
  //From  Jim Lindblom of Sparkfun's code

  // Enable x, y, z and turn off power down:
  writeRegister2(L3G4200D_Address, CTRL_REG1, 0b00001111);

  // If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
  writeRegister2(L3G4200D_Address, CTRL_REG2, 0b00000000);

  // Configure CTRL_REG3 to generate data ready interrupt on INT2
  // No interrupts used on INT1, if you'd like to configure INT1
  // or INT2 otherwise, consult the datasheet:
  writeRegister2(L3G4200D_Address, CTRL_REG3, 0b00001000);

  // CTRL_REG4 controls the full-scale range, among other things:
  if(scale == 250){
    writeRegister2(L3G4200D_Address, CTRL_REG4, 0b00000000);
  }else if(scale == 500){
    writeRegister2(L3G4200D_Address, CTRL_REG4, 0b00010000);
  }else{
    writeRegister2(L3G4200D_Address, CTRL_REG4, 0b00110000);
  }

  // CTRL_REG5 controls high-pass filtering of outputs, use it
  // if you'd like:
  writeRegister2(L3G4200D_Address, CTRL_REG5, 0b00000000);
}

void writeRegister2(int deviceAddress, byte address, byte val) 
{
    Wire.beginTransmission(deviceAddress); // start transmission to device 
    Wire.write(address);       // send register address
    Wire.write(val);         // send value to write
    Wire.endTransmission();     // end transmission
}

int readRegister2(int deviceAddress, byte address)
{
    int v;
    Wire.beginTransmission(deviceAddress);
    Wire.write(address); // register to read
    Wire.endTransmission();

    Wire.requestFrom(deviceAddress, 1); // read a byte

    while(!Wire.available()) 
    {
        // waiting
    }
    v = Wire.read();
    return v;
}


/*
///////////////////
//////////////////
//Acelerômetro///
////////////////
///////////////
*/
void Acelerometro() {
  //--------------X
  Wire.beginTransmission(ADXAddress); // transmit to device
  Wire.write(Register_X0);
  Wire.write(Register_X1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2); 
  if(Wire.available()<=2)   
  {
    X0 = Wire.read();
    X1 = Wire.read(); 
    X1=X1<<8;
    X_out=X0+X1;   
  }

  //------------------Y
  Wire.beginTransmission(ADXAddress); // transmit to device
  Wire.write(Register_Y0);
  Wire.write(Register_Y1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2); 
  if(Wire.available()<=2)   
  {
    Y0 = Wire.read();
    Y1 = Wire.read(); 
    Y1=Y1<<8;
    Y_out=Y0+Y1;
  }
  //------------------Z
  Wire.beginTransmission(ADXAddress); // transmit to device
  Wire.write(Register_Z0);
  Wire.write(Register_Z1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2); 
  if(Wire.available()<=2)   
  {
    Z0 = Wire.read();
    Z1 = Wire.read(); 
    Z1=Z1<<8;
    Z_out=Z0+Z1;
  }
  //
  Xg=X_out/256.0;
  Yg=Y_out/256.0;
  Zg=Z_out/256.0;
}


/*
///////////////////
//////////////////
/É hora do show//
///PORRAAAA/////
///////////////
*/
void ShowTime() {
  
  File dataFile = SD.open("teste2.txt", FILE_WRITE);
  
  if(dataFile) {
    dataFile.print(temperature, 2); 
    dataFile.print("  ");

    dataFile.print(pressure, 0); 
    dataFile.print("  ");

    dataFile.print(atm, 4);
    dataFile.print("  ");

    dataFile.print(altitude, 2); 
    dataFile.print("  ");
  
    dataFile.print(x);
    dataFile.print("  ");

    dataFile.print(y);
    dataFile.print("  ");

    dataFile.print(z);
    dataFile.print("  ");
  
    dataFile.print(Xg);
    dataFile.print("  ");
  
    dataFile.print(Yg);
    dataFile.print("  ");

    dataFile.println(Zg);
  
    dataFile.close();
  }
}


