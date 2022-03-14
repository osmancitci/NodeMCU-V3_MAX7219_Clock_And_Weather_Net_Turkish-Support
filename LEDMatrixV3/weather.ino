/*************************************(openweathermap.org)**************************************/

const char *HavaDurumuHost = "api.openweathermap.org";

void HavaDurumuData()
{
  Serial.print("Bağlanıyor.. "); Serial.println(HavaDurumuHost);
  if (client.connect(HavaDurumuHost, 80)) {
    client.println(String("GET /data/2.5/weather?id=") + cityID + "&units=metric&appid=" + HavaDurumuApi + HavaDurumuDili + "\r\n" +
                   "Host: " + HavaDurumuHost + "\r\nUser-Agent: ArduinoWiFi/1.1\r\n" +
                   "Connection: close\r\n\r\n");
  } else {
    Serial.println("Bağlantı Hatası");
    return;
  }
  Serial.println("-------------");
  String line;
  //Serial.println(line);

  while(client.connected() && !client.available()) 
  delay(1);                                          //Data Bekleniyor
  while (client.connected() && client.available()) {
    char c = client.read();
    if (c == '[' || c == ']') c = ' ';  // Hava Durumu Kimliği Dizi Halinde Geldiğinde Hata Veriyor..
    line += c;
  }

  client.stop();

 // Serial.println(line);
//Json Kapasite Hesaplama
  const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 420;
  DynamicJsonBuffer jsonBuf(capacity);
 
  JsonObject &root = jsonBuf.parseObject(line);
  if (!root.success())
  {
    Serial.println("parseObject() Hatası");
    return;
  }
  DurumKodu = root["weather"]["id"];  // Genel Hava Durumu ID
  //HavaDurumuAna = root["weather"]["main"].as<String>();
  HavaDurumuAciklama = root["weather"]["description"].as<String>();
  HavaDurumuAciklama.toLowerCase();
  //  HavaDurumuKonum = root["name"].as<String>();
  //  Ulke = root["sys"]["country"].as<String>();
  Sicaklik = root["main"]["temp"];
  Nem = root["main"]["humidity"];
  Basinc = root["main"]["pressure"];
  Basinc = Basinc *0.75006;
  SicaklikMin = root["main"]["temp_min"];
  SicaklikMax = root["main"]["temp_max"];
  RuzgarHizi = root["wind"]["speed"];
  RuzgarDerecesi = root["wind"]["deg"];
  Bulutlar = root["clouds"]["all"];
  Serial.print("Hava Kodu = ");
  Serial.println(DurumKodu);  //
  Serial.print("Hava Durumu = ");
  Serial.println(HavaDurumuAciklama);
  
 getTimeLocal();
// Rüzgar Yönleri
  String RuzgarYonleri = "K KDD GDG GBB KB";  // Rüzgar Yönleri Hesaplama..  K KKD KD DKD D DGD GD GGD G GGB GB BGB B BKB KB KKB
  int wr = (RuzgarDerecesi+22)%360/45;
//Günler
  String Gunler[] = {"Hata", "Pazar", "Pazartesi", "Salı", "Çarşamba", "Persembe", "Cuma", "Cumartesi"};
//Rüzgar Yönleri ve Derece 
  String Derece = String(char('~' + 23));
  String YonK = String(char('~' + 24));
  String YonG = String(char('~' + 28));
  String YonD = String(char('~' + 26));
  String YonB = String(char('~' + 30));
  String YonKD = String(char('~' + 25));
  String YonKB = String(char('~' + 31));
  String YonGD = String(char('~' + 27));
  String YonGB = String(char('~' + 29));
//Hava Durumu Verilerini Yazdırıyoruz.
  HavaDurumu = "      " +  Gunler[w]  + "  " + String(gun) + "." + String(ay) + "." + String(yil)+ "      ";
  HavaDurumu +=   String(Sicaklik, 1) + Derece + "C ";
  HavaDurumu += "  Nem: " + String(Nem) + "%  ";  //Nem Verisi
//HavaDurumu += "  Basinc: " + String(Basinc) + "hPa  "; // Hava Basıncı
//Gelen Hava Koduna Göre Hava Durumunu Yazdırma
if ((DurumKodu >= 200) and (DurumKodu <= 232)) HavaDurumu += "Fırtına, ";
if ((DurumKodu >= 300) and (DurumKodu <= 321)) HavaDurumu += "Az Yağmur, ";
if ((DurumKodu >= 500) and (DurumKodu <= 531)) HavaDurumu += "Yağmur, ";
if ((DurumKodu >= 600) and (DurumKodu <= 622)) HavaDurumu += "Kar, ";
if ((DurumKodu >= 701) and (DurumKodu <= 781)) HavaDurumu += "Sis, ";
if (DurumKodu == 800) HavaDurumu += "Hava Açık, ";
if (DurumKodu == 801) HavaDurumu += "Az Bulutlu, ";
if (DurumKodu == 802) HavaDurumu += "Parçalı Bulutlu, ";
//if ((DurumKodu >= 801) and (DurumKodu <= 802)) HavaDurumu += "Bulutlu,  ";
if ((DurumKodu >= 803) and (DurumKodu <= 804)) HavaDurumu += "Çok Bulutlu,  ";
//if ((DurumKodu >= 801) and (DurumKodu <= 804)) HavaDurumu += "Bulut: " + String(Bulutlar) +"%  "; // Yağmur Riski Hesaplama
//if (DurumKodu != 800) HavaDurumu += String(Bulutlar) +"%  "; // Yağmur Riski % Verisi

//Rüzgar Verisi
if (RuzgarHizi != 0)
{
  HavaDurumu += "  Rüzgar: " + String(3.6*RuzgarHizi, 0) + "km/h  "; // Rüzgar Hızı
if (RuzgarDerecesi>=337.5 || RuzgarDerecesi<=22.5) HavaDurumu +=  YonK; // K = 0
if (RuzgarDerecesi>=22.5 && RuzgarDerecesi<=67.6) HavaDurumu +=  YonKD; // KD = 45
if (RuzgarDerecesi>=67.5 && RuzgarDerecesi<=112.5) HavaDurumu +=  YonD;  // D = 90
if (RuzgarDerecesi>=112.5 && RuzgarDerecesi<=157.5) HavaDurumu +=  YonGD; // GD = 135
if (RuzgarDerecesi>=157.5 && RuzgarDerecesi<=202.5) HavaDurumu +=  YonG; // G = 180
if (RuzgarDerecesi>=202.5 && RuzgarDerecesi<=247.5) HavaDurumu +=  YonGB; // GB =225
if (RuzgarDerecesi>=247.5 && RuzgarDerecesi<=292.5) HavaDurumu +=  YonB; // B = 270
if (RuzgarDerecesi>=292.5 && RuzgarDerecesi<=337.5) HavaDurumu +=  YonKB;  // KB = 315
  //HavaDurumu += String(RuzgarDerecesi) + Derece + "  ";
   HavaDurumu +=  RuzgarYonleri.substring(2*wr,2*wr+2) + " (" + String(RuzgarDerecesi) + Derece + ")           "; // Rüzgar Hızı ve Derecesi
}
  //Serial.println(DurumKodu);
  Serial.println(HavaDurumu);
}


// =======================================================================
