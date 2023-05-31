/*
 * 
 * program 
 * training kit mbtcp esp8266
 * pcb local <<COKLAT>>
 * 
 */

 /*
  * rev. 20230515  * 
  * rev. 20230406  *
  * 
  */




//-----------MBTCP------------------
#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else //ESP32
 #include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>
//-----------MBTCP end------------------


//--------------DS1820-------------------------

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//ds1820
// GPIO where the DS18B20 is connected to
const int oneWireBus = D4;      //GPIO2 = D4;  ------------------------<<<<<

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
//-------------------------------------------------





int i = 0;
int statusCode;
const char* ssid = "Tamu Eldikon";
const char* passphrase = "eldikonguest";

int ModbusTCP_port = 502;
/*-------------------------*/
//TK PIN DEFINE 
#define PINSW D2
#define BZ D1
#define LED1 D5
#define LED2 D6
#define LED3 D7
#define LED4 D8
//#define LEDBI D4
/*-------------------------*/

/*===================================================*/

//===============================================================
//===============================================================
//MODBUS REGISTER MAP
const int LED1_COIL = 0; //00001
const int LED2_COIL = 1; //00002
const int LED3_COIL = 2; //00003
const int LED4_COIL = 3; //00004
const int BZ_COIL = 4; //00005
const int PBSWITCH_Ists = 0; //10001
const int AI_IREG = 0; //30001
const int SUHU_IREG = 1; //30002
const int AQ_HREG = 0; //40001

//ModbusIP object
ModbusIP mb;
//===============================================================
//-------------------------end declare------------------------

long ts;

int TM;

/*=============================== setup ==============================*/
void setup() 
{
  Serial.begin(115200);
  //=================================
  pinMode(PINSW,INPUT);
  pinMode(BZ,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  //pinMode(LEDBI,OUTPUT);
  //==================================
  //==================================
  delay(3000);
  //==================================
  Serial.println();
  Serial.println("Disconnecting current wifi connection");
  WiFi.disconnect();
  delay(10);
  
   ts = millis();
    
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  
  digitalWrite(BZ,HIGH);
  delay(100);
  digitalWrite(BZ,LOW);
  delay(1000);
  

//----------------------------------------------------------- 
 IPAddress IP(192,168,24,70);
 IPAddress NETMASK(255,255,255,0);
 IPAddress NETWORK(192,168,24,1);
 IPAddress DNS(8,8,8,8);
 WiFi.config(IP, NETWORK, NETMASK, DNS);  

  
  WiFi.mode(WIFI_STA);
  //====================================
  WiFi.begin(ssid, passphrase );
  Serial.println();
  Serial.println("Waiting.");
  
  while ((WiFi.status() != WL_CONNECTED))
  {     
     Serial.print("."); //cetak . dilayar SerialMonitor
     delay(250);
  }
  //-----------------  

  Serial.println("Connected ");
  Serial.println("ESP8266 Slave Modbus TCP/IP ");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(String(ModbusTCP_port));
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.println("Modbus TCP/IP Online");
  Serial.println(" ");
    delay(100);
 
    //---------berhasil koneksi ke wifi router, ----------------

//----------------------after connected----------------------
  mb.server();
  mb.addCoil(LED1_COIL);//00001
  mb.addCoil(LED2_COIL);//00002
  mb.addCoil(LED3_COIL);//00003
  mb.addCoil(LED4_COIL);//00004
  mb.addCoil(BZ_COIL);//00005
  mb.addIsts(PBSWITCH_Ists); //10001 
  mb.addIreg(AI_IREG);    //30001 
  mb.addIreg(SUHU_IREG);  //30002
  mb.addHreg(AQ_HREG);    //40001 

  mb.Ireg(SUHU_IREG, 257);
  
  delay(500);
  digitalWrite(BZ,HIGH);
  delay(250);
  digitalWrite(BZ,LOW);
  delay(250);
  digitalWrite(BZ,HIGH);
  delay(250);
  digitalWrite(BZ,LOW);
  delay(3000);
    /*---------------------------------------------------------------*/   
}
 /*--------------------- end setup function -------------------*/


void loop() 
{
      mb.task();

      digitalWrite(LED1,mb.Coil(LED1_COIL));  //00001
      digitalWrite(LED2,mb.Coil(LED2_COIL));  //00002
      digitalWrite(LED3,mb.Coil(LED3_COIL));  //00003
      //digitalWrite(LED4,mb.Coil(LED4_COIL));  //00004
      digitalWrite(BZ,mb.Coil(BZ_COIL));      //00005

      analogWrite(LED4,mb.Hreg(AQ_HREG));     //40001
      
      mb.Ists(PBSWITCH_Ists,digitalRead(PINSW)); //10001 
    
      //Read each 1 seconds
      if (millis() > ts + 1000) {
        ts = millis();
        suhu();
        mb.Ireg(AI_IREG,analogRead(A0));  //30001 
        mb.Ireg(SUHU_IREG, TM);           //30002
        
      }       
      
      delay(10); 
}//---------end loop



  void suhu()
{
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  //float temperatureF = sensors.getTempFByIndex(0);
 
  TM= int(temperatureC*10);
}
