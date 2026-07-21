/*****************************************************
 * Smart Attendance System V1.0
 * ESP32 + RC522 + Firebase + LCD + RFID
 * Author : Sasidu-Tech
 *****************************************************/

//====================== LIBRARIES ======================

#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WebServer.h>

//====================== WIFI ===========================

#define WIFI_SSID      "Infinix HOT 40 Pro"
#define WIFI_PASSWORD  "12345678"

//====================== FIREBASE =======================


#define API_KEY       "YOUR_API_KEY "
#define DATABASE_URL  "YOUR_DATABASE_URL"

//====================== RFID ===========================

#define SS_PIN     5
#define RST_PIN    4

MFRC522 rfid(SS_PIN, RST_PIN);

//====================== LCD ============================

LiquidCrystal_I2C lcd(0x27,16,2);

//====================== LED ============================

#define GREEN_LED   26
#define RED_LED     27

//====================== BUZZER =========================

#define BUZZER      25

//====================== BUTTON =========================

#define BUTTON_PIN  33

//====================== FIREBASE =======================

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

//====================== TIME ===========================

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800);

//====================== VARIABLES ======================

WebServer server(80);

bool registerMode = false;
bool detailsSaved = false;

String cardUID = "";

String studentName = "";
String regNo = "";
String department = "";

String tempName = "";
String tempRegNo = "";
String tempDepartment = "";

const char webpage[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<title>Smart Attendance</title>

<meta name="viewport" content="width=device-width, initial-scale=1">

<style>

body{

font-family:Arial;

background:#f2f2f2;

text-align:center;

}

input{

width:280px;

padding:12px;

margin:10px;

font-size:18px;

}

button{

padding:12px 25px;

font-size:18px;

background:#0b8f2d;

color:white;

border:none;

cursor:pointer;

}

</style>

</head>

<body>

<h2>Smart Attendance System</h2>

<form action="/save">

<input name="name" placeholder="Student Name"><br>

<input name="reg" placeholder="Registration Number"><br>

<input name="dep" placeholder="Department"><br>

<button type="submit">

Save Student

</button>

</form>

<br>

<h3>After Saving Scan RFID Card</h3>

</body>

</html>

)rawliteral";



//====================== FUNCTIONS ======================

void setupWiFi() {

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(0,1);
    lcd.print("Please Wait...");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");
  delay(1000);
}

void setupFirebase() {

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Firebase.begin(&config, &auth);

if (Firebase.signUp(&config, &auth, "", ""))
{
  Serial.println("Firebase Anonymous Login OK");
}
else
{
  Serial.println(config.signer.signupError.message.c_str());
}

Firebase.reconnectWiFi(true);

  Serial.println("Firebase Ready");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Firebase Ready");
  delay(1000);

  server.on("/",handleRoot);

  server.on("/save",handleSave);

  server.begin();

  Serial.println("Web Server Started");

  Serial.print("Open Browser : ");

  Serial.println(WiFi.localIP());

}


void setupRFID() {

  SPI.begin();
  rfid.PCD_Init();

  Serial.println("RFID Ready");
}

void setupLCD() {

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SMART");
  lcd.setCursor(0,1);
  lcd.print("ATTENDANCE");

  delay(2000);
}

void setupLED() {

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
}

void setupBuzzer() {

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
}

void setupButton() {

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void handleRoot(){

server.send(200,"text/html",webpage);

}

void handleSave(){

tempName = server.arg("name");

tempRegNo = server.arg("reg");

tempDepartment = server.arg("dep");

detailsSaved = true;

Serial.println("Student Details Saved");

Serial.println(tempName);

Serial.println(tempRegNo);

Serial.println(tempDepartment);

lcd.clear();

lcd.setCursor(0,0);

lcd.print("Details Saved");

lcd.setCursor(0,1);

lcd.print("Scan RFID");

server.send(200,"text/html",

"<h2>Details Saved Successfully.<br>Now Scan RFID Card.</h2>");

}

void setup() {

  Serial.begin(115200);

  setupLCD();
  setupLED();
  setupBuzzer();
  setupButton();
  setupWiFi();
  setupFirebase();
  setupRFID();

  timeClient.begin();

  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Attendance");
  lcd.setCursor(0,1);
  lcd.print("Scan Your Card");

  Serial.println("System Ready...");
}

void loop() {

  timeClient.update();

  // Button Check
  if (digitalRead(BUTTON_PIN) == LOW) {
    registerMode = true;
  } else {
    registerMode = false;
  }

  // RFID Check
  checkRFID();

  server.handleClient();
}


//====================== RFID FUNCTIONS ======================

// UID එක String එකක් විදියට හදන function
String getCardUID() {

  String uid = "";

  for (byte i = 0; i < rfid.uid.size; i++) {

    if (rfid.uid.uidByte[i] < 0x10)
      uid += "0";

    uid += String(rfid.uid.uidByte[i], HEX);
  }

  uid.toUpperCase();

  return uid;
}

// RFID Card Scan
void checkRFID() {

  if (!rfid.PICC_IsNewCardPresent())
    return;

  if (!rfid.PICC_ReadCardSerial())
    return;

  cardUID = getCardUID();

  Serial.println("======================");
  Serial.print("Card UID : ");
  Serial.println(cardUID);

  lcd.clear();

  if (registerMode) {

    lcd.setCursor(0,0);
    lcd.print("REGISTER MODE");

    lcd.setCursor(0,1);
    lcd.print("Card Scanned");

    Serial.println("REGISTER MODE");

    //================ SAVE TO FIREBASE =================

if(detailsSaved)
{

  String path = "/Students/" + cardUID;

  Firebase.RTDB.setString(&fbdo, path + "/Name", tempName);
  Firebase.RTDB.setString(&fbdo, path + "/RegNo", tempRegNo);
  Firebase.RTDB.setString(&fbdo, path + "/Department", tempDepartment);
  Firebase.RTDB.setString(&fbdo, path + "/UID", cardUID);

  if (fbdo.httpCode() == 200) {
  Serial.println("Firebase Write OK");
} else {
  Serial.print("Firebase Error: ");
  Serial.println(fbdo.errorReason());
}

  Serial.println("Student Registered Successfully");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("REGISTERED");
  lcd.setCursor(0,1);
  lcd.print(tempName);

  digitalWrite(GREEN_LED,HIGH);
  delay(1500);
  digitalWrite(GREEN_LED,LOW);

  detailsSaved = false;
}
else
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Save Details");
  lcd.setCursor(0,1);
  lcd.print("First");

  digitalWrite(RED_LED,HIGH);
  delay(1500);
  digitalWrite(RED_LED,LOW);
}

  } else {

   lcd.setCursor(0,0);
lcd.print("Checking...");

String path = "/Students/" + cardUID;

if(Firebase.RTDB.getString(&fbdo, path + "/Name"))
{
    studentName = fbdo.stringData();

    Firebase.RTDB.getString(&fbdo, path + "/RegNo");
    regNo = fbdo.stringData();

    Firebase.RTDB.getString(&fbdo, path + "/Department");
    department = fbdo.stringData();

    Serial.println("Student Found");
    Serial.println(studentName);
    Serial.println(regNo);
    Serial.println(department);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(studentName);

    lcd.setCursor(0,1);
    lcd.print(regNo);

    digitalWrite(GREEN_LED,HIGH);

    digitalWrite(BUZZER,HIGH);
    delay(200);
    digitalWrite(BUZZER,LOW);

    delay(2000);

    digitalWrite(GREEN_LED,LOW);
}
else
{
    Serial.println("Unknown Card");

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Unknown Card");

    digitalWrite(RED_LED,HIGH);

    digitalWrite(BUZZER,HIGH);
    delay(1000);
    digitalWrite(BUZZER,LOW);

    digitalWrite(RED_LED,LOW);

    delay(2000);
}
  }

  digitalWrite(BUZZER, HIGH);
  delay(150);
  digitalWrite(BUZZER, LOW);

  delay(1500);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Attendance");
  lcd.setCursor(0,1);
  lcd.print("Scan Your Card");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
