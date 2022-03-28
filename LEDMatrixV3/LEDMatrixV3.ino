/*******************************************************************************************
 **                              www.osmancitci.com.tr                                    **
 **                             Hayat Oyun Gibi Boşver                                    **
 *******************************************************************************************
 ** Çizen    : Osman Çitçi                                                                **
 ** Tarih    : Ocak 2022                                                                  **
 ** Versiyon : 3.0                                                                        ** 
 ** Uyum     : NodeMCU V3 ESP8266 ESP-12E ile Uyumludur.                                  **
 *******************************************************************************************
*/

/************************( Kullanılan Kitaplıklar )************************/

#include "Arduino.h"
#include <ESP8266WiFi.h>                    // Kütüphaneden Yükleyebilirsin..
#include <ArduinoJson.h>                    // Kütüphaneden Yükleyebilirsin..
#include <Timezone.h>                       // https://github.com/JChristensen/Timezone
#include <TimeLib.h>                        // https://github.com/PaulStoffregen/Time
#include <WiFiUdp.h>                        // Kütüphaneden Yükleyebilirsin..
#include <ArduinoOTA.h>                     // Kütüphaneden Yükleyebilirsin..


/************************************(Kablosuz Bağlantı Ayarları)**********************************/
const char* ssid     = "xxxxxxxxxx";     // SSID ADI
const char* password = "xxxxxxxxxx";   // WIFI SIFRESI

/**************************(openweathermap.org Api Kodu)*******************************/
String HavaDurumuApi = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; //openweathermap.org Api Kodu https://home.openweathermap.org/api_keys
String HavaDurumuDili = "&lang=tr";
String cityID = "xxxxxx"; //Şehir https://openweathermap.org/city/xxxxxx


// Zaman Dilimi Ayarları:
static const char ntpServerName[] = "tr.pool.ntp.org";    //Time Zone Ayarı İçin http://www.pool.ntp.org/zone/tr
const int timeZone = 0;                    
WiFiUDP Udp;
unsigned int localPort = 8888;              // UDP Paketlerini Dinlemek İçin Port
time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);

// - Timezone. - //
// Saat Diliminize Ve Yaz Saatine Göre Düzenleyiniz.
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 1, 180};     //Avrupa Zaman Dilimi
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 1, 120};       //Avrupa Zaman Dilimi (Istanbul, Turkey)
Timezone CE(CEST, CET);

WiFiClient client;
/********************( Değişkenler )********************/
String HavaDurumuAna = "";
String HavaDurumuAciklama = "";
String HavaDurumuKonum = "";
String Ulke;
int Nem;
int Basinc;
float Sicaklik;
float SicaklikMin, SicaklikMax;
int Bulutlar;
float RuzgarHizi;
int RuzgarDerecesi;
String date;
int DurumKodu;
String WSicaklik, Tarih, Saat;
String HavaDurumu;
#define NUM_MAX 4
#define ROTATE 0

// ESP-01
//#define DIN_PIN 2 // D4
//#define CS_PIN  3 // D9/RX
//#define CLK_PIN 0 // D3

// NodeMCU 3.0
#define DIN_PIN D7  // D8
#define CS_PIN  D8  // D7
#define CLK_PIN D5  // D6

#include "max7219.h"
#include "fonts.h"


bool bNTPStarted = false;
bool gbIsConnecting = false;

#define MAX_DIGITS 4
byte dig[MAX_DIGITS] = {0};
byte digold[MAX_DIGITS] = {0};
byte digtrans[MAX_DIGITS] = {0};
int updCnt = 0;
int dots = 0;
long dotTime = 0;
long clkTime = 0;
int dx = 0;
int dy = 0;
byte del = 0;
int h, m, s, w, ay, yil, gun;
const char* wochenstring;
float utcOffset = 2;
long localEpoc = 0;
long localMillisAtUpdate = 0;

/*****************************************( Ayarlar )****************************************/
void setup()
{
// NOT : PORT 9600 KULLANIYORUM ISTERSENIZ 115200 KULLANABİLİRSİNİZ.
  Serial.begin(9600);
  //Serial.begin(115200);
  initMAX7219();
  sendCmdAll(CMD_SHUTDOWN, 1);
  sendCmdAll(CMD_INTENSITY, 5); //PARLAKLIK
  Serial.print("Wifiye Bağlanıyor. ");
  WiFi.begin(ssid, password);

  printStringWithShift(" Bekleyin ", 15,font,' ');
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Bağlanılan IP: "); Serial.println(WiFi.localIP());

  Udp.begin(localPort);
  Serial.print("Local Port: ");
  Serial.println(Udp.localPort());
  Serial.println("Senkranisyon Yapılıyor..");
  setSyncProvider(getNtpTime);
  setSyncInterval(86400);  // Yeniden Eşitlemeler Arasındaki Saniye Sayısını Girin. 86400 = 1 gün
}
/*************************************(Ana Program)**************************************/
void loop()
{
  if (updCnt <= 0) { // Her 10 Saniyede Bir, ~450s=7.5m
    updCnt = 10;
    Serial.println("Sunucuya Bağlanıyor.");
    HavaDurumuData();
    getTimeLocal();
    Serial.println("Bağlantı Sağlandı HAZIR..");
    Serial.println("-------------");
    clkTime = millis();
  }
  
  if (millis() - clkTime > 15000 && !del && dots) { // 15 Saniyede Saat, 30 Saniyede Hava Durumunu Gösterir..
    printStringWithShift(HavaDurumu.c_str(), 45,font,' ');
    updCnt--;
    clkTime = millis();
  }

  if (millis() - dotTime > 500) {
    dotTime = millis();
    dots = !dots;
  }
  getTimeLocal();
  showAnimClock();
}
