# Diagnóstico de Errores de Alimentación - Anura AI Monitor

Este documento describe las fallas eléctricas típicas del microcontrolador y cómo prevenirlas.

---

## 1. El Error del Detector de Brownout (`Brownout detector was triggered`)

Si tu ESP32 se reinicia cíclicamente mostrando en consola la línea:
```
Brownout detector was triggered
ets Jun  8 2016 00:22:57
rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
```

Significa que el procesador ha detectado una caída de tensión por debajo de su umbral seguro de funcionamiento. Sigue estos pasos para solucionarlo:

---

## 2. Diagnóstico de la Fuente de Alimentación

### Paso 1: Medir el Voltaje en la Placa
1. **Medir el Voltaje en la Placa:** Coloca las puntas del multímetro en los pines **3V3 y GND** del ESP32 mientras el dispositivo intenta conectarse a WiFi.
2. **Observar la Caída:** Si el voltaje medido decae de $3.3\text{ V}$ a menos de $3.0\text{ V}$ en el instante de encendido del módem, la fuente de energía o el cableado no es capaz de proveer los $250\text{ mA}$ instantáneos que consume el radio de 2.4 GHz.
3. **Comprobar la Batería 18650:** Si alimentas por batería, mide su voltaje en bornes directos. Si está por debajo de $3.4\text{ V}$, cárgala de inmediato usando el módulo cargador MT3056/TP4056. Los reguladores LDO comunes no pueden mantener 3.3V de salida estables con menos de 3.4V de entrada.

---

## 3. Causas Comunes del Brownout en nuestro Circuito

### Causa A: Jumpers de Conexión muy Delgados (Resistencia del Cable)
- **Detalle:** Los jumpers típicos de protoboard son de aleaciones metálicas económicas (aluminio bañado en cobre) y calibre muy delgado (AWG 28 o 26). 
- **Efecto:** Durante la transmisión WiFi o inicialización del Bluetooth, el ESP32 consume ráfagas cortas de hasta **250mA**. La resistencia del cable delgado provoca una caída de voltaje ($V = I \cdot R$) instantánea. Si el voltaje de la batería está a 3.7V, la caída puede bajar el voltaje del pin VIN del ESP32 a 3.1V, haciendo que el regulador LDO decaiga por debajo del límite seguro de funcionamiento del ESP32.

### Causa B: Batería 18650 Descargada (Menos de 3.4V)
- **Detalle:** A medida que la batería 18650 se drena, su voltaje decae desde 4.2V hasta 3.0V.
- **Efecto:** Aunque el regulador LDO del ESP32 sea de tipo "Low-Dropout", requiere una diferencia de potencial mínima entre la entrada y la salida para estabilizar la línea de 3.3V. Si la batería decae por debajo de 3.4V, el regulador ya no es capaz de sostener los 3.3V estables ante los picos de consumo del microcontrolador, gatillando el detector de Brownout.

---

## 4. Soluciones de Ingeniería Aplicadas

### Paso 2: Solución Física en Protoboard (Condensador Búfer)

La forma más efectiva y estándar de mitigar los picos de consumo inductivo o resistivo es instalando un **condensador de desacoplamiento (bypass)**:

1. Desconecta la energía del circuito.
2. Inserta un **condensador electrolítico de entre $100\mu\text{F}$ y $220\mu\text{F}$** (con voltaje nominal de $6.3\text{ V}$ o superior) lo más cerca posible de los pines físicos del ESP32.
3. **Polaridad Obligatoria:**
   - Conecta la pata negativa (marcada con una franja blanca/negra en el cuerpo del cilindro) al pin **GND** del ESP32.
   - Conecta la pata positiva (pata más larga) al pin **3V3** del ESP32.
4. Conecta en paralelo un condensador cerámico pequeño de **$100\text{ nF}$** (código 104) para filtrar picos de alta frecuencia.
5. Vuelve a energizar el circuito y realiza una prueba de subida de datos. El condensador suministrará la carga acumulada evitando que el regulador caiga.

### Paso 3: Reemplazo de Jumper Cables
Si el condensador no soluciona el problema por completo:
- Los cables de protoboard delgados tienen una alta resistencia parásita. Reemplaza el cable de alimentación positivo (que va de la salida OUT+ del cargador al pin VIN del ESP32) y el de tierra (OUT- a GND) por cables de cobre de calibre grueso **AWG 22** o **AWG 20**.
- Limita la longitud de los cables de alimentación a menos de 10 cm para reducir la impedancia de la línea.
- Conéctate a un puerto USB de la PC que entregue corriente real (evita concentradores o Hubs USB genéricos sin alimentación externa).

### Solución 3: Control del Umbral de Brownout (Software)
*Aviso:* Aunque es posible deshabilitar el detector de Brownout por software usando comandos de bajo nivel de Espressif (`WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);`), **no se recomienda** ya que expone al sistema a bloqueos silenciosos o corrupción de los datos almacenados en LittleFS. Es mejor solucionar la causa de alimentación física.

### Solución 4: Protección en Cargador
El módulo de protección integrado en la placa del cargador MT3056/TP4056 desconectará automáticamente la carga (la batería) si el voltaje cae por debajo de los **2.5V**, protegiendo la vida útil de la celda de litio contra descargas profundas destructivas.

---

## Enlaces de Navegación
| Documento | Tema |
|-----------|-----------|
| [Registro_Fallos_Fisicos.md](Registro_Fallos_Fisicos.md) | Problemas de conectividad USB y flasheo manual |
| [Solucion_Sensores.md](Solucion_Sensores.md) | Problemas con el bus I2C y el sensor BME280 |
| [Solucion_Errores_Bluetooth.md](Solucion_Errores_Bluetooth.md) | Fallos de emparejamiento Bluetooth (PIN 1234) |
| [Solucion_Errores_WiFi.md](Solucion_Errores_WiFi.md) | Desconexiones WiFi y lógica de timeout |
| [Bugs_Firmware.md](Bugs_Firmware.md) | Errores de compilación comunes |

[⬅️ Volver al Menú Principal](../README.md)