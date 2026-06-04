# Solución de Errores de WiFi - Anura AI Monitor

Esta guía técnica explica cómo diagnosticar y solucionar desconexiones y fallas en la transmisión WiFi.

---

## 1. El Problema: Bloqueo del Dispositivo por Falta de Conexión

El código clásico de conexión WiFi en Arduino suele bloquear el hilo de ejecución principal usando bucles infinitos:
```cpp
while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Bloqueo indefinido si el Router está apagado
}
```
Si el nodo sensor está desplegado en la selva o invernadero y la red WiFi central se apaga (debido a lluvias, fallos eléctricos o mantenimiento), el ESP32 se congela en este ciclo, manteniendo el chip de radio encendido a máxima potencia y consumiendo 180mA continuamente, lo que descarga la batería 18650 por completo en unas pocas horas.

---

## 2. Solución: Timeout de Conexión y Modo Offline

Implementamos un sistema de control de tiempo (timeout) en la conexión. Si el router no responde dentro de un lapso predeterminado (por ejemplo, 8 segundos), el ESP32 aborta la conexión, deshabilita el módem WiFi y pasa a registrar los datos en el sistema de archivos interno **LittleFS**.

### Código de Conexión Robusto (C++):
```cpp
bool conectarConTimeout(const char* ssid, const char* pass, int maxIntentos = 15) {
    WiFi.begin(ssid, pass);
    int intentos = 0;
    
    while (WiFi.status() != WL_CONNECTED && intentos < maxIntentos) {
        delay(500);
        intentos++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        return true; // Conexión Exitosa
    } else {
        // Apagar WiFi para evitar consumo excesivo de corriente
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        return false; // Fallo de Conexión (Modo Offline Activado)
    }
}
```

---

## 3. Diagnóstico Paso a Paso de Desconexiones WiFi

Si el ESP32 logra conectarse pero pierde la señal aleatoriamente o falla al intentar conectarse, realiza los siguientes pasos de diagnóstico:

### 3.1 Comprobar Credenciales y SSID
1. Abre tu código fuente o archivo de cabecera y verifica que los campos `WIFI_SSID` y `WIFI_PASSWORD` coincidan exactamente con la configuración de tu router (respetando mayúsculas, minúsculas, espacios y caracteres especiales).
2. **Importante:** El ESP32 solo soporta redes inalámbricas en la banda de **2.4 GHz** (protocolo 802.11 b/g/n). Si intentas conectarlo a una red de **5 GHz** (típica en routers modernos con bandas híbridas o móviles de última generación), el ESP32 fallará en el escaneo y nunca se asociará. Asegúrate de configurar una banda dedicada a 2.4 GHz en tu router.

### 3.2 Tabla de Niveles de Señal RSSI (dBm)

Conecta el ESP32 a la PC, abre la consola serie a 115200 baudios y lee el valor de señal WiFi imprimiendo la función `WiFi.RSSI()` en tu ciclo.

| Rango de RSSI (dBm) | Calidad de Señal | Comportamiento Esperado |
| :--- | :--- | :--- |
| **$-30\text{ a }-60\text{ dBm}$** | Excelente / Buena | Conexión instantánea, estable y sin pérdida de paquetes. |
| **$-70\text{ a }-80\text{ dBm}$** | Regular / Aceptable | Conexión posible pero con latencias elevadas en DHCP. |
| **$<-85\text{ dBm}$** | Mala / Crítica | Desconexiones continuas y fallos al enviar datos. |

- **Solución:** Acerca el nodo al punto de acceso, use repetidores o cambie a un módulo **ESP32-WROOM-32U** conectando una antena dipolo externa para elevar la ganancia de recepción en áreas de follaje denso o alta humedad.

### 3.3 Optimización contra Bloqueos en Firmware
Para evitar que el nodo agote su batería al intentar conectarse a una red fuera de rango:
1. Reemplaza el bucle bloqueante `while (WiFi.status() != WL_CONNECTED)` por una rutina con contador de reintentos límite (máximo 15 intentos).
2. Si el contador se agota, apaga el transceptor de radio para evitar fugas de corriente:
   ```cpp
   WiFi.disconnect(true);
   WiFi.mode(WIFI_OFF);
   ```
3. Deleja el almacenamiento de la medición a la memoria Flash LittleFS e inicia el ciclo de Deep Sleep para volver a intentar la conexión en el próximo reinicio, preservando la energía del nodo.

### 3.4 Causa C: Modo WiFi Power Save (Ahorro de Energía)
- **Detalle:** Por defecto, el framework de Espressif activa un modo de ahorro de energía en la radio WiFi que puede causar microcortes y desconexiones con ciertos routers.
- **Solución:** Deshabilite la suspensión del transceptor en la inicialización (dentro del `setup()` después de llamar a `WiFi.begin()`):
  ```cpp
  WiFi.setSleep(WIFI_PS_NONE);
  ```

---

## Enlaces de Navegación
| Documento | Tema |
|-----------|-----------|
| [Registro_Fallos_Fisicos.md](Registro_Fallos_Fisicos.md) | Problemas de conectividad USB y flasheo manual |
| [Solucion_Sensores.md](Solucion_Sensores.md) | Problemas con el bus I2C y el sensor BME280 |
| [Solucion_Errores_Bluetooth.md](Solucion_Errores_Bluetooth.md) | Fallos de emparejamiento Bluetooth (PIN 1234) |
| [Errores_Alimentacion.md](Errores_Alimentacion.md) | Reinicios por caídas de voltaje (Brownouts) |
| [Bugs_Firmware.md](Bugs_Firmware.md) | Errores de compilación comunes |

[⬅️ Volver al Menú Principal](../README.md)