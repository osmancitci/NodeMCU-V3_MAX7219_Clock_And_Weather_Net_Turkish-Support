/*-------- NTP Kodu ----------*/

const int NTP_PACKET_SIZE = 48; // iLK 48bytında NTP zAMANI
byte packetBuffer[NTP_PACKET_SIZE]; //Gelen Giden Veri Paketleme

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP Server IP Adresi

  while (Udp.parsePacket() > 0) ; // Tüm Paketleri At
  Serial.println("NTP Alınıyor..");
  // Rastgele Sunucu Çek
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("NTP Yanıt Alınıyor");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // Arabelleği Okuma
      unsigned long secsSince1900;
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("NTP Yanıtı Yok");
  return 0; //Zaman Belirlenmezse 0 Yap
}

// NTP Isteği Gönderim
void sendNTPpacket(IPAddress &address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;   // Sürüm
  packetBuffer[1] = 0;     // Saat Türü
  packetBuffer[2] = 6;     // Yanıt Aralığı
  packetBuffer[3] = 0xEC;  // Saat Hassasiyeti
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  Udp.beginPacket(address, 123); //NTP Istekleri 123 Portu Ustunden Yapılmalıdır.
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
