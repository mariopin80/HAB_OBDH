/********  HAB_OBDH *************/
/* IFSP_Boituva                 */
/* Prof. Mario Pin              */
/* v3.0 Implantacao de watchdog */
/* 09/04/2026                   */
/********** TO DO: **************/
/* Implantar estados:           */
/*  DESCIDA e POUSO             */
/* Evitar travamento de         */
/*  INICIALIZACAO sem GPS       */
/* Beacon em SEGURACA           */
/********************************/

/***** bibliotecas necessarias *****/
#include <avr/wdt.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <EBYTE.h>
#include <DallasTemperature.h>
#include <Adafruit_BMP085.h>
#include <SdFat.h>
#include <LowPower.h>

/***** define flags de watchdog *****/
#define WD_SET 0xAA
#define WD_RST 0x55

/***** define as portas da COMM *****/
#define LORA_M0 4
#define LORA_M1 5
#define COMM_TX 6
#define COMM_RX 7
#define LORA_AUX -1

/***** define as portas do GNSS *****/
#define GNSS_RX 8
#define GNSS_TX 9

/***** define pino de controle do cartao SD *****/
#define CS_SD 10

/***** define o pino de dados dos sensores de temperatura *****/
#define ONE_WIRE_BUS 3

/***** define os canais A/D para leitura de tensoes *****/
#define AnalogVbat A1
#define AnalogVpv A0

/***** define constantes do sistema *****/
#define Vbat_min 390                  //tensao minima da bateria 5,3V
#define seaLevelPressure_hPa 1013.00  //pressao no nivel do mar @@@Ajustar na data de lançamento@@@

/***** cria os possiveis estados de operacao *****/
enum Estados {INICIALIZACAO, OPER_1, NOITE, SEGURANCA};

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
  float Speed;
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
  unsigned short CNT;
};

DATA MyData;

/***** variaveis de watchdog *****/
volatile byte WDT_TMP;
volatile byte WDT_BMP;
volatile byte WDT_GPS;
volatile byte WDT_EPS;
volatile byte WDT_FIL;

/***** variaveis globais *****/
byte Hundredths;
uint8_t deviceCount;
byte min_ant;
unsigned long ultimo_millis = millis();

/***** array de configuração da COMM *****/
byte Lo_P[] = {0xC2, 0x00, 0x00, 0x18, 0xA, 0x47};
byte Hi_P[] = {0xC2, 0x00, 0x00, 0x18, 0xA, 0x44};

/***** funcoes de transmissao *****/
void LoRa_LoPWR();
void LoRa_HiPWR();

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

  /***** Garante que as flags iniciam em estado seguro *****/
  WDT_TMP = WD_RST;
  WDT_BMP = WD_RST;
  WDT_GPS = WD_RST;
  WDT_EPS = WD_RST;
  WDT_FIL = WD_RST;

  /***** Ativa o Watchdog de Hardware *****/
  wdt_enable(WDTO_8S);
  
  /***** inicializa portas seriais *****/
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

  /***** seleciona referencia de tensão externa 3,3V - SOMENTE PLACA MODIFICADA*****/
  analogReference(EXTERNAL);

  /***** inicializa sensor de pressao atmosferica *****/
  baro.begin();

  /***** inicializa SD Card e cria arquivo *****/
  SDcard.begin(CS_SD, SPI_HALF_SPEED);
  LogFILE.open("teste3.txt" , O_RDWR | O_CREAT | O_AT_END);

  MyData.CNT = 0;
}

/************** PROGRAMA PRINCIPAL **************/
void loop() {
  
  switch (estado){
    
    /*******inicializacao do sistema********/
    case INICIALIZACAO:{  
      smartdelay(2000);
      
      WDT_GPS = WD_SET;
      WDT_EPS = WD_SET;

      GPS_aquisition();
      EPS_aquisition();      
      while((MyData.Sats < 4|| MyData.Sats > 40) && MyData.Vbat > Vbat_min){ 
        LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
        
        wdt_enable(WDTO_8S); 
        
        smartdelay(2000);
        
        // Seta as flags antes de ler
        WDT_BMP = WD_SET;
        WDT_EPS = WD_SET;
        WDT_GPS = WD_SET;
        WDT_TMP = WD_SET;
        
        BMP_aquisition();
        EPS_aquisition();
        GPS_aquisition();
        TMP_aquisition();
        
        // ADICIONADO: Reseta o Watchdog enquanto espera o GPS!
        watchdog_check(); 
        }
      if(MyData.Vbat <= Vbat_min)
        estado = SEGURANCA;
        else 
        estado = OPER_1;
      break;
    }
    
    /******* operacao normal durante o dia *******/
    case OPER_1:{ 
      smartdelay(2000);
      min_ant = MyData.Minute;
      ultimo_millis = millis();
      while(MyData.Hour < 21 && MyData.Hour >= 11 && MyData.Vbat > Vbat_min){  //se hora entre 21 e 11 UTC -> Operação noturna. Se Vbat baixo -> modo de segurança
        LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);

        wdt_enable(WDTO_8S);
        
        smartdelay(2000);
        
        WDT_TMP = WD_SET;
        WDT_BMP = WD_SET;
        WDT_GPS = WD_SET;
        WDT_EPS = WD_SET;
        WDT_FIL = WD_SET;
        
        TMP_aquisition();
        BMP_aquisition();
        GPS_aquisition();
        EPS_aquisition();
        FIL_print();
        if (MyData.Minute != min_ant || (millis() - ultimo_millis) > 90000) //envia dados pelo COMM a cada minuto do GPS ou a cada 1'30 se sem sinal de GPS
          {
          if (MyData.Minute % 10 == 0)   //transmite 30dB
            {
            LoRa_HiPWR();
            min_ant = MyData.Minute;
            }
           else                          //transmite 20dB
            {            
            LoRa_LoPWR();
            min_ant = MyData.Minute;
            }
          ultimo_millis = millis(); 
          }
        MyData.CNT ++;
        watchdog_check();
        }
      if(MyData.Vbat <= Vbat_min)
        estado = SEGURANCA;
        else 
        estado = NOITE;
      break;
    }
    
    /*********operacao normal durante a noite**********/
    case NOITE: {
      smartdelay(2000);
      while((MyData.Hour >= 21 || MyData.Hour < 11) && MyData.Vbat > Vbat_min){
        LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);

        wdt_enable(WDTO_8S);
        
        smartdelay(2000);
        
        WDT_TMP = WD_SET;
        WDT_BMP = WD_SET;
        WDT_GPS = WD_SET;
        WDT_EPS = WD_SET;
        WDT_FIL = WD_SET;
        
        TMP_aquisition();
        BMP_aquisition();
        GPS_aquisition();
        EPS_aquisition();
        FIL_print();
        MyData.CNT ++;
        watchdog_check();
        }
      if(MyData.Vbat <= Vbat_min)
        estado = SEGURANCA;
        else 
        estado = OPER_1;
      break;
    }
    
    /**********operacao de seguranca (bateria baixa)**********/
    case SEGURANCA:{
     smartdelay(2000);
     while(MyData.Vbat <= Vbat_min){
        LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);

        wdt_enable(WDTO_8S);
        
        WDT_EPS = WD_SET;

        EPS_aquisition();
        watchdog_check();
        }
      estado = OPER_1;
      break;
      }
    }
                   
}

/********* Funcoes auxiliares *******************/

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  while (millis() - start < ms){
    if (UART_GNSS.available())
      gps.encode(UART_GNSS.read());
  } 
}

void TMP_aquisition() {
  //comando para os sensores de temperatura fazerem uma conversao
  sensortemp.requestTemperatures();

  /***** carrega dados dos sensores de temperatura *****/
  MyData.Temp1 = sensortemp.getTempCByIndex(0) * 10;
  MyData.Temp2 = sensortemp.getTempCByIndex(1) * 10;
  MyData.Temp3 = sensortemp.getTempCByIndex(2) * 10;

  WDT_TMP = WD_RST;
}

void BMP_aquisition() {

  MyData.Pressure =  baro.readPressure();                          //le valor de pressao atmosferica e converte para kPa
  MyData.Temp_BMP180 = baro.readTemperature() * 10;                //le temperatura do barometro
  MyData.Baro_Alt = baro.readAltitude(seaLevelPressure_hPa * 100); //le altitude barometrica em relacao ao valor de calibracao

  WDT_BMP = WD_RST;
}

void GPS_aquisition() {

  /***** carrega dados de GPS *****/
  gps.crack_datetime(&MyData.Year, &MyData.Month, &MyData.Day, &MyData.Hour, &MyData.Minute, &MyData.Second, &Hundredths, &MyData.Age);
  gps.get_position(&MyData.Latitude, &MyData.Longitude, &MyData.Age);
  MyData.Altitude = gps.altitude() / 100;
  if (gps.course() <= 36000)
    MyData.Heading = gps.course() / 100;
  else
    MyData.Heading = 0;
  MyData.Speed = gps.f_speed_kmph();
  MyData.HDOP = gps.hdop();
  MyData.Sats = gps.satellites();

  WDT_GPS = WD_RST;
}

void EPS_aquisition() {

  MyData.Vbat = analogRead(AnalogVbat);
  MyData.Vpv  = analogRead(AnalogVpv);

  WDT_EPS = WD_RST;
}

void LoRa_HiPWR() {
  UART_COMM.write(Hi_P, sizeof(Hi_P)); delay(100);
  digitalWrite(4, LOW); digitalWrite(5, LOW); delay(100);
  E32.SendStruct(&MyData, sizeof(MyData));
  digitalWrite(4, HIGH); digitalWrite(5, HIGH); delay(100);
}

void LoRa_LoPWR() {
  UART_COMM.write(Lo_P, sizeof(Lo_P)); delay(100);
  digitalWrite(4, LOW); digitalWrite(5, LOW); delay(100);
  E32.SendStruct(&MyData, sizeof(MyData));
  digitalWrite(4, HIGH); digitalWrite(5, HIGH); delay(100);
}

void FIL_print() {

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

  WDT_FIL = WD_RST;
}
void watchdog_check() {
  // Exige a assinatura exata de sucesso (0x55) de todas as flags.
  // Protege contra travamentos e corrupção de memória por radiação cósmica (SEU).
  if (WDT_TMP == WD_RST && 
      WDT_BMP == WD_RST && 
      WDT_GPS == WD_RST && 
      WDT_EPS == WD_RST && 
      WDT_FIL == WD_RST) {
        
    wdt_reset(); // "Alimenta o cão" - zera o temporizador de hardware
  }
}
