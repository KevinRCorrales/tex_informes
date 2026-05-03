#include <LiquidCrystal_I2C.h>  // Librería necesaria para manejar el LCD I2C
#include <Servo.h>              // Librería necesaria para manejar el servomotor
#include <IRremote.hpp>         // Librería necesaria para manejar el receptor infrarrojo y el control remoto
#include <Keypad.h>             // Librería necesaria para el manejo del keypad 4x4

// Estados del sistema en enum
enum EstadoSistema {
  REPOSO,
  INGRESO,
  PERMITIDO,
  DENEGADO,
  BLOQUEO
};

EstadoSistema estado = REPOSO;

// Estructura para crear usuarios con un id o nombre de usuario y una contraseña
struct Usuario {
  String id;
  String passwd;
};

// Array que funciona como DataBase de usuarios
Usuario users[3] = {
  { "A198", "9*D2" },
  { "B123", "83A#" },
  { "C000", "AD23" }
};

// Definicion de ROWS y COLS = Tamaño del Keypad
const byte ROWS = 4;
const byte COLS = 4;

// Variables de Estado y Pines a usar en el montaje
const byte SERVO_PIN = 10;
const byte PIN_RECEPTOR = 11;
const byte PIN_LED = 13;
const byte BUZZER = 12;
const byte PIN_LED_EXITO = A1;  // No hay mas pines :(
String usuario = "";
String password = "";
bool ingresandoPassword = false;
bool candadoCerrado = true;
// Tiempos para medir con millis
unsigned long tAlerta = 0;
unsigned long tExito = 0;
unsigned long tBloqueo = 0;
unsigned long tMotor = 0;
// Contador de intentos fallidos
int fallidos = 0;

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

// Función de validación
bool validarUsuario(String user, String pass) {
  for (int i = 0; i < 3; i++) {
    if (users[i].id == user && users[i].passwd == pass) {
      return true;
    }
  }
  return false;
}

void setup() {
  Serial.begin(9600);

  // Inicializador de el LCD
  lcd.init();
  lcd.backlight();

  lcd_login();

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_LED_EXITO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

void loop() {

  char tecla = keypad.getKey();

  if (tecla) {

    // Ingreso de Usuario
    if (!ingresandoPassword) {

      usuario += tecla;

      lcd.setCursor(0, 1);
      lcd.print(usuario);

      // Cuando el usuario tenga 4 caracteres
      if (usuario.length() == 4) {
        ingresandoPassword = true;
        lcd.clear();
        lcd.print("Ingrese clave");
      }
    }

    // Ingreso de contraseña
    else {

      password += tecla;

      lcd.setCursor(password.length() - 1, 1);
      lcd.print("*");

      // Cuando la contraseña tenga 4 caracteres
      if (password.length() == 4) {
        lcd.print("Validando...");

        if (validarUsuario(usuario, password)) {

          lcd.setCursor(0, 1);
          lcd.print("Acceso OK");
          ok(F("Acceso concedido"));

        } else {

          lcd.setCursor(0, 1);
          lcd.print("Error");
          fail(F("Acceso denegado"));
          fallidos++;
        }

        if (fallidos == 3) {
          estado = BLOQUEO; // Cambiar estado
          tBloqueo = millis(); // Empezar conteo del bloqueo
          fallidos = 0; // Reseteo para el próximo login
        }

        // Reseteo del Sistema
        usuario = "";
        password = "";
        ingresandoPassword = false;

        lcd_login();
      }
    }
  }

  verificacion();
}

void lcd_login() {
  lcd.setCursor(0, 0);
  lcd.print("Ingrese usuario");
}

void verificacion() {
  /*
  Función que verifica el tiempo transcurrido desde que se encendió la
  alarma o se giró el motor, se usa millis en vez de delay para que Arduino
  pueda seguir trabajando en otras tareas y no se bloquee
  */
  if ((millis() - tAlerta >= 1000)) {
    digitalWrite(PIN_LED, LOW);
    digitalWrite(BUZZER, LOW);
  }

  if (!candadoCerrado && (millis() - tMotor) >= 10000) {
    motor.write(A_MAX);
    candadoCerrado = true;
    Serial.println(F("Candado cerrado por seguridad trás 10 segundos"));
  }

  if (estado == BLOQUEADO && millis() - tBloqueo >= 15000) {
    estado = INICIAL;
    lcd_login();
  }

  if (millis() - tExito >= 1000) {
    digitalWrite(PIN_LED_EXITO, LOW);
  }
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
