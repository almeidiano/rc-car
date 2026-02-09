#include <Arduino.h>

int STBY = 9;   // Standby pin
int PWMAB = 6;  // PWM pin

// Motor A (lado esquerdo)
int ABIN1 = 7;

// Motor B (lado direito)
int ABIN2 = 8;

// // Declaração das funções
// void parada();
// void frente();
// void tras();
// void curvaDireita();
// void curvaEsquerda();
// void curvaExtremaEsquerda();
// void curvaExtremaDireita();

// void setup() {
//   // Inicializar Serial para debug (9600 baud)
//   Serial.begin(9600);
  
//   // Pino de controle lógico
//   pinMode(STBY, OUTPUT);
  
//   // Pinos A (lado direito)
//   pinMode(AIN1, OUTPUT);
//   pinMode(AIN2, OUTPUT);
//   pinMode(PWMA, OUTPUT);

//   // Pinos B (lado esquerdo)
//   pinMode(BIN1, OUTPUT);
//   pinMode(BIN2, OUTPUT);
//   pinMode(PWMB, OUTPUT);  

//   // Habilitar o driver
//   digitalWrite(STBY, HIGH);
// }

// // Motor movendo para frente
// void frente() {
//   Serial.println("-> FRENTE");
//   // Motor A (lado esquerdo)
//   digitalWrite(AIN1, LOW);
//   digitalWrite(AIN2, HIGH);
//   analogWrite(PWMA, 255);
//   Serial.println("   Motor A: AIN1=LOW, AIN2=HIGH, PWMA=255");

//   // Motor B (lado direito)
//   digitalWrite(BIN1, HIGH);
//   digitalWrite(BIN2, LOW);
//   analogWrite(PWMB, 255);
//   Serial.println("   Motor B: BIN1=HIGH, BIN2=LOW, PWMB=255");
// }

// // Motor movendo para trás
// void tras() {
//   Serial.println("-> TRÁS");
//   // Motor A (lado esquerdo)
//   digitalWrite(AIN1, HIGH);
//   digitalWrite(AIN2, LOW);
//   analogWrite(PWMA, 255);
//   Serial.println("   Motor A: AIN1=HIGH, AIN2=LOW, PWMA=255");

//   // Motor B (lado direito)
//   digitalWrite(BIN1, LOW);
//   digitalWrite(BIN2, HIGH);
//   analogWrite(PWMB, 255);
//   Serial.println("   Motor B: BIN1=LOW, BIN2=HIGH, PWMB=255");
// }

// // Motor parado
// void parada() {
//   Serial.println("-> PARADA");
//   // Motor A 
//   digitalWrite(AIN1, LOW);
//   digitalWrite(AIN2, LOW);
//   analogWrite(PWMA, 0);

//   // Motor B
//   digitalWrite(BIN1, LOW);
//   digitalWrite(BIN2, LOW);
//   analogWrite(PWMB, 0);
//   Serial.println("   Todos os motores parados");
// }

// void curvaSuaveDireita() {
//   // Para curvar à direita avançando:
//   // Motor ESQUERDO rápido, motor DIREITO lento.
  
//   digitalWrite(AIN1, LOW); 
//   digitalWrite(AIN2, HIGH); 
//   analogWrite(PWMA, 250); // Rápido
  
//   digitalWrite(BIN1, HIGH); 
//   digitalWrite(BIN2, LOW);
//   analogWrite(PWMB, 170); // Lento
// }

// void curvaSuaveEsquerda() {
//   // Para curvar à esquerda avançando:
//   // Motor DIREITO rápido, motor ESQUERDO lento.
  
//   digitalWrite(BIN1, HIGH); 
//   digitalWrite(BIN2, LOW);
//   analogWrite(PWMB, 250); // Rápido
  
//   digitalWrite(AIN1, LOW); 
//   digitalWrite(AIN2, HIGH);
//   analogWrite(PWMA, 170); // Lento
// }

// // A ideia é realizar um movimento de 360 graus para a esquerda, ou seja, um movimento no sentido anti-horário. 
// void curvaExtremaEsquerda() {
//   Serial.println("-> CURVA ESQUERDA");

//   digitalWrite(BIN1, HIGH); 
//   digitalWrite(BIN2, LOW);
//   analogWrite(PWMB, 250); // Rápido
  
//   digitalWrite(AIN1, LOW); 
//   digitalWrite(AIN2, HIGH);
//   analogWrite(PWMA, 0); // Parado
// }

// // O contrário do comando anterior 
// void curvaExtremaDireita() {
//   Serial.println("-> CURVA DIREITA");

//   digitalWrite(AIN1, LOW); 
//   digitalWrite(AIN2, HIGH); 
//   analogWrite(PWMA, 250); // Rápido
  
//   digitalWrite(BIN1, HIGH); 
//   digitalWrite(BIN2, LOW);
//   analogWrite(PWMB, 0); // Parado
// }

// void loop() {
//   frente();
//   delay(5000);

//   parada();
//   delay(5000);

//   tras();
//   delay(5000);

//   parada();
//   delay(5000);

//   curvaSuaveDireita();
//   delay(5000);

//   parada();
//   delay(5000);

//   curvaSuaveEsquerda();
//   delay(5000);

//   parada();
//   delay(5000);

//   curvaExtremaEsquerda();
//   delay(5000);

//   parada();
//   delay(5000);

//   curvaExtremaDireita();
//   delay(5000);
// }