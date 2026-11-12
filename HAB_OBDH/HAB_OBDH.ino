/********  HAB_OBDH ***********/
/* IFSP_Boituva               */
/* Prof. Mario Pin            */
/* v2.0.1                     */
/* 09/06/2022                 */
/******************************/ 

/***** bibliotecas necessarias *****/
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <EBYTE.h>
#include <DallasTemperature.h>
#include <Adafruit_BMP085.h>
#include <SdFat.h> //2.1.0

#define LORA_M0 4
#define LORA_M1 5
#define COMM_TX 6
#define COMM_RX 7
#define LORA_AUX -1

#define GNSS_RX 8
#define GNSS_TX 9

#define CS_SD 10

#define ONE_WIRE_BUS 3

#define AnalogVbat A0
#define AnalogVpv A1

#define Vbat_min 410 //tensao minima da bateria
#define seaLevelPressure_hPa 1016.00 //pressao no nivel do mar

/***** cria os possiveis estados de operacao *****/
enum Estados{INICIALIZACAO, OPER_1, NOTURNO, SEGURANCA};

/***** define estado inicial *****/
Estados estado = INICIALIZACAO;

/***** cria portas seriais para COMM e GNSS *****/
SoftwareSerial UART_COMM (COMM_RX, COMM_TX); //RX, TX
SoftwareSerial UART_GNSS (GNSS_RX, GNSS_TX); //RX, TX

/***** define pinos de controle da COMM: M0, M1, Aux *****/ 
EBYTE E32(&UART_COMM, LORA_M0, LORA_M1, LORA_AUX);

/***** define GPS *****/
TinyGPS gps;

/***** define sensor de pressao *****/
Adafruit_BMP085 baro; 

/***** define SD Card *****/
SdFat SDcard; 
SdFile LogFILE;

/***** define sensores de temperatura *****/
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensortemp(&oneWire);



/***** struct para criar pacote de transmissao de dados *****/
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

/***** variaveis globais *****/
byte Hundredths;
uint8_t deviceCount;
byte min_ant;
byte PWRDC;
unsigned long ultimo_millis;

/***** array de configuração do transceiver *****/
byte Lo_P[] = {0xC2, 0x00, 0x00, 0x1A, 0x15, 0x47};
byte Hi_P[] = {0xC2, 0x00, 0x00, 0x1A, 0x15, 0x44};

/***** funcao de espera inteligente *****/
static void smartdelay(unsigned long ms);

/***** funcoes para aquisicao de dados dos diversos subsistemas *****/
void TMP_aquisition();
void BMP_aquisition();
void GPS_aquisition();
void EPS_aquisition();

/***** funcao para gravar dados em arquivo *****/
void FIL_print();

/***** inicializacao dos subsistemas *****/
void setup() {

  /***** inicializa portas seriais *****/
  //Serial.begin(9600); //#### SOMENTE PARA TESTES #####
  UART_COMM.begin(9600);
  UART_GNSS.begin(9600);
  
  /***** inicializa COMM *****/
  E32.init(); 
  digitalWrite(4, HIGH); digitalWrite(5, HIGH); delay(100);
  UART_COMM.write(Lo_P, sizeof(Lo_P)); delay(100);
  
  /***** inicializa sensores de temperatura *****/
  sensortemp.begin(); 
  delay(1000);
  deviceCount = sensortemp.getDeviceCount();  // localiza os sensores de temperatura
  
  /***** inicializa sensor de pressao atmosferica *****/
  baro.begin(); 
  
  /***** inicializa SD Card e cria arquivo *****/
  SDcard.begin(CS_SD, SPI_HALF_SPEED);  
  LogFILE.open("teste3.txt" , O_RDWR | O_CREAT | O_AT_END);
    
  MyData.CNT = 0;
}

/************** PROGRAMA PRINCIPAL **************/
void loop(){ 
  
  switch (estado){
    
    /********** inicializacao do sistema ********/
    case INICIALIZACAO:{  
      smartdelay(3000);
      EPS_aquisition();
      GPS_aquisition();      
      //Aguarda pelo menos 5 satélites GPS para operação normal ou se Vbat baixo entra em modo de segurança
      while((MyData.Sats < 5|| MyData.Sats > 40) && MyData.Vbat > Vbat_min){  
        smartdelay(10000);
        GPS_aquisition();
        EPS_aquisition();
        TMP_aquisition();
        }
      if(MyData.Vbat <= Vbat_min)
        estado = SEGURANCA;
        else 
        estado = OPER_1;
      break;
    }
    
    /********** OPERACAO DIURNA NORMAL ****************/
    case OPER_1:{ 
      min_ant = MyData.Minute;
      //se hora entre 22 e 11 UTC -> Operação noturna. Se Vbat baixo -> modo de segurança
      while(MyData.Hour < 22 && MyData.Hour >= 11 && MyData.Vbat > Vbat_min){  
        delay(4000);
        smartdelay(1000);
        BMP_aquisition();
        EPS_aquisition();
        GPS_aquisition();
        TMP_aquisition();
        FIL_print();
        //envia dados pelo COMM a cada minuto do GPS ou a cada 1'30 se sem sinal de GPS
        if (MyData.Minute != min_ant || (millis() - ultimo_millis) > 90000) 
          {
          if (MyData.Minute % 10 == 0) //transmite 30dB
            {
            UART_COMM.write(Hi_P, sizeof(Hi_P)); delay(100);
            digitalWrite(4, LOW); digitalWrite(5, LOW); delay(100);
            E32.SendStruct(&MyData, sizeof(MyData));
            digitalWrite(4, HIGH); digitalWrite(5, HIGH); delay(100);
            UART_COMM.write(Lo_P, sizeof(Lo_P)); delay(100);
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
        estado = SEGURANCA;
        else 
        estado = NOTURNO;
      break;
    }
    
    /********** OPERACAO NOTURNA ****************/
    case NOTURNO: {
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
        estado = SEGURANCA;
        else 
        estado = OPER_1;
      break;
    }
    
    /********** MODO DE SEGURANCA ***********/
    case SEGURANCA:{
      while(MyData.Vbat <= Vbat_min){
        delay (10000);
        EPS_aquisition();
        }
      estado = INICIALIZACAO;
      break;
    }
  }
}

/********* Funcoes auxiliares *******************/ 

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do{
    while (UART_GNSS.available())
      gps.encode(UART_GNSS.read());
  } while (millis() - start < ms);
}

void TMP_aquisition(){
  //comando para os sensores de temperatura fazerem uma conversao
  sensortemp.requestTemperatures();
  
  /***** carrega dados dos sensores de temperatura *****/
  MyData.Temp1 = sensortemp.getTempCByIndex(0)*10;
  MyData.Temp2 = sensortemp.getTempCByIndex(1)*10;
  MyData.Temp3 = sensortemp.getTempCByIndex(2)*10;
}

void BMP_aquisition(){
  
  MyData.Pressure =  baro.readPressure();                          //le valor de pressao atmosferica e converte para kPa
  MyData.Temp_BMP180 = baro.readTemperature()*10;                  //le temperatura do barometro
  MyData.Baro_Alt = baro.readAltitude(seaLevelPressure_hPa * 100); //le altitude barometrica em relacao ao valor de calibracao
}

void GPS_aquisition(){

  /***** carrega dados de GPS *****/
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
  MyData.Vpv  = analogRead(AnalogVpv);
  
}

void FIL_print(){

  /***** imprime dados em arquivo no SD Card *****/
  LogFILE.print(MyData.CNT);
  LogFILE.print(F(";"));
  
  LogFILE.print(MyData.Day);
  LogFILE.print(F("/"));
  LogFILE.print(MyData.Month);
  LogFILE.print(F("/"));
  LogFILE.print(MyData.Year);
  LogFILE.print(F(";"));
  
  LogFILE.print(MyData.Hour);
  LogFILE.print(F(":"));
  LogFILE.print(MyData.Minute);
  LogFILE.print(F(":"));
  LogFILE.print(MyData.Second);
  LogFILE.print(F(";"));

  LogFILE.print(MyData.Age);
  LogFILE.print(F(";"));
    
  LogFILE.print(MyData.Latitude);
  LogFILE.print(F(";"));
  
  LogFILE.print(MyData.Longitude);
  LogFILE.print(F(";"));    
  
  LogFILE.print(MyData.Altitude);
  LogFILE.print(F(";"));
  
  LogFILE.print(MyData.Speed);
  LogFILE.print(F(";"));
  
  LogFILE.print(MyData.Heading);
  LogFILE.print(F(";"));

  LogFILE.print(MyData.HDOP);
  LogFILE.print(F(";"));

  LogFILE.print(MyData.Sats);
  LogFILE.print(F(";"));

  LogFILE.print(MyData.Temp1);
  LogFILE.print(F(";"));
  
  LogFILE.print(MyData.Temp2);
  LogFILE.print(F(";"));
  
  LogFILE.print(MyData.Temp3);
  LogFILE.print(F(";"));

  LogFILE.print(MyData.Temp_BMP180);
  LogFILE.print(F(";"));
  
  LogFILE.print(MyData.Pressure);
  LogFILE.print(F(";"));
  
  LogFILE.print(MyData.Baro_Alt);
  LogFILE.print(F(";"));

  LogFILE.print(MyData.Vbat);
  LogFILE.print(F(";"));

  LogFILE.print(MyData.Vpv);
  LogFILE.println(F(";"));
    
  /***** grava no cartao SD *****/
  LogFILE.sync();
}
