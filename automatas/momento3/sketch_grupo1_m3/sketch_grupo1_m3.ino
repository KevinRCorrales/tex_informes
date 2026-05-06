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
bool usuarioExiste = false;
bool limpiarResiduo = false;
// Tiempos para medir con millis
unsigned long tAlerta = 0;
unsigned long tExito = 0;
unsigned long tBloqueo = 0;
unsigned long tMotor = 0;
// Contador de intentos fallidos
int fallidos = 0;
// Limites del servomotor para no llevarlo al límite de su torque
const int A_MAX = 170;
const int A_MIN = 10;

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

Servo motor;

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

bool verificarExistencia(String user) {
  for (int i = 0; i < 3; i++) {
    if (users[i].id == user) {
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

  motor.attach(SERVO_PIN);
  motor.write(A_MAX);  // Posicionar el candado inicialmente como cerrado
  IrReceiver.begin(PIN_RECEPTOR, ENABLE_LED_FEEDBACK); // Inicializar el IRL
}

void loop() {
  char tecla = keypad.getKey();

  if (tecla && estado != BLOQUEO) {
    estado = INGRESO;

    // Ingreso de Usuario
    if (!ingresandoPassword) {

      usuario += tecla;

      // Cuando el usuario tenga 4 caracteres
      if (usuario.length() == 4) {
        if (verificarExistencia(usuario)) {
          ingresandoPassword = true;
          lcd.setCursor(0, 0);
          lcd.print("Ingrese clave  "); // Espacios extra para sobreescribir el mensaje anterior
        } else {
          lcd.setCursor(0, 1);
          lcd.print("Error");
          digitalWrite(PIN_LED, HIGH);
          digitalWrite(BUZZER, HIGH);
          tAlerta = millis();
          fail(F("El usuario ingresado no existe"));
          usuario = "";
        }
        limpiarResiduo = true; // Limpiar el residuo
      } else {
        lcd.setCursor(0, 1);
        lcd.print(usuario);
      }
    }

    // Ingreso de contraseña
    else {
      password += tecla;

      lcd.setCursor(password.length() - 1, 1);
      lcd.print("*");

      // Cuando la contraseña tenga 4 caracteres
      if (password.length() == 4) {
        if (validarUsuario(usuario, password)) {
          lcd.setCursor(0, 1);
          lcd.print("Acceso OK");
          ok(F("Acceso concedido"));
          estado = PERMITIDO;
          motor.write(A_MIN);
          digitalWrite(PIN_LED_EXITO, HIGH);
          tMotor = millis();
          tExito = millis();
        } else {
          lcd.setCursor(0, 1);
          lcd.print("Error    ");
          fail(F("Acceso denegado"));
          digitalWrite(PIN_LED, HIGH);
          digitalWrite(BUZZER, HIGH);
          tAlerta = millis(); // Iniciar conteo de la alerta
          fallidos++;
          limpiarResiduo = true;
        }

        if (fallidos == 3) {
          estado = BLOQUEO; // Cambiar estado
          tBloqueo = millis(); // Empezar conteo del bloqueo
          fallidos = 0; // Reseteo para el próximo login
          ingresandoPassword = false;
          lcd.setCursor(0, 0);
          lcd.print("Bloqueo: Use IR");
          lcd.setCursor(0, 1);
          lcd.print("o espere 15 sg");
          limpiarResiduo = false; // Asegurar que parte de nuestro mensaje no sea eliminado
        }

        // Reseteo del Sistema
        usuario = "";
        password = "";
      }
    }
  }

// Lectura y decodificación de señal del control remoto IR
if (IrReceiver.decode()) {

  uint32_t codigo = IrReceiver.decodedIRData.command;

  Serial.print("IR: ");
  Serial.println(codigo);

  IrReceiver.resume();

  // DEsbloqueo 
  if (estado == BLOQUEO && codigo == 162) { // Boton Power en el control IR 
    estado = REPOSO;
    lcd.clear();
    lcd.print("Desbloqueado");
    delay(1000);
    lcd_login();
  }
   // Apertura Directa (ADMIN)
  if (codigo == 162) { // Boton Power en el control IR 

    lcd.clear();
    lcd.print("Admin acceso");

    motor.write(A_MIN);
    candadoCerrado = false;

    digitalWrite(PIN_LED_EXITO, HIGH);

    tMotor = millis();
    tExito = millis();

    estado = PERMITIDO;

    Serial.println("Apertura por IR (admin)");
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
  if (((estado == INGRESO) || (estado == BLOQUEO)) && (millis() - tAlerta >= 1000)) {
    digitalWrite(PIN_LED, LOW);
    digitalWrite(BUZZER, LOW);
    if (limpiarResiduo) {
      lcd.setCursor(0, 1);
      lcd.print("     ");
      limpiarResiduo = false;
    }
  }

  if (!candadoCerrado && (millis() - tMotor) >= 10000) {
    motor.write(A_MAX);
    candadoCerrado = true;
    Serial.println(F("Candado cerrado por seguridad trás 10 segundos"));
  }

  if (estado == BLOQUEO && millis() - tBloqueo >= 15000) {
    estado = REPOSO;
    lcd_login();
    lcd.setCursor(0, 1);
    lcd.print("              ");
  }

  if (estado == PERMITIDO && millis() - tExito >= 1000) {
    digitalWrite(PIN_LED_EXITO, LOW);
    lcd_login();
    estado = REPOSO;
    lcd.setCursor(0, 1);
    lcd.print("         ");
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
