# Falla de Conexión USB y Carga de Firmware - Paso a Paso

Si al intentar subir el código recibes el mensaje `Failed to connect to ESP32: No serial data received` o tu computadora no reconoce el puerto COM, sigue esta guía estructurada:

---

## Paso 1: Diagnóstico de Reconocimiento del Dispositivo

1. Conecta tu ESP32 al puerto USB de la computadora.
2. Abre el **Administrador de Dispositivos** de Windows (presiona Win + X y selecciona Administrador de Dispositivos).
3. Despliega la sección **Puertos (COM y LPT)**.
   - **Caso A (Correcto):** Aparece un dispositivo como *"Silicon Labs CP210x USB to UART Bridge (COMx)"* o *"CH340 (COMx)"* sin advertencias.
     - *Acción:* Pasa al Paso 3.
   - **Caso B (Signo de Advertencia Amarillo):** El puerto aparece con un triángulo amarillo.
     - *Causa:* Driver dañado o ausente.
     - *Acción:* Pasa al Paso 2.
   - **Caso C (No Aparece Nada):** El Administrador de Dispositivos no cambia al conectar y desconectar el cable.
     - *Acción:* Intenta con otro cable USB. El $80\%$ de los cables genéricos son solo de carga (no transmiten datos). Asegúrate de usar un cable de transferencia de datos de alta velocidad. 

---

## Paso 2: Instalación de Controladores (Drivers)

Descarga e instala el controlador correspondiente según el chip puente de tu placa:

- **Puente CP2102 (Común en placas oficiales):** [Drivers oficiales de Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
- **Puente CH340 / CH341 (Común en clones económicos):** [Drivers oficiales de WCH](http://www.wch-ic.com/downloads/CH341SER_EXE.html)

Una vez instalados, reinicia la computadora y comprueba de nuevo el Administrador de Dispositivos.

---

## Paso 3: Forzar el Modo de Programación en Placa (Flasheo Manual)

Si el puerto COM es reconocido pero la IDE de Arduino se queda en `Connecting........___` y luego falla:

1. Deja el cable USB conectado.
2. Mantén presionado el botón **BOOT** (o FLASH) físico en la placa del ESP32.
3. Haz clic en el botón de **Subir** de la IDE de Arduino.
4. Tan pronto como veas el mensaje `Connecting...` en la consola de la IDE, suelta el botón **BOOT**.
5. Si no se inicia la subida, utiliza un cable jumper externo en la protoboard para conectar directamente el pin **GPIO 0** a **GND** de forma permanente antes de iniciar la subida. Una vez que termine, remueve el cable para que pueda correr el firmware de forma normal.
