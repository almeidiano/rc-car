// 2x_drivers_semi_parallel
#include <Arduino.h>

// --- LADO DIREITO ---
const int RIGHT_PWM = 10;  // Pino PWM (~)
const int R_AIN = 2;
const int R_BIN = 3;

// --- LADO ESQUERDO ---
const int LEFT_PWM = 11;   // Pino PWM (~)
const int L_AIN = 6;
const int L_BIN = 7;

void setup() {
  Serial.begin(9600);
  
  // Configura todos como saída em um loop simples
  for(int i = 2; i <= 11; i++) {
    pinMode(i, OUTPUT);
  }

  // STBY está no 5V físico, então não precisa de código!
  Serial.println("4WD Sequencial Pronto!");
}

void mover(int velEsq, int velDir, bool frenteEsq, bool frenteDir) {
  // Motores Esquerdos
  digitalWrite(L_AIN, frenteEsq ? LOW : HIGH);
  digitalWrite(L_BIN, frenteEsq ? LOW : HIGH);
  analogWrite(LEFT_PWM, velEsq);

  // Motores Direitos
  digitalWrite(R_AIN, frenteDir ? LOW : HIGH);
  digitalWrite(R_BIN, frenteDir ? LOW : HIGH);
  analogWrite(RIGHT_PWM, velDir);
}

void curvaSuaveDireita() {
  // Para curvar à direita avançando:
  // Motor ESQUERDO rápido, motor DIREITO lento.
  
  digitalWrite(AIN1, LOW); 
  digitalWrite(AIN2, HIGH); 
  analogWrite(PWMA, 250); // Rápido
  
  digitalWrite(BIN1, HIGH); 
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 170); // Lento
}

void curvaSuaveEsquerda() {
  // Para curvar à esquerda avançando:
  // Motor DIREITO rápido, motor ESQUERDO lento.
  
  digitalWrite(BIN1, HIGH); 
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 250); // Rápido
  
  digitalWrite(AIN1, LOW); 
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 170); // Lento
}

// A ideia é realizar um movimento de 360 graus para a esquerda, ou seja, um movimento no sentido anti-horário. 
void curvaExtremaEsquerda() {
  Serial.println("-> CURVA ESQUERDA");

  digitalWrite(BIN1, HIGH); 
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 250); // Rápido
  
  digitalWrite(AIN1, LOW); 
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 0); // Parado
}

// O contrário do comando anterior 
void curvaExtremaDireita() {
  Serial.println("-> CURVA DIREITA");

  digitalWrite(AIN1, LOW); 
  digitalWrite(AIN2, HIGH); 
  analogWrite(PWMA, 250); // Rápido
  
  digitalWrite(BIN1, HIGH); 
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 0); // Parado
}

void loop() {
  mover(200, 200, true, true); // Sempre frente
}