#include <LiquidCrystal_I2C.h>  // Librería necesaria para manejar el LCD I2C
#include <Servo.h>              // Librería necesaria para manejar el servomotor
#include <IRremote.hpp>         // Librería necesaria para manejar el receptor infrarrojo y el control remoto
#include <Keypad.h>             // Librería necesaria para el manejo del keypad 4x4

// Definicion de ROWS y COLS = Tamaño del Keypad
const byte ROWS = 4;
const byte COLS = 4;

// Variables de Estado y Pines a usar en el montaje
const byte SERVO_PIN = 10;
const byte PIN_RECEPTOR = 11;
const byte PIN_LED = 13;
const byte BUZZER = 12;
const byte PIN_LED_EXITO = A1;  // No hay mas pines :(

// Creacion de el mapa de teclas
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = { 9, 8, 7, 6 };  // Pines a usar para las filas
byte colPins[COLS] = { 5, 4, 3, 2 };  // Pines a usar para las columnas

// Crear objeto keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


// Creacion de el LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);

  // Inicializador de el LCD
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Sistema listo");
}

void loop() {
}


/*
============================================
Funciones para debug en serial ok funciona para cuando algo se inicia correctamente
y fail para cuando se producen fallos y el usuario debe conocerlos
*/
void ok(const __FlashStringHelper* msg) {
  Serial.print(F("[  OK  ] "));
  Serial.println(msg);
}

void fail(const __FlashStringHelper* msg) {
  Serial.print(F("[FAILED] "));
  Serial.println(msg);
}
