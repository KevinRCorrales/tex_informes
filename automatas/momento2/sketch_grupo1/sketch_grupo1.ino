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
const int PIN_MOTOR = 8;
const int PIN_BOTON_MANUAL = 3;
const int PIN_BOTON_AUTO = 4;
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
bool estadoManualSalida = false;

// Antirrebote 
bool estadoAnteriorModo = HIGH;
bool estadoAnteriorAccion = HIGH;

// Tiempo
unsigned long ultimoTiempoLectura = 0;
const unsigned long intervaloLectura = 500;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_MOTOR, OUTPUT);

  pinMode(PIN_BOTON_MANUAL, INPUT_PULLUP);
  pinMode(PIN_BOTON_AUTO, INPUT_PULLUP);

  sensor.begin();
  lcd.init();
  lcd.backlight();
}

void loop() {

  // -------- BOTONES --------
  bool lecturaModo = digitalRead(PIN_BOTON_MANUAL);
  bool lecturaAccion = digitalRead(PIN_BOTON_AUTO);

  if (lecturaModo == LOW && estadoAnteriorModo == HIGH) {
    sistemaManual = !sistemaManual;
  }
  estadoAnteriorModo = lecturaModo;

  if (lecturaAccion == LOW && estadoAnteriorAccion == HIGH) {
    estadoManualSalida = !estadoManualSalida;
  }
  estadoAnteriorAccion = lecturaAccion;

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

      digitalWrite(PIN_MOTOR, estadoManualSalida);
    }

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temperaturaActual);
    lcd.print("C");

    lcd.setCursor(0, 1);

    if (sistemaManual) {
      lcd.print("MAN ");
    } else {
      lcd.print("AUTO ");
    }

    lcd.print("L:");
    lcd.print(valorLuz);

    lcd.print(" M:");
    lcd.print(digitalRead(PIN_MOTOR));
  }
}