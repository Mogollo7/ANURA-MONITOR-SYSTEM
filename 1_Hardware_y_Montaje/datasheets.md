# Datasheets de Sensores y Componentes - Anura AI Monitor

Esta carpeta reúne la documentación técnica directa y especificaciones de fábrica de los integrados y módulos clave del proyecto.

---

## 1. Sensor Bosch BME280
- **Documento:** Datasheet Oficial del Fabricante (Bosch Sensortec).
- **Parámetros Críticos:**
  - Voltaje de Alimentación ($V_{DD}$): $1.71\text{ V} - 3.6\text{ V}$.
  - Consumo en Reposo (Sleep Mode): $0.1\text{ }\mu\text{A}$.
  - Consumo en Medición (Humedad/Temperatura): $340\text{ }\mu\text{A}$.
  - Dirección I2C: `0x76` (SDO $\rightarrow$ GND) / `0x77` (SDO $\rightarrow$ VDD).
- **Referencia:** [Descargar Datasheet BME280 (PDF)](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf)

---

## 2. Microcontrolador Espressif ESP32-WROOM-32U
- **Documento:** Hoja de especificaciones de hardware (Espressif Systems).
- **Parámetros Críticos:**
  - Voltaje de alimentación del chip: $3.0\text{ V} - 3.6\text{ V}$ (Corriente mínima suministrada: $500\text{ mA}$).
  - Frecuencia de Reloj: 80MHz a 240MHz.
  - WiFi: 802.11 b/g/n (hasta 150 Mbps).
  - Bluetooth: v4.2 BR/EDR (Classic) y BLE.
- **Referencia:** [Descargar Datasheet ESP32-WROOM-32U (PDF)](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf)

---

## 3. Cargador de Baterías de Litio MT3056
- **Documento:** Hoja técnica del cargador lineal con protección integrada.
- **Parámetros Críticos:**
  - Voltaje de Entrada: 4.5V - 6V.
  - Corriente de Carga Máxima: 1A.
  - Voltaje de Corte de Descarga: 2.5V (Protección contra muerte de la celda de litio).
- **Referencia:** [Ver especificaciones del MT3056](https://www.alldatasheet.com/)

---

## Enlaces de Navegación
| Documento | Propósito |
|-----------|-----------|
| [Manual_Montaje.md](Manual_Montaje.md) | Manual paso a paso de ensamble |
| [Esquematico_Conexiones.md](Esquematico_Conexiones.md) | Diagramas de conexiones eléctricas |
| [Guia_Alimentacion_Energia.md](Guia_Alimentacion_Energia.md) | Gestión de energía y batería |
| [Lista_Componentes.md](Lista_Componentes.md) | Lista de materiales y BOM |

[⬅️ Volver al Menú Principal](../README.md)