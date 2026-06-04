# Guía de Alimentación de Energía - Anura AI Monitor

Este documento describe la arquitectura de alimentación del **Anura AI Monitor**, detallando el uso del cargador MT3056/TP4056, la diferencia en el uso de los pines VIN y 3.3V del ESP32, y cómo medir el voltaje de la batería para telemetría.

---

## 1. Módulo de Carga MT3056 / TP4056

El módulo de carga sirve como puente seguro entre una fuente externa USB de 5V (cargador de celular, panel solar) y la batería de iones de litio (3.7V nominal, 4.2V a carga completa).

### Especificaciones Clave:
- **Voltaje de Entrada:** 4.5V - 5.5V (Puerto Micro-USB / USB-C o pines IN+/IN-).
- **Corriente de Carga:** Configurada por defecto a **1A** (mediante resistencia RPROG de $1.2\text{ k}\Omega$).
- **Voltaje de Corte:** $4.2\text{ V} \pm 1\%$.
- **Protección de Batería:** Corte por descarga profunda a menos de 2.5V y protección de sobrecorriente a 3A.

---

## 2. VIN (5V) vs 3.3V en el ESP32

El ESP32 funciona internamente a **3.3V**. La placa de desarrollo DevKit incluye un regulador de voltaje lineal LDO (usualmente el AP2112K o AMS1117) que reduce los voltajes de entrada a 3.3V estables.

```
                  +--------------------------------+
                  |    Regulador LDO del ESP32     |
[Entrada VIN] ---->   (Ej. AP2112K-3.3 o AMS1117)  ----> [Salida Interna 3.3V]
(4.5V - 6V)       +--------------------------------+     (Alimenta el chip ESP32 y sensores)
```

### Pin VIN (ó 5V):
- **Función:** Entrada de alimentación de voltaje no regulado (pero acotado).
- **Rango Seguro:** 4.5V a 6.0V.
- **Uso en nuestro proyecto:** Conectamos el terminal **OUT+** del módulo de carga (que entrega el voltaje de la batería de 3.7V - 4.2V) a este pin. Aunque la batería caiga por debajo de 5V, los reguladores modernos de bajo decaimiento (LDO Low-Dropout) del ESP32 pueden seguir regulando a 3.3V incluso con entradas de 3.5V.

### Pin 3.3V:
- **Función:** Salida de voltaje regulado cuando el ESP32 está alimentado por USB o VIN; o Entrada directa si se puentea el regulador.
- **Rango Estricto:** $3.0\text{ V}$ a $3.6\text{ V}$.
- **Uso en nuestro proyecto:** Alimentación del sensor BME280.
- **Peligro:** Nunca inyectes voltajes superiores a 3.6V en este pin o destruirás el ESP32 de forma instantánea.

---

## 3. Circuito de Monitoreo de Batería (Voltímetro de Telemetría)

Para que el ESP32 envíe el nivel de batería por telemetría a los microservicios de Anura, debemos leer el voltaje analógico de la batería. 
Como la batería puede llegar a **4.2V** y el ADC del ESP32 solo tolera un máximo de **3.3V**, es obligatorio implementar un **divisor de tensión**.

### Diagrama del Divisor de Voltaje:
```
 Batería (+) ───[ R1 = 100k Ohm ]───┬─── Pin GPIO 34 (ADC1_CH6 ESP32)
                                    │
                             [ R2 = 100k Ohm ]
                                    │
 Riel GND ────────────────────────────┴─── GND
```

### Cálculo de Voltaje:
El divisor con dos resistencias idénticas de $100\text{ k}\Omega$ divide el voltaje de entrada a la mitad ($V_{out} = V_{in} \cdot 0.5$).
- Si la batería está a **4.2V** (máxima carga), el pin GPIO 34 recibirá **2.1V** (dentro del rango seguro).
- Si la batería cae a **3.4V** (umbral crítico), el pin GPIO 34 recibirá **1.7V**.

### Código de Ejemplo para Lectura en Firmware:
```cpp
const int pinBateria = 34;
const float factorDivision = 2.0; // Restaura la mitad dividida
const float vReferencia = 3.3;    // Voltaje de referencia del ESP32

float leerVoltajeBateria() {
  int rawADC = analogRead(pinBateria);
  // Conversión del ADC (12 bits: 0 - 4095) a voltaje en el pin
  float vPin = (rawADC / 4095.0) * vReferencia;
  // Multiplicar por 2 para obtener el voltaje real de la batería
  return vPin * factorDivision;
}
```

---

## 4. Recomendaciones de Eficiencia Energética
1. **Uso de Deep Sleep (Sueño Profundo):** El ESP32 consume ~150-240mA transmitiendo por WiFi. Si se deja encendido continuamente, una batería 18650 durará menos de 15 horas. En modo Deep Sleep, el consumo baja a microamperios ($\approx 150 \mu\text{A}$ en placas de desarrollo comerciales debido al USB-UART y LDO).
2. **Ciclo de Trabajo:** Programar el dispositivo para despertar cada 10 minutos, encender sensores, leer datos, transmitir vía WiFi/MQTT en menos de 5 segundos, y volver a dormir inmediatamente. Esto eleva la vida útil de la batería a **meses**.
3. **Resistencias del Divisor:** Usar valores altos de resistencia ($100\text{ k}\Omega$ o superior) evita que el propio divisor drene corriente constantemente a tierra de forma innecesaria ($I_{drenaje} = 4.2\text{V} / 200\text{k}\Omega = 21 \mu\text{A}$).

---

## Enlaces de Navegación
| Documento | Propósito |
|-----------|-----------|
| [Manual_Montaje.md](Manual_Montaje.md) | Manual paso a paso de ensamble |
| [Esquematico_Conexiones.md](Esquematico_Conexiones.md) | Diagramas de conexiones eléctricas |
| [Lista_Componentes.md](Lista_Componentes.md) | Lista de materiales y BOM |
| [datasheets.md](datasheets.md) | Datasheets técnicos |

[⬅️ Volver al Menú Principal](../README.md)