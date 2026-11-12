/********  HAB_OBDH ***********/
/* IFSP_Boituva               */
/* Prof. Mario Pin            */
/* v1.10                      */
/* 14/01/2021                 */ 
/*VOO SAIRUS I                */
/******************************/ 
/* Transm 1x/ 1min  20dB  OK  */
/* Transm 1x/ 10min 30dB  OK  */
/* TX_Power Down          OK  */
/* Watchdog transm. OPER_D    */
/******************************/



//bibliotecas necessarias
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <EBYTE.h>
#include <DallasTemperature.h>
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
DallasTemperature sensortemp(&oneWire);

//define portas analogicas para medir tensao
#define AnalogVbat A0
#define AnalogVpv A1

//define minima tensao da bateria
#define Vbat_min 410

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
uint8_t deviceCount;
byte min_ant;
byte PWRDC;
unsigned long ultimo_millis;

//array de configuração do transceiver
byte Lo_P[] = {0xC2, 0x00, 0x00, 0x1A, 0x15, 0x47};
byte Hi_P[] = {0xC2, 0x00, 0x00, 0x1A, 0x15, 0x44};

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
  //Serial.begin(9600); //**SOMENTE PARA TESTES**
  ESerial.begin(9600);
  serialGPS.begin(9600);
  

  //inicializa COMM
  E32.init(); 
  digitalWrite(4, HIGH); digitalWrite(5, HIGH); delay(100);
  ESerial.write(Lo_P, sizeof(Lo_P)); delay(100);
  
  //inicializa sensores de temperatura
  sensortemp.begin(); 
  delay(1000);
  deviceCount = sensortemp.getDeviceCount();  // localiza os sensores de temperatura
  
  //inicializa sensor de pressao atmosferica
  bmp.begin(); 
  
  // Inicializa SD Card e cria arquivo
  sdCard.begin(chipSelect,SPI_HALF_SPEED);  
  meuArquivo.open("teste3.txt" , O_RDWR | O_CREAT | O_AT_END);
    
  MyData.CNT = 0;
  
  
}

void loop(){ //programa principal
  
  switch (estado){
    
    /*******inicializacao do sistema********/
    case INIC:{  
      smartdelay(3000);
      EPS_aquisition();
      GPS_aquisition();      
      while((MyData.Sats < 4|| MyData.Sats > 40) && MyData.Vbat > Vbat_min){  //Aguarda pelo menos 4 satélites GPS para operação normal ou se Vbat baixo entra em modo de segurança
        smartdelay(10000);
        GPS_aquisition();
        EPS_aquisition();
        TMP_aquisition();
        }
      if(MyData.Vbat <= Vbat_min)
        estado = SEGUR;
        else 
        estado = OPER_D;
      break;
    }
    
    /******* operacao normal durante o dia *******/
    case OPER_D:{ 
      min_ant = MyData.Minute;
      while(MyData.Hour < 22 && MyData.Hour >= 11 && MyData.Vbat > Vbat_min){  //se hora entre 22 e 11 UTC -> Operação noturna. Se Vbat baixo -> modo de segurança
        delay(4000);
        smartdelay(1000);
        BMP_aquisition();
        EPS_aquisition();
        GPS_aquisition();
        TMP_aquisition();
        FIL_print();
        if (MyData.Minute != min_ant || (millis() - ultimo_millis) > 90000) //envia dados pelo COMM a cada minuto do GPS ou a cada 1'30 se sem sinal de GPS
          {
          if (MyData.Minute % 10 == 0) //transmite 30dB
            {
            ESerial.write(Hi_P, sizeof(Hi_P)); delay(100);
            digitalWrite(4, LOW); digitalWrite(5, LOW); delay(100);
            E32.SendStruct(&MyData, sizeof(MyData));
            digitalWrite(4, HIGH); digitalWrite(5, HIGH); delay(100);
            ESerial.write(Lo_P, sizeof(Lo_P)); delay(100);
            min_ant = MyData.Minute;
            }
           else                          //transmite 20dB
            {            
            digitalWrite(4, LOW); digitalWrite(5, LOW); delay(100);
            E32.SendStruct(&MyData, sizeof(MyData));
            digitalWrite(4, HIGH); digitalWrite(5, HIGH); delay(100);
            min_ant = MyData.Minute;
            }
          ultimo_millis = millis(); 
          }
        MyData.CNT ++;
        }
      if(MyData.Vbat <= Vbat_min)
        estado = SEGUR;
        else 
        estado = OPER_N;
      break;
    }
    
    /*********operacao normal durante a noite**********/
    case OPER_N: {
      while((MyData.Hour >= 22 || MyData.Hour < 11) && MyData.Vbat > Vbat_min){
        delay(9000);
        smartdelay(1000);
        BMP_aquisition();
        EPS_aquisition();
        GPS_aquisition();
        TMP_aquisition();
        FIL_print();
        MyData.CNT ++;
        }
      if(MyData.Vbat <= Vbat_min)
        estado = SEGUR;
        else 
        estado = OPER_D;
      break;
    }
    
    /**********operacao de seguranca (bateria baixa)**********/
    case SEGUR:{
     while(MyData.Vbat <= Vbat_min){
        delay (10000);
        EPS_aquisition();
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
  sensortemp.requestTemperatures();
  
  // carrega dados dos sensores de temperatura
  MyData.Temp1 = sensortemp.getTempCByIndex(0)*10;
  MyData.Temp2 = sensortemp.getTempCByIndex(1)*10;
  MyData.Temp3 = sensortemp.getTempCByIndex(2)*10;
  

  
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
  Serial.println (MyData.Sats);
}

void EPS_aquisition(){

  MyData.Vbat = analogRead(AnalogVbat); 
  MyData.Vpv  = analogRead(AnalogVpv);
  
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
