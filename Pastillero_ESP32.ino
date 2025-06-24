// Estas líneas incluyen bibliotecas necesarias:
// 1. WiFi.h → para conectar la ESP32 a una red WiFi.
// 2. time.h → para obtener la hora actual desde internet (hora mundial real).
// 3. Callmebot_ESP32.h → para enviar mensajes de WhatsApp a través de un servicio llamado CallMeBot.
#include <WiFi.h>
#include <time.h>
#include <Callmebot_ESP32.h>

// En ssid y password entre las comillas escribes el nombre de la red WiFi y la contraseña para que la ESP32 se conecte a internet
const char* ssid = " ";
const char* password = " ";

// En phoneNumber pones el número de teléfono al que se enviará el mensaje por WhatsApp (con código del país)
String phoneNumber = "+57 ";

// Clave de autenticación que CallMeBot requiere para enviar mensajes a ese número
String apiKey = " ";
//
   
// Lista de pines (entradas/salidas físicas en la ESP32) donde hay conectados 7 LEDs,
// uno por cada día de la semana. El orden es: Domingo, Lunes, Martes, Miércoles, Jueves, Viernes, Sábado
// El Led del pin 21 correspondería al domingo, el del pin 2 al martes y así sucesivamente...
const int pinesDias[7] = {21, 2, 15, 4, 5, 18, 19};

// Estas listas indican a qué hora debe encenderse y apagarse el LED cada día.
// Cada posición corresponde al mismo día de la semana que en pinesDias.
// Por ejemplo, el lunes (posición 1) se enciende a las 13:41 y se apaga a las 13:42
const int horasEncendido[7] = {10, 13, 14, 15, 14, 17, 21};
const int minutosEncendido[7] = {28, 41, 41, 41, 41, 41, 50};
const int minutosApagado[7]  = {29, 42, 42, 42, 42, 42, 51};

// Esta variable se usa para asegurar que se envíe el mensaje solo una vez al día
bool mensajeEnviadoHoy = false;

// Esta variable guarda el último día (dia del mes) en que se envió el mensaje.
// Sirve para comparar con el día actual y evitar mensajes duplicados.
int ultimoDiaMensaje = -1;

// Esta función configura la hora actual de la ESP32 utilizando servidores de internet.
// Usa el horario de Colombia (UTC-5).
void configurarHora() {
  configTime(-5 * 3600, 0, "pool.ntp.org", "time.nist.gov");  // Servidores de hora
  Serial.print("Sincronizando hora"); // Muestra que está intentando sincronizar
  
  // Este ciclo espera hasta que el reloj interno se haya sincronizado correctamente.
  while (time(nullptr) < 100000) {
    Serial.print(".");
    delay(500); // Espera medio segundo antes de volver a intentar
  }
  Serial.println("\nHora sincronizada."); // Muestra que ya obtuvo la hora
}

// Esta función se ejecuta una vez por segundo. Y revisa si ya es la hora de encender
// el LED del día actual (correspondiente al dia) y envia el recordatorio por WhatsApp.
void revisarHoraYEncenderLED() {
  struct tm timeinfo;   // Esta estructura guarda la información sobre la hora actual
  
  // getLocalTime copia en timeinfo la hora actual.
  // Si falla, se muestra un mensaje y se sale de la función.
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error obteniendo la hora.");
    return;
  }

  // Extrae la hora, minuto, día del mes y día de la semana actuales
  int hora = timeinfo.tm_hour;
  int minuto = timeinfo.tm_min;
  int dia = timeinfo.tm_mday;
  int diaSemana = timeinfo.tm_wday; // 0 = Domingo, 1 = Lunes, ..., 6 = Sábado

  // Muestra en pantalla (monitor serial) la hora y el día actuales
  Serial.printf("Hora actual: %02d:%02d | Día: %d\n", hora, minuto, diaSemana);

   // Selecciona el pin correspondiente al día actual. Ejemplo: si hoy es martes (2), usará pinesDias[2]
   // En este ejemplo pinesDias[2] corresponde al Led conectado al pin 15
  int pinLED = pinesDias[diaSemana];

  // Toma también la hora y minutos de encendido y apagado para el día actual
  int horaEncender = horasEncendido[diaSemana];
  int minutoEncender = minutosEncendido[diaSemana];
  int minutoApagar = minutosApagado[diaSemana];
  
  // Encender LED y enviar mensaje Si ya es la hora de encender el LED:
  if (hora == horaEncender && minuto == minutoEncender) {
    digitalWrite(pinLED, HIGH);  // Enciende el LED correspondiente a ese día

    // Si aún no se ha enviado el mensaje hoy, lo envía
    if (!mensajeEnviadoHoy || dia != ultimoDiaMensaje) {
      String mensaje = "⚠ Alerta programada: Hora de tomar la pastilla del día.";
      Callmebot.whatsappMessage(phoneNumber, apiKey, mensaje); // Enviar mensaje de WhatsApp
      Serial.println("Mensaje enviado (por hora programada): " + mensaje);
      Serial.println(Callmebot.debug());  // Muestra información de depuración

      // Marca que el mensaje ya fue enviado hoy
      mensajeEnviadoHoy = true;
      ultimoDiaMensaje = dia;
    }

  } else if (hora == horaEncender && minuto == minutoApagar) {
    // Si ya pasó el minuto programado de apagado, apaga el LED
    digitalWrite(pinLED, LOW);

    // Reinicia la bandera para permitir que mañana se pueda enviar otro mensaje
    mensajeEnviadoHoy = false; 
  }
}

// Esta función se ejecuta una sola vez cuando la ESP32 se enciende o se reinicia.
void setup() {
  Serial.begin(115200); // Inicializa el monitor serial para mostrar mensajes de texto

 // Configura todos los pines donde están los LEDs como salidas, y los apaga
 for (int i = 0; i < 7; i++) {
    pinMode(pinesDias[i], OUTPUT);    // Define el pin como salida
    digitalWrite(pinesDias[i], LOW);  // Asegura que el LED esté apagado
  }

  // Conecta la ESP32 a la red WiFi usando los datos proporcionados
  WiFi.begin(ssid, password);
  Serial.println("Connecting"); // Muestra mensaje mientras intenta conectarse

  // Espera hasta que se conecte a la red WiFi
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  // Muestra puntos mientras espera
  }

  // Muestra mensaje cuando ya está conectado, y muestra la dirección IP
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Llama a la función para obtener la hora actual desde internet
  configurarHora();  // Obtener hora desde internet NTP
}

// Esta función se ejecuta repetidamente (miles de veces por segundo).
// Aquí se llama una vez por segundo a la función que revisa si es hora de actuar.
void loop() {
  revisarHoraYEncenderLED();  // Revisa si hay que encender LED y enviar recordatorio
  delay(1000);  // Espera 1 segundo antes de volver a intentarlo
}
