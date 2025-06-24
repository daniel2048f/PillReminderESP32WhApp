# Proyecto: Recordatorio de Pastillas con ESP32 y WhatsApp

Este proyecto emplea una placa ESP32 para enviar recordatorios diarios mediante WhatsApp sobre la toma de medicación, en horarios específicos según el día de la semana. Además, enciende un LED como señal visual para reforzar la alerta. El sistema se sincroniza automáticamente con servidores de hora en Internet y utiliza el servicio gratuito CallMeBot para el envío de los mensajes.

---

## Tabla de Contenidos

- Descripción General
- Características Principales
- Componentes y Hardware
- Esquema de Conexiones
- Funcionamiento del Sistema
- Configuración de CallMeBot
- Instrucciones de Uso
- Posibles Mejoras Futuras
- Créditos y Licencia

---

## Descripción General

El sistema permite programar alertas diarias para recordar la toma de medicamentos mediante un LED y un mensaje de WhatsApp. Está diseñado para ejecutarse de forma autónoma: se conecta a una red WiFi, obtiene la hora desde Internet (servidores NTP) y revisa una vez por segundo si debe activar el LED correspondiente y enviar el mensaje.

---

## Características Principales

- Envío automático de mensajes por WhatsApp a través de CallMeBot.
- Sincronización de hora con servidores NTP.
- Encendido de un LED específico según el día de la semana.
- Sistema autónomo y reutilizable.
- Bajo consumo de energía en reposo.
- Totalmente configurable por software.

---

## Componentes y Hardware

| Componente              | Cantidad |
| ----------------------- | -------- |
| ESP32 DevKit            | 1        |
| LED rojo                | 7        |
| Resistencias 390 Ω      | 7        |
| Cables de conexión      | varios   |
| Fuente de 5V para ESP32 | 1        |

---

## Esquema de Conexiones (Figura N°1)

Cada LED está conectado a un pin digital de la ESP32 a través de una resistencia de 390 ohmios para limitar la corriente. La correspondencia entre pines y días es:

| Día       | Pin ESP32 |
| --------- | --------- |
| Domingo   | GPIO 21   |
| Lunes     | GPIO 2    |
| Martes    | GPIO 15   |
| Miércoles | GPIO 4    |
| Jueves    | GPIO 5    |
| Viernes   | GPIO 18   |
| Sábado    | GPIO 19   |

---

## Funcionamiento del Sistema

1. La ESP32 se conecta automáticamente a la red WiFi configurada.
2. Luego sincroniza su reloj interno con servidores NTP.
3. Cada segundo, la función `revisarHoraYEncenderLED()` compara la hora actual con los horarios de encendido y apagado configurados para cada día.
4. Si coincide con la hora de encendido:
   - Se enciende el LED correspondiente al día.
   - Se envía un mensaje de WhatsApp con un recordatorio.
5. Un minuto después, el LED se apaga y se reinicia la lógica para el día siguiente.

---

## Configuración de CallMeBot

Para que CallMeBot funcione, sigue estos pasos:

1. Agrega a tu WhatsApp el número: `+34 684 72 80 23`.
2. Envíale el mensaje: `I allow callmebot to send me messages`.
3. Recibirás una clave (`apiKey`) única.
4. En el código:
   - En la línea `String phoneNumber = "+57..."`, escribe tu número (con código del país).
   - En la línea `String apiKey = "..."`, pega tu clave recibida.

---

## Requisitos Previos

- Asegúrate de instalar la librería **Callmebot-ESP32-master** en tu entorno de desarrollo Arduino. Puedes descargarla desde el repositorio del proyecto o añadirla manualmente a través del Gestor de Bibliotecas.

## Instrucciones de Uso

1. Configura el archivo `.ino` con tu red WiFi, número de teléfono y `apiKey`.
2. Carga el programa a tu ESP32 desde el IDE de Arduino.
3. Conecta los LEDs como se indica.
4. Alimenta la ESP32.
5. El sistema se conectará automáticamente, sincronizará la hora y comenzará a operar.
6. En los horarios definidos, encenderá el LED correspondiente al día y enviará el mensaje de WhatsApp.

---

## Posibles Mejoras Futuras

- Integración con pantalla LCD para mostrar información en tiempo real.
- Configuración remota vía servidor web o app móvil.
- Programación dinámica de horarios desde una interfaz web.
- Almacenamiento de logs en tarjeta SD.
- Soporte para múltiples recordatorios por día.

---

## Créditos y Licencia

Proyecto desarrollado por Daniel Cangrejo, 2025.  
Este proyecto puede ser usado, modificado y redistribuido libremente, siempre que se mantenga la atribución al autor original.
