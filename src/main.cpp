// PWMA: Controle de velocidade do Motor A (sinal PWM). Use 0 para parar e 255 (em 8-bit) para velocidade máxima.
// AIN1 & AIN2: Controle de direção do Motor A.
// IN1=Alto, IN2=Baixo: Sentido horário (CW).
// IN1=Baixo, IN2=Alto: Sentido anti-horário (CCW).
// IN1=Baixo, IN2=Baixo: Parada (Stop).
// IN1=Alto, IN2=Alto: Freio curto (Short Brake).

// #include <Arduino.h>

// // Pinos do TB6612FNG para Motor A
// int STBY = 9;   // Standby pin
// int AIN1 = 8;   // Direction control 1
// int AIN2 = 7;   // Direction control 2
// int PWMA = 6;   // Speed control (PWM)

// // Declaração das funções
// void frente();
// void tras();
// void parada();
// void freio();

// void setup() {
//   // Pinos A (lado esquerdo)
//   // Configurar pinos como OUTPUT
//   pinMode(STBY, OUTPUT);
//   pinMode(AIN1, OUTPUT);
//   pinMode(AIN2, OUTPUT);
//   pinMode(PWMA, OUTPUT);

//   // Pinos B (lado direito)
//   // Configurar pinos como OUTPUT
//   pinMode(STBY, OUTPUT);
//   pinMode(BIN1, OUTPUT);
//   pinMode(BIN2, OUTPUT);
//   pinMode(PWMB, OUTPUT);  

//   // Habilitar o driver (tirar do standby)
//   digitalWrite(STBY, HIGH);
// }

// // Motor movendo para frente
// void frente() {
//   digitalWrite(AIN1, HIGH);
//   digitalWrite(AIN2, LOW);
//   analogWrite(PWMA, 255);
// }

// // Motor movendo para trás
// void tras() {
//   digitalWrite(AIN1, LOW);
//   digitalWrite(AIN2, HIGH);
//   analogWrite(PWMA, 255);
// }

// // Motor parado (sem freio)
// void parada() {
//   digitalWrite(AIN1, LOW);
//   digitalWrite(AIN2, LOW);
//   analogWrite(PWMA, 0);
// }

// // Freio curto (AIN1 e AIN2 EM HIGH)
// void freio() {
//   digitalWrite(AIN1, HIGH);
//   digitalWrite(AIN2, HIGH);
//   analogWrite(PWMA, 0);
// }

// void loop() {
//   // Estado 1: Frente (5 segundos)
//   frente();
//   delay(5000);

//   // Estado 2: Trás (5 segundos)
//   tras();
//   delay(5000);

//   // Estado 3: Parada (5 segundos)
//   parada();
//   delay(5000);

//   // // Estado 4: Freio curto (5 segundos)
//   // freio();
//   // delay(5000);
// }

// curvas
// #include <Arduino.h>
// const int AIN1 = 8;
// const int AIN2 = 9;
// const int PWMA = 10; // Precisa ser pino PWM (~)
// const int BIN1 = 7;
// const int BIN2 = 6;
// const int PWMB = 5;  // Precisa ser pino PWM (~)
// const int STBY = 11;

// void curvaSuaveDireita();
// void irParaFrente();  

// void setup() {
//   pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT); pinMode(PWMA, OUTPUT);
//   pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT); pinMode(PWMB, OUTPUT);
//   pinMode(STBY, OUTPUT);
  
//   digitalWrite(STBY, HIGH); // Ativa o driver
// }

// void loop() {
//   curvaSuaveDireita();
//   delay(3000);
  
//   irParaFrente();
//   delay(2000);
// }

// void irParaFrente() {
//   // Lado Esquerdo - Frente (Velocidade Total)
//   digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
//   analogWrite(PWMA, 200);
  
//   // Lado Direito - Frente (Velocidade Total)
//   digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
//   analogWrite(PWMB, 200);
// }

// void curvaSuaveDireita() {
//   // Para curvar à direita avançando:
//   // Motor ESQUERDO rápido, motor DIREITO lento.
  
//   digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
//   analogWrite(PWMA, 250); // Rápido
  
//   digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
//   analogWrite(PWMB, 100); // Lento
// }

// Frente, trás, curvas para direita e esquerda
#include <Arduino.h>

// Pinos do TB6612FNG para Motor A
int STBY = 9;   // Standby pin
int AIN1 = 8;   // Direction control 1
int AIN2 = 7;   // Direction control 2
int PWMA = 6;   // Speed control (PWM)

// Pinos do TB6612FNG para Motor B
int BIN1 = 5;   // Direction control 1
int BIN2 = 4;   // Direction control 2
int PWMB = 3;   // Speed control (PWM)

// Declaração das funções
void frente();
void tras();
void parada();
void freio();

void setup() {
  // Pinos A (lado esquerdo)
  // Configurar pinos como OUTPUT
  pinMode(STBY, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  // Pinos B (lado direito)
  // Configurar pinos como OUTPUT
  pinMode(STBY, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);  

  // Habilitar o driver (tirar do standby)
  digitalWrite(STBY, HIGH);
}

// Motor movendo para frente
void frente() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 120);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 120);
}

// Motor movendo para trás
void tras() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 120);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, 120);
}

// Motor parado (sem freio)
void parada() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 0);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 0);
}

// Freio curto (AIN1 e AIN2 EM HIGH)
void freio() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 0);
}

void loop() {
  // Estado 1: Frente (5 segundos)
  frente();
  delay(5000);

  // Estado 2: Trás (5 segundos)
  tras();
  delay(5000);

  // Estado 3: Parada (5 segundos)
  parada();
  delay(5000);

  // // Estado 4: Freio curto (5 segundos)
  // freio();
  // delay(5000);
}