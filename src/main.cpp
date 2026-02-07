#include <Arduino.h>

int STBY = 9;   // Standby pin

// Pinos do TB6612FNG para Motor A (lado direito)
int AIN1 = 8;   // Direction control 1 (motor traseiro)
int AIN2 = 7;   // Direction control 2 (motor dianteiro)
int PWMA = 6;   // Speed control (PWM)

// Pinos do TB6612FNG para Motor B (lado esquerdo)
int BIN1 = 5;   // Direction control 1 (motor traseiro)
int BIN2 = 4;   // Direction control 2 (motor dianteiro)
int PWMB = 3;   // Speed control (PWM)

// Declaração das funções
void frente();
void tras();
void parada();

void setup() {
  // Inicializar Serial para debug (9600 baud)
  Serial.begin(9600);
  Serial.println("=== Sistema iniciado ===");
  
  // Pino de controle lógico
  pinMode(STBY, OUTPUT);
  
  // Pinos A (lado direito)
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  // Pinos B (lado esquerdo)
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);  

  // Habilitar o driver
  digitalWrite(STBY, HIGH);
  Serial.println("Driver habilitado");
}

// Motor movendo para frente
void frente() {
  Serial.println("-> FRENTE");
  // Motor A
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 255);
  Serial.println("   Motor A: AIN1=LOW, AIN2=HIGH, PWMA=255");

  // Motor B
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 255);
  Serial.println("   Motor B: BIN1=HIGH, BIN2=LOW, PWMB=255");
}

// Motor movendo para trás
void tras() {
  Serial.println("-> TRÁS");
  // Motor A
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 255);
  Serial.println("   Motor A: AIN1=HIGH, AIN2=LOW, PWMA=255");

  // Motor B
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, 255);
  Serial.println("   Motor B: BIN1=LOW, BIN2=HIGH, PWMB=255");
}

// Motor parado
void parada() {
  Serial.println("-> PARADA");
  // Motor A
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 0);

  // Motor B
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 0);
  Serial.println("   Todos os motores parados");
}

void loop() {
  frente();
  delay(5000);

  parada();
  delay(5000);

  tras();
  delay(5000);
}