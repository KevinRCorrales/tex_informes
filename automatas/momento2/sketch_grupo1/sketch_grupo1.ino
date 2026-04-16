/*
Grupo 1: Estación de confort para puesto de estudio
*/


// Librerías necesarias para el sensor y el LCD
#include <OneWire.h>            // Protocolo de comunicación del sensor
#include <DallasTemperature.h>  // Lectura y consulta de temperaturas
// LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pines a usar
const int PIN_SENSOR = 2;
const int PIN_LED = 6;
const int PIN_BOTON_MANUAL = 3;
const int PIN_BOTON_AUTO = 4;
const int pwm = A1;

// Objetos para el sensor y el LCD
OneWire onewire(PIN_SENSOR);
DallasTemperature sensor(&onewire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables de estado
float temperaturaActual = 0.0;

// Variable para saber si el sistema está en automatico o en manual
bool sistemaManual = false;

bool estadoAnteriorIniciar = HIGH;
bool estadoAnteriorBajar = HIGH;
bool estadoAnteriorSubir = HIGH;

unsigned long ultimoTiempoLectura = 0;
const unsigned long intervaloLectura = 1000;

void setup() {
  // Configurar botones como INPUT_PULLUP
  pinMode(PIN_BOTON_MANUAL, INPUT_PULLUP);
  pinMode(PIN_BOTON_AUTO, INPUT_PULLUP);

  // Iniciar el LCD y el sensor
  sensor.begin();
  lcd.init();
  lcd.backlight();
}

void loop() {
  int lectura = analogRead(pwm);
  temperaturaActual = sensor.getTempCByIndex(0);
  if (millis() - ultimoTiempoLectura >= intervaloLectura) {
    ultimoTiempoLectura = millis();
    sensor.requestTemperatures();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperaturaActual);
    lcd.print(" C");
  }
}
