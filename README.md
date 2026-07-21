# 🎓 Smart Attendance System using ESP32 & Firebase

A professional IoT-based Smart Attendance System developed using ESP32, RFID (RC522), Firebase Realtime Database, and a Web Registration Interface.

This project allows students to register their RFID cards through a web page and automatically records attendance by scanning the RFID card.

---

# 🚀 Features

✅ RFID Student Registration

✅ Automatic Attendance

✅ Firebase Realtime Database

✅ ESP32 Web Server

✅ LCD I2C Display

✅ Buzzer Notification

✅ Green & Red LED Status

✅ Register Mode Button

✅ Fast RFID Authentication

---

# 🛠 Hardware Used

- ESP32 DevKit V1
- RC522 RFID Module
- RFID Cards / Tags
- 16x2 LCD I2C
- Push Button
- Green LED
- Red LED
- Active Buzzer
- Breadboard
- Jumper Wires

---

# 💻 Software Used

- Arduino IDE
- Firebase Realtime Database
- ESP32 Board Package
- GitHub

---

# 📚 Libraries

- WiFi
- SPI
- MFRC522
- LiquidCrystal_I2C
- Firebase_ESP_Client
- ArduinoJson
- NTPClient
- WebServer

---

# ⚙️ Pin Configuration

| Device | ESP32 Pin |
|---------|-----------|
| RC522 SDA | GPIO 5 |
| RC522 RST | GPIO 4 |
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |
| Green LED | GPIO 26 |
| Red LED | GPIO 27 |
| Buzzer | GPIO 25 |
| Push Button | GPIO 33 |

---

# 🌐 System Workflow

## Student Registration

1. Press Register Button

2. Open ESP32 Web Page

3. Enter

- Student Name
- Registration Number
- Department

4. Click Save

5. Scan RFID Card

6. Student information is automatically stored in Firebase.

---

## Attendance

Student scans RFID card.

↓

ESP32 searches Firebase.

↓

If UID exists

↓

Student Name displayed on LCD

↓

Attendance automatically saved.

---

# 🔥 Firebase Database Structure

```

Students

UID

Name

RegNo

Department

UID

Attendance

Date

UID

Time

Name

RegNo

```

---

# 📷 Project Images

- Circuit Diagram
- Hardware Setup
- LCD Display
- Web Registration Page
- Firebase Database
- GitHub Repository

---

# 👨‍💻 Author

## Sasidu-Tech

Electronics | Arduino | IoT | Embedded Systems

GitHub:
https://github.com/Sasidu-Tech

---

## ⭐ If you like this project, don't forget to Star this repository.
