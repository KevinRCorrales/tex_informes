/*
Grupo 1: Estación de confort para puesto de estudio
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pines
const int PIN_SENSOR = 2;
const int PIN_LED = 6;
const int PIN_MOTOR = 7;
const int PIN_BOTON_MODO = 3;
const int PIN_BOTON_MOTOR = 4;
const int PIN_POT = A1;
const int PIN_LDR = A0;

// Objetos
OneWire onewire(PIN_SENSOR);
DallasTemperature sensor(&onewire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables
float temperaturaActual = 0.0;
int valorLuz = 0;

bool sistemaManual = false;
bool motorEncendido = false;

// Antirrebote
bool estadoAnteriorModo = HIGH;
bool estadoAnteriorMotor = HIGH;
bool estadoBotonModo = HIGH;
bool estadoBotonMotor = HIGH;

unsigned long ultimoTiempoModo = 0;
unsigned long ultimoTiempoMotor = 0;
const unsigned long debounceDelay = 15;

// Tiempo
unsigned long ultimoTiempoLectura = 0;
const unsigned long intervaloLectura = 1000;  // Un seguno para la actualización

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_MOTOR, OUTPUT);

  pinMode(PIN_BOTON_MODO, INPUT_PULLUP);
  pinMode(PIN_BOTON_MOTOR, INPUT_PULLUP);

  sensor.begin();
  lcd.init();
  lcd.backlight();
}

void loop() {
  // Comenzar conteo
  unsigned long tiempo = millis();
  // -------- BOTONES --------
  bool lecturaModo = digitalRead(PIN_BOTON_MODO);
  bool lecturaMotor = digitalRead(PIN_BOTON_MOTOR);

  // Antirrebote botón modo
  if (lecturaModo != estadoAnteriorModo) {
    ultimoTiempoModo = millis();
  }

  if ((millis() - ultimoTiempoModo) >= debounceDelay) {
    if (lecturaModo != estadoBotonModo) {
      estadoBotonModo = lecturaModo;
      if (estadoBotonModo == LOW) {
        sistemaManual = !sistemaManual;
      }
    }
  }

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
          Serial.println("Rechazado, el sistema está en modo automático.");
        }
      }
    }
  }

  estadoAnteriorMotor = lecturaMotor;

  // -------- CONTROL CON MILLIS --------
  if (millis() - ultimoTiempoLectura >= intervaloLectura) {
    ultimoTiempoLectura = millis();

    sensor.requestTemperatures();
    temperaturaActual = sensor.getTempCByIndex(0);

    valorLuz = analogRead(PIN_LDR);
    int lecturaPot = analogRead(PIN_POT);

    if (!sistemaManual) {

      int brillo = map(valorLuz, 0, 1023, 255, 0);
      analogWrite(PIN_LED, brillo);

      if (temperaturaActual > 28) {
        digitalWrite(PIN_MOTOR, HIGH);
      } else {
        digitalWrite(PIN_MOTOR, LOW);
      }

    } else {
      int brillo = map(lecturaPot, 0, 1023, 0, 255);
      analogWrite(PIN_LED, brillo);
      digitalWrite(PIN_MOTOR, motorEncendido);
    }

    lcd.setCursor(0, 0);
    lcd.print("T: ");
    lcd.print(temperaturaActual);
    lcd.print("C");

    lcd.setCursor(0, 1);

    Serial.print("Estado del sistema: ");
    if (sistemaManual) {
      lcd.print("MAN ");
      Serial.println("MANUAL");
    } else {
      lcd.print("AUTO");
      Serial.println("AUTOMATICO");
    }

    lcd.print(" L:");
    lcd.print(valorLuz);

    lcd.print(" M:");
    lcd.print(digitalRead(PIN_MOTOR));
  }
}
