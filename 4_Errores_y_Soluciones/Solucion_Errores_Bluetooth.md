# Solución de Errores de Bluetooth - Anura AI Monitor

Este documento detalla los problemas de sincronización de Bluetooth Classic (SSP) con dispositivos móviles y cómo solucionarlos a nivel de firmware.

---

## 1. Problema: Error de Emparejamiento / Falta de PIN

Los sistemas operativos modernos (particularmente Android 10+ e iOS) han endurecido las directivas de seguridad para conexiones Bluetooth. A menudo, al emparejar un ESP32 con la librería `BluetoothSerial.h`, el teléfono móvil no muestra el diálogo de confirmación de PIN o rechaza la conexión de forma inmediata catalogándola como insegura, debido a que el ESP32 no solicita autenticación explícita.

### 1.1 Solución: Forzar SSP (Simple Secure Pairing) con PIN de Bypass

Para solucionar esto, se configura un PIN estático de 4 dígitos en el ESP32 antes de inicializar la transmisión serie. Esto fuerza a los dispositivos móviles a mostrar el diálogo de emparejamiento solicitando el código.

#### Configuración en Código (C++):

```cpp
#include "BluetoothSerial.h"

BluetoothSerial ESP32_BT;

void setup() {
  Serial.begin(115200);

  // Iniciar Bluetooth con nombre del dispositivo
  ESP32_BT.begin("Anura_Monitor");

  // PIN de emparejamiento (opcional)
  ESP32_BT.setPin("1234", 4);

  Serial.println("Bluetooth iniciado correctamente");
  Serial.println("Dispositivo listo para emparejar: Anura_Monitor");

  // Mensaje inicial al conectarse
  ESP32_BT.println("Conexion Bluetooth exitosa con ESP32 ✔");
}

void loop() {
  // Mensaje de vida cada 5 segundos
  ESP32_BT.println("ESP32 activo y conectado ✔");
  Serial.println("Enviando señal de vida por Bluetooth");

  delay(5000);
}
```

### 1.2 Verificación desde la Aplicación Móvil

Para visualizar la conexión y el envío de datos, utilice la aplicación móvil **Serial Bluetooth** (compatible tanto para Android como para iOS).

Siga estos pasos:

1. Ingrese a la sección **"Device"** dentro de la aplicación.
2. Seleccione el nombre con el cual configuró el dispositivo Bluetooth (en el ejemplo: `Anura_Monitor`).
3. Se abrirá la terminal mostrando toda la información de los datos enviados en tiempo real.

De esta forma podrá confirmar que el módulo Bluetooth de su ESP32 funciona correctamente.

---

## 2. Problema: El Dispositivo No Es Visible (Discovery Fail)

Si el ESP32 no aparece en la búsqueda Bluetooth del celular, existen dos causas principales:

### 2.1 Causa A: Memoria Saturada (WiFi y Bluetooth Concurrentes)

El módulo Bluetooth Classic consume aproximadamente **100 KB de RAM**. Si hay fugas de memoria o se inicializó el WiFi en paralelo, la pila Bluetooth falla silenciosamente al inicializarse y el dispositivo no será visible.

- **Solución:** No inicialice WiFi y Bluetooth de forma concurrente. Apague el WiFi antes de llamar a `SerialBT.begin()`:
  ```cpp
  WiFi.mode(WIFI_OFF);
  SerialBT.begin("Anura_Monitor");
  ```

### 2.2 Causa B: Antena Inestable por Caída de Voltaje

El ESP32 requiere picos de corriente elevados al anunciar el servicio Bluetooth. Si el voltaje de alimentación baja de **3.6V**, el chip apaga el transmisor RF y el dispositivo deja de ser visible.

- **Solución:** Añada un condensador de desacoplamiento de $100\mu\text{F}$ o $220\mu\text{F}$ en paralelo con los pines **3.3V** y **GND**, colocado lo más cerca posible del ESP32 para actuar como búfer de corriente durante los picos de transmisión.

---

## Enlaces de Navegación
| Documento | Propósito |
|-----------|-----------|
| [Registro_Fallos_Fisicos.md](Registro_Fallos_Fisicos.md) | Problemas de conectividad USB y flasheo manual |
| [Solucion_Sensores.md](Solucion_Sensores.md) | Problemas con el bus I2C y el sensor BME280 |
| [Solucion_Errores_WiFi.md](Solucion_Errores_WiFi.md) | Fallos de conectividad WiFi |
| [Errores_Alimentacion.md](Errores_Alimentacion.md) | Reinicios por caídas de voltaje |
| [Bugs_Firmware.md](Bugs_Firmware.md) | Errores de compilación comunes |

[⬅️ Volver al Menú Principal](../README.md)