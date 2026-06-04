# Errores Comunes de Compilación y Firmware - Anura AI Monitor

Este documento contiene un glosario de los errores de software y compilación más habituales al trabajar con este firmware en la IDE de Arduino o PlatformIO.

---

## 1. Librerías Faltantes (`No such file or directory`)

### Error:
```
Firmware_ESP32:12:30: fatal error: Adafruit_BME280.h: No such file or directory
#include <Adafruit_BME280.h>
                              ^
compilation terminated.
```
* **Causa:** La IDE de Arduino no encuentra la librería externa instalada en su biblioteca local.
* **Solución:**
  1. En la IDE de Arduino, dirígete a: **Herramientas** $\rightarrow$ **Administrar bibliotecas...** (o presiona Ctrl+Shift+I).
  2. En el buscador escribe **"Adafruit BME280"**.
  3. Presiona **Instalar**. Instala también todas sus dependencias asociadas (como *Adafruit Unified Sensor* y *Adafruit BusIO*).

---

## 2. Definición no Válida de Bluetooth (`'BluetoothSerial' does not name a type`)

### Error:
```
Transmision_Datos:8:5: error: 'BluetoothSerial' does not name a type
     BluetoothSerial serialBT;
     ^
```
* **Causa A (Placa Incorrecta seleccionada):** Estás compilando el firmware seleccionando una tarjeta Arduino convencional (como Arduino Uno / Nano) que no tiene soporte para Bluetooth Classic.
  - *Solución:* Asegúrate de tener seleccionado **ESP32 Dev Module** (o tu placa ESP32 equivalente) bajo el menú **Herramientas** $\rightarrow$ **Placa**.
* **Causa B (Soporte Deshabilitado en ESP32-S2/S3/C3):** Ciertos microcontroladores de la familia ESP32 (como el ESP32-S2 o ESP32-S3) solo incorporan Bluetooth BLE (bajo consumo) pero **no** tienen Bluetooth Classic. La librería `BluetoothSerial.h` es exclusiva de los chips ESP32 clásicos (ESP32-WROOM-32).
  - *Solución:* Valida que estés compilando para un chip ESP32 nativo de primera generación. Si migras a un chip moderno S3/C3, deberás cambiar la librería a una solución de BLE.

---

## 3. Conflicto de Puertos Seriales (`Serial port not found / Permission denied`)

### Error:
```
An error occurred while uploading the sketch
esptool.py v4.5.1
Serial port COM5
Traceback (most recent call last):
serial.serialutil.SerialException: could not open port 'COM5': PermissionError(13, 'Access is denied.', None, 5)
```
* **Causa A (Puerto COM en Uso):** Tienes abierto el Monitor Serie de otra aplicación (como Putty o Serial Bluetooth Terminal en modo bypass) en la misma computadora que está bloqueando el puerto COM5.
  - *Solución:* Cierra cualquier monitor serie o terminal conectada antes de presionar el botón "Subir".
* **Causa B (Drivers no Instalados):** Si el cable USB-Serial de tu placa utiliza el chip genérico CH340 y no tienes instalado el driver oficial de WCH.
  - *Solución:* Descarga e instala los drivers oficiales "CH341SER.EXE" desde la página oficial del fabricante.
