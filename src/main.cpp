// RC Car - Arduino Uno (slave) + 2x TB6612FNG (um por lado) com canais A+B em paralelo
// ---------------------------------------------------------------
// Cada driver tem PWMA+PWMB jumpeados em 1 pino e AIN1+BIN1 / AIN2+BIN2
// jumpeados em 1 pino cada. VCC do driver no 5V do Uno, STBY controlado por pino.
//
// Driver ESQUERDO (LEFT):
//   PWMA+PWMB  -> D9    (PWM - Timer1)
//   AIN1+BIN1  -> D7    (direção IN1)
//   AIN2+BIN2  -> D8    (direção IN2)
//   STBY       -> D4    (standby - LOW=desligado, HIGH=ativo)
//
// Driver DIREITO (RIGHT):
//   PWMA+PWMB  -> D10   (PWM - Timer1)
//   AIN1+BIN1  -> D5    (direção IN1)
//   AIN2+BIN2  -> D6    (direção IN2)
//   STBY       -> D2    (standby - LOW=desligado, HIGH=ativo)
//
// D0/D1 ficam livres: são a Serial USB usada pelo Android (master).
// D13 evitado: o LED pisca no boot e faria o motor dar um tranco.
//
// Saídas: AO1+BO1 / AO2+BO2 de cada driver -> motores do respectivo lado
//
// ===================== PROTOCOLO SERIAL (115200 8N1) =====================
// Um comando por linha, terminado em '\n':
//   F      -> frente
//   B      -> ré
//   L      -> girar esquerda
//   R      -> girar direita
//   S      -> parar (standby)
//   X      -> freio
//   V<n>   -> velocidade 0-255 (ex: V150)
// Resposta: "OK <cmd>" ou "ERR <cmd>".
//
// Failsafe: se o carro estiver em movimento e nenhum comando chegar em
// TIMEOUT_MS, ele para sozinho. O master deve reenviar o comando de
// movimento periodicamente (ex: a cada 200 ms) enquanto o botão estiver
// pressionado.

#include <Arduino.h>

// ===================== PINOS =====================

// --- Lado Esquerdo (LEFT) ---
const int PWMAB_LEFT = 9;    // PWMA + PWMB juntos
const int IN1_LEFT   = 7;    // AIN1 + BIN1 juntos
const int IN2_LEFT   = 8;    // AIN2 + BIN2 juntos
const int STBY_LEFT  = 4;    // STBY do driver esquerdo

// --- Lado Direito (RIGHT) ---
const int PWMAB_RIGHT = 10;  // PWMA + PWMB juntos
const int IN1_RIGHT   = 5;   // AIN1 + BIN1 juntos
const int IN2_RIGHT   = 6;   // AIN2 + BIN2 juntos
const int STBY_RIGHT  = 2;   // STBY do driver direito

// ===================== CONFIG =====================
const unsigned long TIMEOUT_MS = 600;  // failsafe
int velocidadeAtual = 150;             // Velocidade padrão 0-255

bool emMovimento = false;
unsigned long ultimoComando = 0;

// ===================== FUNÇÕES AUXILIARES =====================

// --- Controle individual por lado ---

void leftFrente(int vel) {
  digitalWrite(STBY_LEFT, HIGH);  // Ativa driver
  digitalWrite(IN1_LEFT, HIGH);
  digitalWrite(IN2_LEFT, LOW);
  analogWrite(PWMAB_LEFT, vel);
}

void leftRe(int vel) {
  digitalWrite(STBY_LEFT, HIGH);  // Ativa driver
  digitalWrite(IN1_LEFT, LOW);
  digitalWrite(IN2_LEFT, HIGH);
  analogWrite(PWMAB_LEFT, vel);
}

void leftParar() {
  analogWrite(PWMAB_LEFT, 0);
  digitalWrite(IN1_LEFT, LOW);
  digitalWrite(IN2_LEFT, LOW);
  digitalWrite(STBY_LEFT, LOW);   // Standby - economiza energia
}

void leftFreio() {
  digitalWrite(STBY_LEFT, HIGH);  // Ativa driver (freio ativo precisa do driver ligado)
  digitalWrite(IN1_LEFT, HIGH);
  digitalWrite(IN2_LEFT, HIGH);
  analogWrite(PWMAB_LEFT, 255);
}

void rightFrente(int vel) {
  digitalWrite(STBY_RIGHT, HIGH); // Ativa driver
  digitalWrite(IN1_RIGHT, HIGH);
  digitalWrite(IN2_RIGHT, LOW);
  analogWrite(PWMAB_RIGHT, vel);
}

void rightRe(int vel) {
  digitalWrite(STBY_RIGHT, HIGH); // Ativa driver
  digitalWrite(IN1_RIGHT, LOW);
  digitalWrite(IN2_RIGHT, HIGH);
  analogWrite(PWMAB_RIGHT, vel);
}

void rightParar() {
  analogWrite(PWMAB_RIGHT, 0);
  digitalWrite(IN1_RIGHT, LOW);
  digitalWrite(IN2_RIGHT, LOW);
  digitalWrite(STBY_RIGHT, LOW);  // Standby - economiza energia
}

void rightFreio() {
  digitalWrite(STBY_RIGHT, HIGH); // Ativa driver (freio ativo precisa do driver ligado)
  digitalWrite(IN1_RIGHT, HIGH);
  digitalWrite(IN2_RIGHT, HIGH);
  analogWrite(PWMAB_RIGHT, 255);
}

// --- Controle combinado (ambos os lados) ---

void frente(int vel) {
  leftFrente(vel);
  rightFrente(vel);
}

void re(int vel) {
  leftRe(vel);
  rightRe(vel);
}

void parar() {
  leftParar();
  rightParar();
}

void freio() {
  leftFreio();
  rightFreio();
}

void girarEsquerda(int vel) {
  leftRe(vel);       // esquerdo para trás
  rightFrente(vel);  // direito para frente
}

void girarDireita(int vel) {
  leftFrente(vel);   // esquerdo para frente
  rightRe(vel);      // direito para trás
}

// ===================== SERIAL / COMANDOS =====================

char buffer[16];
byte bufLen = 0;

void executarComando(const char* cmd) {
  bool ok = true;

  switch (cmd[0]) {
    case 'F': frente(velocidadeAtual);        emMovimento = true;  break;
    case 'B': re(velocidadeAtual);            emMovimento = true;  break;
    case 'L': girarEsquerda(velocidadeAtual); emMovimento = true;  break;
    case 'R': girarDireita(velocidadeAtual);  emMovimento = true;  break;
    case 'S': parar();                        emMovimento = false; break;
    case 'X': freio();                        emMovimento = false; break;
    case 'V':
      if (cmd[1] == '\0') { ok = false; break; }
      velocidadeAtual = constrain(atoi(cmd + 1), 0, 255);
      break;
    default:
      ok = false;
  }

  ultimoComando = millis();
  Serial.print(ok ? F("OK ") : F("ERR "));
  Serial.println(cmd);
}

void lerSerial() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (bufLen > 0) {
        buffer[bufLen] = '\0';
        executarComando(buffer);
        bufLen = 0;
      }
    } else if (bufLen < sizeof(buffer) - 1) {
      buffer[bufLen++] = c;
    }
  }
}

// ===================== SETUP =====================

void setup() {
  Serial.begin(115200);

  // Pinos de direção - LEFT
  pinMode(IN1_LEFT, OUTPUT);
  pinMode(IN2_LEFT, OUTPUT);
  pinMode(STBY_LEFT, OUTPUT);
  pinMode(PWMAB_LEFT, OUTPUT);

  // Pinos de direção - RIGHT
  pinMode(IN1_RIGHT, OUTPUT);
  pinMode(IN2_RIGHT, OUTPUT);
  pinMode(STBY_RIGHT, OUTPUT);
  pinMode(PWMAB_RIGHT, OUTPUT);

  // Timer1 (pinos 9 e 10) sem prescaler: PWM ~31 kHz, fora da faixa audível
  // (o padrão é ~490 Hz, que faz os motores chiarem). Não afeta millis().
  TCCR1B = (TCCR1B & 0b11111000) | 0x01;

  // Começa parado
  parar();

  Serial.println(F("RC-CAR PRONTO"));
}

// ===================== LOOP - AGUARDANDO COMANDOS =====================

void loop() {
  lerSerial();

  if (emMovimento && millis() - ultimoComando > TIMEOUT_MS) {
    parar();
    emMovimento = false;
    Serial.println(F("TIMEOUT"));
  }
}
