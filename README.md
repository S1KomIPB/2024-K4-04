# Capstone ESP monitoring api

### Struktur Program
- `./esp32-code/` -> kode yang dijalankan di ESP32 untuk connect ke server
- `./main.py` -> kode yang dijalankan server

### Instalasi (linux)
```
python -m venv venv
source venv/bin/activate
pip install -r requirements.txt

```

### Menjalankan program
**Server**
- jalankan `uvicorn main:app --host 0.0.0.0 --reload`
- catatan: `--host 0.0.0.0` menerima koneksi dari IP mana saja (tidak aman)

**ESP32**
- buka kode `esp32-code/esp32-main/esp32-main.ino` di Arduino IDE
- sesuaikan SSID-Password Wi-Fi pada kode tersebut (network/WiFi server harus sama dengan ESP32)
- sesuaikan IP server pada kode tersebut dengan memeriksa `ifconfig`
- jalankan kode tersebut
