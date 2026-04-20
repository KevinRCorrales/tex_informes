/*
Grupo 1: Estación de confort para puesto de estudio

Sistema que evalua las condiciones de un puesto de trabajo o estudio
y a partir de los resultados toma decisiones para ajustar la luz del ambiente
y el encendido de un ventilador.

El usuario también puede decidir si usar un botón para alternar entre automático
y manual, si el usuario usa manual, puede ajustar la luz usando una perilla (potenciometro)
y encender o apagar el ventilador (motor) usando un segundo botón.

Además de los sensores, los pulsadores, el LED, el LCD y el motor, el sistema usa un LCD
con módulo I2C para mostrar información al usuario como la temperatura medida, el estado del
sistema (AUTO o MAN), el valor de la luz y el estado del motor.

============== FLUJO DE TRABAJO =================

 1. Usuario enciende el sistema
 2. Presiona el botón para cambiar a manual
 3. Ajusta la luz usando la perilla
 4. Enciende el ventilador usando el botón
 5. Si el usuario nota cambios frecuentes en el ambiente pulsa el primer botón
    y cambia a autómatico para que el sistema decida por él
*/

// Librerías necesarias para el sensor de temperatura y el LCD con módulo I2C
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pines
const int PIN_SENSOR = 2;
const int PIN_LED = 6; // Pin PWM para variar el brillo
const int PIN_MOTOR = 7;
const int PIN_BOTON_MODO = 3;
const int PIN_BOTON_MOTOR = 4;
// Pines analógicos para recibir cambios variados
const int PIN_POT = A1;
const int PIN_LDR = A0;

// Objetos necesarios para el manejo del sensor por medio del protocolo
// OneWire, las solicitudes de temperatura y el LCD usando el módulo I2C
OneWire onewire(PIN_SENSOR);
DallasTemperature sensor(&onewire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables de inicio
float temperaturaActual = 0.0;
int valorLuz = 0;

// Iniciar el sistema en automático y con el motor apagado a menos de que
// el modo automático lo encienda
bool sistemaManual = false;
bool motorEncendido = false;

// Antirrebote
// Iniciar en HIGH (Alto) esto en INPUT_PULLUP indica que el botó no está siendo presionado
bool estadoAnteriorModo = HIGH;
bool estadoAnteriorMotor = HIGH;
bool estadoBotonModo = HIGH;
bool estadoBotonMotor = HIGH;

unsigned long ultimoTiempoModo = 0;
unsigned long ultimoTiempoMotor = 0;
const unsigned long debounceDelay = 15; // Antirrebote

// Tiempo
unsigned long ultimoTiempoLectura = 0;
const unsigned long intervaloLectura = 1000;  // Un segundo para la actualización

void setup() {
  Serial.begin(9600); // Comunicación en el puerto serial, debe configurarse en 9600 como está en el begin
  // Led y motor como salida
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_MOTOR, OUTPUT);

  // Botones usando la resistencia INPUT_PULLUP
  pinMode(PIN_BOTON_MODO, INPUT_PULLUP);
  pinMode(PIN_BOTON_MOTOR, INPUT_PULLUP);

  sensor.begin(); // Iniciat el sensor
  lcd.init(); // Iniciar el LCD
  lcd.backlight(); // Encender el fondo del LCD
}

void loop() {
  // -------- BOTONES --------
  // Lectura inicial de los botones
  bool lecturaModo = digitalRead(PIN_BOTON_MODO);
  bool lecturaMotor = digitalRead(PIN_BOTON_MOTOR);

  // Antirrebote botón modo
  if (lecturaModo != estadoAnteriorModo) {
    // Si se detecta un cambio comenzamos a contar
    ultimoTiempoModo = millis();
  }

  if ((millis() - ultimoTiempoModo) >= debounceDelay) {
    // Revisamos si pasó el tiempo y si sigue estando presionado
    if (lecturaModo != estadoBotonModo) {
      // Guardamos en el estado la lectura tomada
      estadoBotonModo = lecturaModo;
      if (estadoBotonModo == LOW) {
        // Cambiamos el sistema al modo contrario
        sistemaManual = !sistemaManual;
      }
    }
  }

  // Actualizamos el estado anterior con la nueva lectura
  estadoAnteriorModo = lecturaModo;

  // Antirrebote botón acción
  if (lecturaMotor != estadoAnteriorMotor) {
    ultimoTiempoMotor = millis();
  }

  if ((millis() - ultimoTiempoMotor) >= debounceDelay) {
    if (lecturaMotor != estadoBotonMotor) {
      estadoBotonMotor = lecturaMotor;
      if (estadoBotonMotor == LOW) {
        if (sistemaManual) {
          // Admitir unicamente el cambio si estamos en modo manual
          // Sino estamos en modo manual, se ignora y se delega al auto
          // usando la medición de temperatura
          motorEncendido = !motorEncendido;
        } else {
          // Mensaje de advertencia para el usuario
          Serial.println("Rechazado, el sistema está en modo automático.");
        }
      }
    }
  }

  estadoAnteriorMotor = lecturaMotor;

  // -------- CONTROL CON MILLIS --------
  // Hacer la lectura y actualización con millis para no bloquear el Arduino con delay
  if (millis() - ultimoTiempoLectura >= intervaloLectura) {
    ultimoTiempoLectura = millis();

    // Tomamos la temperatura registrada por el sensor y la convertimos a grados centigrados
    sensor.requestTemperatures();
    temperaturaActual = sensor.getTempCByIndex(0);

    // Consultamos el valor registrado por el sensor de luz y por el potenciometro
    valorLuz = analogRead(PIN_LDR);
    int lecturaPot = analogRead(PIN_POT);

    if (!sistemaManual) {
      // Si el sistema está en modo automático aplicamos los cambios a partir del valor
      // de la luz y de la temperatura que fueron obtenidos por los sensores

      /* Realizamos un mapeo para convertir el valor analogo de la luz que va de 0 a 1023
      a un valor dentro del rango que pueda aceptar PWM (0 a 225) */
      int brillo = map(valorLuz, 0, 1023, 255, 0);
      analogWrite(PIN_LED, brillo);

      if (temperaturaActual > 28) {
        // Si la temperatura es alta encendemos el motor que funcionará como ventilador
        digitalWrite(PIN_MOTOR, HIGH);
      } else {
        // Sino nos aseguramos de que esté apagado
        digitalWrite(PIN_MOTOR, LOW);
      }

    } else {
      // por el contrario si estamos en modo manual ajustamos el brillo de acuerdo a lo
      // que diga el potenciometro usando un mapeo de analogico a PWM
      int brillo = map(lecturaPot, 0, 1023, 0, 255);
      // Mandamos ese valor al Pin del LED
      analogWrite(PIN_LED, brillo);
      // Encendemos o apagamos el motor de acuerdo al valor que haya escogido el usuario
      // al presionar el botón motor, si el usuario no ha presionado dicho botón, se queda apagado
      digitalWrite(PIN_MOTOR, motorEncendido);
    }

    // Nos situamos en la primer fila y primer columna del LCD y escribimos la temperatura
    lcd.setCursor(0, 0);
    lcd.print("T: ");
    if (temperaturaActual == -127.00) {
      lcd.print("ERROR  "); // 7 caracteres
      Serial.println("Error en la lectura del sensor, verifique la conexión.");
    } else {
      lcd.print(temperaturaActual); // 5 o incluso 6 caracteres e.g: 23.45 o 123.45
      lcd.print("C "); // 2 caracteres, esto suma 7 u 8 caracteres, ideal para una sobreescritura correcta sin usar clear
    }

    // Bajamos a la segunda fila
    lcd.setCursor(0, 1);

    // Escribimos en Serial un log (registro) más detallado
    Serial.print("Estado del sistema: ");
    if (sistemaManual) {
      // Escribimos en el LCD y el Serial dependiendo del estado del sistema
      lcd.print("MAN ");
      Serial.println("MANUAL");
    } else {
      lcd.print("AUTO");
      Serial.println("AUTOMATICO");
    }

    // Escribimos unicamente en el LCD el valor de la luz y el estado booleano del botón (1 o 0)
    lcd.print(" L:");
    lcd.setCursor(7, 1);
    lcd.print(valorLuz);

    lcd.print(" M:");
    lcd.print(digitalRead(PIN_MOTOR));
    lcd.print(" ");
  }
}
