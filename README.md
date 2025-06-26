
# Integrare ESP32 + Python MQTT Bridge – Ghid complet pentru proiectul de Licență

Acest proiect pune la dispoziție o soluție IoT completă pentru monitorizarea condițiilor de mediu, alcătuită din două componente fundamentale care comunică între ele folosind protocolul MQTT securizat.

## 🧠 Cum funcționează sistemul, pe scurt

- **ESP32_CODE:** O placă ESP32 echipată cu senzori (de exemplu BME280, MQ2, senzor de flacără) colectează date despre mediul înconjurător (temperatură, umiditate, presiune, nivel gaze, detecție foc). La intervale regulate, aceste date sunt publicate prin MQTT (pe topicuri separate pentru fiecare măsurătoare) către un broker MQTT securizat. Fiecare placă ESP32 poate avea un identificator propriu, pentru a permite diferențierea dispozitivelor în rețea.

- **BRIDGE_CODE:** Un script Python care funcționează ca un "gateway software". Se conectează la același broker MQTT și ascultă topicurile publicate de ESP32-uri. Orice mesaj nou trimis de senzori este recepționat instant, iar datele pot fi afișate în consolă, salvate, sau procesate suplimentar (de exemplu trimitere alerte, integrare cu baze de date sau dashboard-uri). Toate datele de conectare și configurare sunt gestionate ușor, printr-un fișier `.env`.

**Integrarea acestor două componente permite colectarea, centralizarea și procesarea automată a informațiilor din mediul real, fiind baza unui sistem IoT scalabil, sigur și modern.**

---

## 1️⃣ Configurare și utilizare ESP32_CODE (cu PlatformIO)

**ESP32_CODE** este firmware-ul care trebuie încărcat pe placa ESP32. Acesta va citi datele de la senzori și le va transmite către brokerul MQTT.

### Pași de urmat:

1. **Pregătește mediul de dezvoltare:**
   - Instalează [Visual Studio Code](https://code.visualstudio.com/) și extensia [PlatformIO IDE](https://platformio.org/install/ide?install=vscode).
   - Descarcă folderul `ESP32_CODE` din proiect.

2. **Deschide proiectul ESP32 în PlatformIO:**
   - Deschide VSCode și selectează `Open Folder...`, apoi alege `ESP32_CODE`.

3. **Configurează datele de conectare:**
   - În fișierul `main.cpp`, setează:
     - Numele și parola rețelei WiFi la care va fi conectat ESP32-ul.
     - Adresa, portul și credentialele pentru brokerul MQTT.
     - Un identificator unic pentru acest ESP32 (folosit în topicuri, pentru a-l diferenția de alte dispozitive din rețea).
   - Dacă este nevoie, modifică viteza monitorului serial (baudrate) pentru debugging.

4. **Instalează dependențele:**
   - Asigură-te că în fișierul `platformio.ini` există toate librăriile necesare pentru senzori și MQTT.

5. **Conectează placa ESP32 la calculator prin USB.**

6. **Compilează și încarcă firmware-ul:**
   - Folosește butonul “Upload” din PlatformIO, sau comanda corespunzătoare în terminal.
   - Poți monitoriza funcționarea și mesajele de debug cu “Serial Monitor” din PlatformIO.

7. **Rezultatul:**  
   - ESP32 va începe să trimită automat datele colectate către brokerul MQTT, pe topicuri dedicate (de exemplu: `livingroom_1/temperature`, `livingroom_1/gas/alarm` etc).

---

## 2️⃣ Configurare și utilizare BRIDGE_CODE (Python MQTT Bridge)

**BRIDGE_CODE** permite colectarea și vizualizarea datelor trimise de oricare (sau toate) ESP32-urile din rețea, folosind un script Python modern.

### Pași de urmat:

1. **Deschide un terminal/command prompt în folderul `BRIDGE_CODE` din proiect.**

2. **Creează și activează un mediu virtual Python:**
   - Pentru Linux/Mac:
     ```
     python3 -m venv venv
     source venv/bin/activate
     ```
   - Pentru Windows:
     ```
     python -m venv venv
     venv\Scriptsctivate
     ```

3. **Instalează toate dependențele necesare:**
   - Execută:
     ```
     pip install -r requirements.txt
     ```
   - Acest fișier conține, de exemplu, `paho-mqtt` (librărie MQTT) și `python-dotenv` (pentru gestionarea variabilelor din .env).

4. **Configurează datele de acces la broker:**
   - În fișierul `.env` din același folder, completează:
     - adresa brokerului MQTT (aceeași folosită și de ESP32)
     - portul, numele de utilizator și parola (dacă brokerul este securizat)
   - Astfel, scriptul va folosi automat aceste date la fiecare pornire.

5. **Pornește scriptul de bridge:**
   - Rulează:
     ```
     python mqtt_subscriber.py
     ```
   - În consolă vei vedea în timp real toate datele transmise de ESP32, pe topicurile configurate.

### Ce face concret bridge-ul Python?

- Se conectează la brokerul MQTT folosind datele din `.env`.
- Se abonează la topicurile relevante pentru unul sau mai multe ESP32-uri.
- Afișează instant orice mesaj nou primit (de la orice dispozitiv), fiind ușor de extins pentru a salva datele, a trimite alerte sau a integra cu alte aplicații.

---

## 🧩 Concluzie

Prin aceste două componente:
- Obții o rețea de senzori automatizați (ESP32) ce transmit datele, în siguranță, către infrastructura centrală MQTT.
- Ai un “gateway software” (Python bridge) pentru a colecta, vizualiza și prelucra aceste date – ușor de extins pentru orice aplicație de tip smart home, industrie, educație sau cercetare.

Dacă ai nevoie de clarificări suplimentare, de exemple de extindere, sau de integrare cu baze de date/grafice/alte sisteme – nu ezita să ceri!
