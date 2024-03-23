#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define WIFI_SSID "SSID" //Название сети
#define WIFI_PASSWORD "PASS" //Пароль от сети
#define BOT_TOKEN "API" // API токен от вашего бота

#define PIN_MQ2  A0

const unsigned long BOT_MTBS = 1000; 

unsigned long bot_lasttime; 

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

int value;
String convertValue;
String isGas;

void checkGas()
{
   value = analogRead(PIN_MQ2);
   convertValue = value;
   if (value > 450) { isGas = "ppm, превышение нормы газа"; }
   else { isGas = "ppm, уровень газа в норме"; }

}

void setupConn()
{
  Serial.begin(115200);
  Serial.println();
  pinMode(D1, OUTPUT);
  configTime(0, 0, "pool.ntp.org");   
  secured_client.setTrustAnchors(&cert); 
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}
void inConnected()
{
  if(WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(D1, LOW);
    setupConn();
  }
  else
  {
   digitalWrite(D1, HIGH);
  }
}
void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);
  
  String answer;
  for (int i = 0; i < numNewMessages; i++)
  {
    telegramMessage &msg = bot.messages[i];
    Serial.println("Received " + msg.text);
    if (msg.text == "/help")
      answer = "So you need _help_, uh? me too! use /start or /status";
    else if (msg.text == "/start")
    {
      checkGas();
      answer = "Привет, данные о газа: " + convertValue + isGas;
    }
    else if (msg.text == "/status")
      answer = "All is good here, thanks for asking!";
    else
      answer = "Say what?";

    bot.sendMessage(msg.chat_id, answer, "Markdown");
  }
}

void bot_setup()
{
  const String commands = F("["
                            "{\"command\":\"start\", \"description\":\"Получить данные о газе\"},"
                            "]");
  bot.setMyCommands(commands);
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  pinMode(D1, OUTPUT);
  configTime(0, 0, "pool.ntp.org");   
  secured_client.setTrustAnchors(&cert); 
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot_setup();
}


void loop()
{
  
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
    inConnected();
  }
}
