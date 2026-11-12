/********  HAB_OBDH **********/
/* IFSP_Boituva              */
/* Prof. Mario Pin           */
/* v1.2                      */
/* 11/11/2020                */
/*****************************/

//bibliotecas necessarias
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <EBYTE.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SdFat.h>

//cria os possiveis estados de operacao
enum Estados{INIC, OPER_D, OPER_N, SEGUR};

//define estado inicial
Estados estado = INIC;

//cria portas seriais para COMM e GPS
SoftwareSerial ESerial  (7, 6); //RX, TX
SoftwareSerial serialGPS(8, 9); //RX, TX

//define pinos de controle da COMM: M0, M1, Aux 
EBYTE E32(&ESerial, 4, 5, -1);

//define GPS
TinyGPS gps;

//define sensor de pressao
#define seaLevelPressure_hPa 1016.00 //pressao no nivel do mar
Adafruit_BMP085 bmp; 


//define SD Card
#define chipSelect 10  //SD Card CS
SdFat sdCard; 
SdFile meuArquivo;


//define sensores de temperatura
#define ONE_WIRE_BUS 3  //pino de dados
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

//define portas analogicas para medir tensao
#define AnalogVbat A0
#define AnalogVpv A1


// Struct para criar pacote de transmissao de dados
struct DATA {
  long Latitude;
  long Longitude;
  unsigned short Altitude;
  unsigned short Speed;
  unsigned short Heading;
  byte Hour;
  byte Minute;
  byte Second;
  byte Day;
  byte Month;
  int Year;
  unsigned long Age;
  unsigned long HDOP;
  unsigned short Sats;
  short Temp1;
  short Temp2;
  short Temp3;
  short Temp_BMP180;
  long Pressure;
  unsigned short Baro_Alt;
  unsigned short Vbat;
  unsigned short Vpv;
  unsigned long CNT;
  };

DATA MyData;

//variaveis globais
byte Hundredths;
uint8_t deviceCount = 0;

//funcao de espera inteligente
static void smartdelay(unsigned long ms);

//funcoes para aquisicao de dados dos diversos subsistemas
void TMP_aquisition();
void BMP_aquisition();
void GPS_aquisition();
void EPS_aquisition();

//funcao para gravar dados em arquivo
void FIL_print();

//funcao do programa principal
void operacao_voo();

void setup() {

  //inicializa portas seriais
  Serial.begin(9600); //**FOR DEBUG ONLY**
  ESerial.begin(9600);
  serialGPS.begin(9600);

  //inicializa COMM
  E32.init(); 

  //inicializa sensores de temperatura
  sensors.begin(); 
  deviceCount = sensors.getDeviceCount();  // locate temperature sensors devices on the bus

  //inicializa sensor de pressao atmosferica
  bmp.begin(); 
  
  // Inicializa SD Card e cria arquivo
  sdCard.begin(chipSelect,SPI_HALF_SPEED);  
  meuArquivo.open("teste3.txt" , O_RDWR | O_CREAT | O_AT_END);
  
  
 
  // CASO QUEIRA ALTERAR CONFIGURACOES DE COMM
  // E32.SetAirDataRate(ADR_8K);     // change the air data rate
  // E32.SetAddressH(0);             // set the high address byte
  // E32.SetAddressL(0);             // set the low address byte
  // E32.SetChannel(17);             // set the channel (0-32 is pretty typical)
  // E32.SetTransmitPower(3);        // set transmission power
  // E32.SaveParameters(TEMPORARY);  // save the parameters to the EBYTE EEPROM, you can save temp if periodic changes are needed
     
  // CASO QUEIRA IMPRIMIR NA SERIAL OS PARAMETROS DE COMM
  // Transceiver.PrintParameters();
  
  MyData.CNT = 0;
}

void loop(){ //programa principal
  
  switch (estado){
    
    //inicializacao do sistema
    case INIC:{  
      Serial.println("INIC");
      EPS_aquisition();
      smartdelay(3000);
      GPS_aquisition();      
      while(MyData.Sats < 2 && MyData.HDOP > 10 && MyData.Vbat > 9){
        Serial.println("INIC.W");
        smartdelay(5000);
        GPS_aquisition();
        EPS_aquisition();
        }
      if(MyData.Vbat <= 9)
        estado = SEGUR;
        else 
        estado = OPER_D;
      break;
    }
    
    //operacao normal durante o dia
    case OPER_D:{ 
      Serial.println("OPER_D");
      Serial.println(MyData.Age);
      while(/*MyData.Hour < 22 && MyData.Hour >= 11 && */MyData.Vbat > 9){
        Serial.println("OPER_D.W");
        Serial.println(MyData.Temp_BMP180);
        //aguarda tempo para proxima transmissao
        smartdelay(5000);
        TMP_aquisition();
        BMP_aquisition();
        GPS_aquisition();
        EPS_aquisition();
        FIL_print();
        //envia dados pelo COMM
        E32.SendStruct(&MyData, sizeof(MyData));
        MyData.CNT ++;
      }
      if(MyData.Vbat <= 9)
        estado = SEGUR;
        else 
        estado = OPER_N;
      break;
    }
    
    //operacao normal durante a noite
    case OPER_N: {
      Serial.println("OPER_N");
      Serial.println(MyData.Age);
      while((MyData.Hour >=22 || MyData.Hour < 11) && MyData.Vbat > 9){
        Serial.println("OPER_N.W");
        Serial.println(MyData.Age);
        smartdelay(1000);
        TMP_aquisition();
        BMP_aquisition();
        GPS_aquisition();
        EPS_aquisition();
        FIL_print();
        MyData.CNT ++;

        //aguarda tempo para proxima operacao
        delay(1000);
      }
      if(MyData.Vbat <= 9)
        estado = SEGUR;
        else 
        estado = OPER_D;
      break;
    }
    
    //operacao de seguranca (bateria baixa)
    case SEGUR:{
      Serial.println("SEGUR");
      Serial.println(MyData.Age);
      while(MyData.Vbat <= 9){
        GPS_aquisition();
        Serial.println("SEGUR.W");
        Serial.println(MyData.Age);
        EPS_aquisition();
        smartdelay(1000);
      }
      estado = INIC;
      break;
      }

       
    }
  }



//funcoes 

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (serialGPS.available())
      gps.encode(serialGPS.read());
  } while (millis() - start < ms);
}

void TMP_aquisition(){
  //comando para os sensores de temperatura fazerem uma conversao
  sensors.requestTemperatures(); 

  // carrega dados dos sensores de temperatura
  MyData.Temp1 = sensors.getTempCByIndex(1)*10;
  MyData.Temp2 = sensors.getTempCByIndex(2)*10;
  MyData.Temp3 = sensors.getTempCByIndex(3)*10;
}

void BMP_aquisition(){
  
  MyData.Pressure =  bmp.readPressure();                          //le valor de pressao atmosferica e converte para kPa
  MyData.Temp_BMP180 = bmp.readTemperature()*10;                  //le temperatura do barometro
  MyData.Baro_Alt = bmp.readAltitude(seaLevelPressure_hPa * 100); //le altitude barometrica em relacao ao valor de calibracao
}

void GPS_aquisition(){

  //carrega dados de GPS
  gps.crack_datetime(&MyData.Year, &MyData.Month, &MyData.Day, &MyData.Hour, &MyData.Minute, &MyData.Second, &Hundredths, &MyData.Age);
  gps.get_position(&MyData.Latitude, &MyData.Longitude, &MyData.Age);
  MyData.Altitude = gps.altitude()/100;
  if(gps.course() <= 36000)
    MyData.Heading = gps.course()/100;
  else 
    MyData.Heading = 0;  
  MyData.Speed = gps.f_speed_kmph();
  MyData.HDOP = gps.hdop();
  MyData.Sats = gps.satellites();
}

void EPS_aquisition(){

  MyData.Vbat = analogRead(AnalogVbat);
  MyData.Vpv = analogRead(AnalogVpv);
  
}

void FIL_print(){

  //imprime dados em arquivo no SD Card
  meuArquivo.print(MyData.CNT);
  meuArquivo.print(F(";"));
  
  meuArquivo.print(MyData.Day);
  meuArquivo.print(F("/"));
  meuArquivo.print(MyData.Month);
  meuArquivo.print(F("/"));
  meuArquivo.print(MyData.Year);
  meuArquivo.print(F(";"));
  
  meuArquivo.print(MyData.Hour);
  meuArquivo.print(F(":"));
  meuArquivo.print(MyData.Minute);
  meuArquivo.print(F(":"));
  meuArquivo.print(MyData.Second);
  meuArquivo.print(F(";"));

  meuArquivo.print(MyData.Age);
  meuArquivo.print(F(";"));
    
  meuArquivo.print(MyData.Latitude);
  meuArquivo.print(F(";"));
  
  meuArquivo.print(MyData.Longitude);
  meuArquivo.print(F(";"));    
  
  meuArquivo.print(MyData.Altitude);
  meuArquivo.print(F(";"));
  
  meuArquivo.print(MyData.Speed);
  meuArquivo.print(F(";"));
  
  meuArquivo.print(MyData.Heading);
  meuArquivo.print(F(";"));

  meuArquivo.print(MyData.HDOP);
  meuArquivo.print(F(";"));

  meuArquivo.print(MyData.Sats);
  meuArquivo.print(F(";"));

  meuArquivo.print(MyData.Temp1);
  meuArquivo.print(F(";"));
  
  meuArquivo.print(MyData.Temp2);
  meuArquivo.print(F(";"));
  
  meuArquivo.print(MyData.Temp3);
  meuArquivo.print(F(";"));

  meuArquivo.print(MyData.Temp_BMP180);
  meuArquivo.print(F(";"));
  
  meuArquivo.print(MyData.Pressure);
  meuArquivo.print(F(";"));
  
  meuArquivo.print(MyData.Baro_Alt);
  meuArquivo.print(F(";"));

  meuArquivo.print(MyData.Vbat);
  meuArquivo.print(F(";"));

  meuArquivo.print(MyData.Vpv);
  meuArquivo.println(F(";"));
    
  //compila arquivo
  meuArquivo.flush();
}
