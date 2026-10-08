# 📋 Digital Attendance System

An ESP32-based smart attendance and presence monitoring system that combines **RFID authentication** with **BLE-based continuous presence tracking** to provide secure check-in/check-out, real-time monitoring, and reliable attendance management.

---

## 🚀 Overview

Traditional attendance systems usually verify a person only when they enter or leave a premises. After check-in, the system may continue to consider the person present even if they leave without checking out.

The **Digital Attendance System** addresses this limitation by combining two layers of authentication:

- 🔐 **RFID** for formal attendance check-in/check-out
- 📡 **BLE** for continuous presence verification
- 📊 **Live monitoring** for real-time attendance status
- 🚨 **Anomaly detection** for identifying unexpected absence
- 👥 **Emergency headcount** for quickly determining who is currently present

The system is designed using ESP32 and can be extended from a basic RFID attendance device into a complete IoT-based attendance and presence monitoring platform.

---

## 🎯 Objectives

- Provide secure RFID-based attendance authentication.
- Record both check-in and check-out events.
- Reduce false attendance caused by unauthorized absence.
- Continuously monitor indoor presence using Bluetooth Low Energy.
- Detect unexpected disappearance from the monitored area.
- Provide a real-time attendance monitoring interface.
- Support quick emergency headcount.
- Enable automated attendance data logging.

---

## ⚙️ Key Features

### 🔐 RFID Authentication

Users authenticate themselves using an RFID card or tag.

Each RFID UID is mapped to an authorized user.

**Authorized user:**
- Access is granted.
- User name is displayed on the OLED.
- Buzzer provides an entry confirmation beep.

**Unauthorized card:**
- Access is denied.
- OLED displays the denial message.
- Buzzer produces multiple warning beeps.

---

### 🔄 Check-In / Check-Out

The system maintains the current attendance state of registered users.

```text
RFID Scan
    ↓
Identify User
    ↓
Check Current Status
    ↓
 ┌───────────────┐
 │ Outside       │ → Check-In
 └───────────────┘

 ┌───────────────┐
 │ Inside        │ → Check-Out
 └───────────────┘
