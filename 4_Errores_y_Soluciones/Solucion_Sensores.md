# Solución de Problemas del Sensor BME280 - Anura AI Monitor

Esta guía detalla los errores comunes asociados al bus I2C y al sensor BME280, y cómo resolverlos.

> **Nota importante:** Para este proyecto, asegúrese de haber conseguido el **BME280**, ya que existe una versión más económica llamada **BMP280** que no posee capacidad de medición de humedad. En caso de no necesitar medir dicha variable, puede utilizar el BMP280 sin inconvenientes con el mismo código.

---

## 1. Código de Verificación: Conexión BME280 + Bluetooth

El siguiente código permite confirmar la correcta detección del sensor BME280 o BMP280 por parte del ESP32, y envía las lecturas en tiempo real a través de Bluetooth Serial para su visualización desde un dispositivo móvil.

```cpp
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth no está habilitado
#endif

Adafruit_BME280 bme;
BluetoothSerial ESP32_BT;

void setup() {

  Serial.begin(115200);

  // Inicialización del Bluetooth
  ESP32_BT.begin("Anura_Monitor_BME280");

  // Configuración del PIN Bluetooth
  ESP32_BT.setPin("1234", 4);

  Serial.println("Bluetooth iniciado correctamente");
  Serial.println("PIN Bluetooth: 1234");

  // Inicialización I2C
  // SDA = GPIO 21
  // SCL = GPIO 22
  Wire.begin(21, 22);

  // Intento de conexión con dirección 0x76
  bool estado = bme.begin(0x76);

  // Si falla, intenta con 0x77
  if (!estado) {
    estado = bme.begin(0x77);
  }

  // Verificación del sensor
  if (!estado) {
    Serial.println("No se encontró el sensor BME280/BMP280");
    ESP32_BT.println("ERROR: Sensor BME280/BMP280 no detectado");
    while (1);
  }

  Serial.println("Sensor BME280/BMP280 detectado correctamente");
  ESP32_BT.println("Sensor BME280/BMP280 conectado correctamente");
}

void loop() {

  float temperatura = bme.readTemperature();
  float humedad = bme.readHumidity();

  // Envío de datos por Bluetooth
  ESP32_BT.print("Temperatura: ");
  ESP32_BT.print(temperatura);
  ESP32_BT.println(" °C");

  ESP32_BT.print("Humedad: ");
  ESP32_BT.print(humedad);
  ESP32_BT.println(" %");

  ESP32_BT.println("------------------------");

  // Mostrar también en Monitor Serial
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");

  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.println(" %");

  Serial.println("------------------------");

  delay(3000);
}
```

---

## 2. Problema: El Sensor No Es Detectado

**Error en consola:** `Could not find a valid BME280 sensor!`

El bus I2C utiliza dos cables principales (SDA y SCL) compartidos. Si el firmware está programado para buscar al sensor en una dirección específica y este responde en otra, la inicialización de la librería de Adafruit fallará por completo.

### 2.1 Causa: Dirección I2C Incorrecta

El BME280 tiene una patilla física de selección de dirección llamada **SDO** que determina en cuál de las dos direcciones posibles responderá el sensor:

- Si **SDO** está conectado a **GND** → la dirección I2C es **`0x76`**.
- Si **SDO** está conectado a **3V3 (VCC)** → la dirección I2C es **`0x77`**.

Muchas placas de desarrollo genéricas económicas vienen configuradas de fábrica con la dirección `0x77` en lugar de la común `0x76`, lo que genera el fallo de inicialización si el código solo busca en `0x76`.

### 2.2 Solución: Inicialización Dinámica en Cascada

Para hacer el código tolerante a variaciones del fabricante del sensor, se implementa una inicialización en cascada que prueba ambas direcciones automáticamente:

```cpp
bool iniciarSensor() {
    // Intentar con dirección estándar 0x76
    if (bme.begin(0x76)) {
        Serial.println("Sensor BME280 inicializado en 0x76");
        return true;
    }

    // Si falla, probar con dirección secundaria 0x77
    if (bme.begin(0x77)) {
        Serial.println("Sensor BME280 inicializado en 0x77");
        return true;
    }

    Serial.println("Error grave: Sensor BME280 no responde en 0x76 ni 0x77");
    return false;
}
```

### 2.3 Escaneo Físico del Bus I2C (Herramienta de Diagnóstico)

Si el sensor sigue sin ser detectado tras revisar las direcciones, puedes flashear el siguiente código de utilidad (**I2C Scanner**) en tu ESP32. Esto te indicará si hay algún integrado conectado físicamente al bus I2C y en qué dirección responde:

#### Paso 1: Escaneo Físico del Bus I2C
1. Sube el código de utilidad ubicado en [Firmware_ESP32.ino](../../2_Software_y_Codigo/Firmware_ESP32/Firmware_ESP32.ino) o usa el escáner I2C integrado.
2. Abre el Monitor Serie a **115200** baudios.
3. Analiza el reporte en consola:
   - **Caso A (No se detecta dispositivo):** La consola imprime `No se detectó ningún dispositivo I2C conectado.`
     - *Causa:* Mal cableado, soldaduras frías o falta de alimentación en el sensor. Pasa al paso 2.
   - **Caso B (Dispositivo detectado en 0x76 o 0x77):**
     - *Causa:* El hardware funciona correctamente, pero hay una discrepancia en la dirección lógica del software. Ajusta tu código para usar esa dirección específica.

#### Paso 2: Validación Eléctrica y Cableado
1. **Verificar Alimentación:** Mide con el multímetro el voltaje entre el pin VCC y GND del sensor BME280. Debe estar en un rango de $3.0\text{ V} - 3.4\text{ V}$.
2. **Revisar Pines I2C:** Asegúrates de que no cruses las líneas:
   - El pin **SDA** del sensor debe ir al pin **GPIO 21** del ESP32.
   - El pin **SCL** del sensor debe ir al pin **GPIO 22** del ESP32.
3. **Comprobar Soldadura:** Asegúrates de que la tira de pines macho esté correctamente soldada al BME280. El contacto a presión por gravedad en la protoboard suele fallar por vibraciones leves.

```cpp
#include <Wire.h>

void setup() {
  Wire.begin(21, 22); // Inicializa I2C con SDA = GPIO 21, SCL = GPIO 22
  Serial.begin(115200);
  while (!Serial); // Espera a que se abra la consola serie
  Serial.println("\n--- Escáner I2C Iniciado ---");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Escaneando bus I2C...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Dispositivo I2C encontrado en la dirección 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Error desconocido en la dirección 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) {
    Serial.println("No se detectó ningún dispositivo I2C conectado.\n");
  } else {
    Serial.println("Escaneo de bus I2C finalizado.\n");
  }

  delay(5000); // Repetir cada 5 segundos
}
```

Una vez cargado, abre el Monitor Serie a **115200** baudios:
- **Caso A (No se detecta dispositivo):** Si imprime `No se detectó ningún dispositivo I2C conectado.`, tienes un problema eléctrico o de cableado (revisa el paso a paso de alimentación y soldaduras).
- **Caso B (Dispositivo detectado en 0x76 o 0x77):** El hardware funciona correctamente, pero debes ajustar tu software para que use esa dirección específica (o emplear la rutina de inicialización dinámica).

---

## 3. Problema: Valores Congelados o Erráticos

**Síntoma:** La humedad se queda fija en `0%` o la temperatura siempre marca el mismo valor (ej. `22.4°C`) sin variar, a pesar de que el sensor inicia sin errores.

Si el sensor no devuelve errores durante el arranque pero los datos nunca varían o son absurdos, existen tres causas posibles:

### 3.1 Causa A: Cortocircuito por Condensación de Agua

La condensación directa de gotas de agua sobre el elemento sensor interrumpe la medición capacitiva de la humedad relativa.

- **Solución:** Reubicar el sensor en posición invertida (boca abajo) dentro de la carcasa para evitar que el rocío o la lluvia se acumulen dentro del encapsulado metálico del BME280.

### 3.2 Causa B: Bus I2C Bloqueado por Ruido Electromagnético

En ocasiones, el ruido electromagnético en las líneas de datos puede dejar bloqueado el bus I2C, congelando las lecturas en el último valor registrado.

- **Solución:** Implementar un reset del bus en el código, liberando los pines y reinicializándolos cuando se detectan 3 lecturas idénticas consecutivas:
  ```cpp
  Wire.end();
  delay(50);
  Wire.begin(21, 22);
  ```

### 3.3 Causa C: Alimentación Ruidosa o Inestable

El sensor BME280 es sumamente sensible al ruido eléctrico presente en la línea de alimentación, lo que puede provocar lecturas erróneas o congeladas.

- **Solución:** Tome los 3.3V directamente de la salida del regulador del ESP32 y no del pin de entrada de la batería. Coloque un condensador cerámico de $100\text{ nF}$ entre las patas **VCC** y **GND** del sensor BME280, ubicado lo más cerca posible del chip, para desacoplar el ruido de alta frecuencia.

---

## Enlaces de Navegación
| Documento | Tema |
|-----------|-----------|
| [Registro_Fallos_Fisicos.md](Registro_Fallos_Fisicos.md) | Problemas de conectividad USB y flasheo manual |
| [Solucion_Errores_Bluetooth.md](Solucion_Errores_Bluetooth.md) | Fallos de emparejamiento Bluetooth (PIN 1234) |
| [Solucion_Errores_WiFi.md](Solucion_Errores_WiFi.md) | Desconexiones WiFi y lógica de timeout |
| [Errores_Alimentacion.md](Errores_Alimentacion.md) | Reinicios por caídas de voltaje (Brownouts) |
| [Bugs_Firmware.md](Bugs_Firmware.md) | Errores de compilación comunes |

[⬅️ Volver al Menú Principal](../README.md)