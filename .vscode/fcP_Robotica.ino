void setup() {
#include <PS4Controller.h>

// Definição dos pinos dos motores
#define MOTOR_ESQUERDO_PIN1 32 
#define MOTOR_ESQUERDO_PIN2 33 
#define MOTOR_DIREITO_PIN1  25 
#define MOTOR_DIREITO_PIN2  26 

#define PWM_ESQUERDO 12
#define PWM_DIREITO  13

// Configuração do PWM
#define FREQ_PWM 5000
#define RESOLUCAO 8
#define CANAL_ESQUERDO 0
#define CANAL_DIREITO 1

void setup() {
  Serial.begin(115200);

  // Configurar pinos de direção
  pinMode(MOTOR_ESQUERDO_PIN1, OUTPUT);
  pinMode(MOTOR_ESQUERDO_PIN2, OUTPUT);
  pinMode(MOTOR_DIREITO_PIN1, OUTPUT);
  pinMode(MOTOR_DIREITO_PIN2, OUTPUT);

  // --- CONFIGURAÇÃO DE PWM (COMPATÍVEL COM ESP32 V2 e V3) ---
  // Se der erro de compilação aqui, apenas use ledcAttach(PWM_ESQUERDO, FREQ_PWM, RESOLUCAO);
  ledcAttachPin(PWM_ESQUERDO, CANAL_ESQUERDO);
  ledcSetup(CANAL_ESQUERDO, FREQ_PWM, RESOLUCAO);
  
  ledcAttachPin(PWM_DIREITO, CANAL_DIREITO);
  ledcSetup(CANAL_DIREITO, FREQ_PWM, RESOLUCAO);
  // ----------------------------------------------------------

  // Iniciar o Bluetooth com o seu MAC
  if (PS4.begin("2c:dc:d7:27:99:5d")) {
    Serial.println("Bluetooth iniciado! Ligue o controle.");
  } else {
    Serial.println("Erro ao iniciar Bluetooth.");
  }
}

void loop() {
  if (PS4.isConnected()) {
    // Pegando valores do analógico (-127 a 127)
    int y = PS4.LStickY(); 
    int x = PS4.RStickX();

    // Lógica de movimentação com zona morta de 20
    if (y > 20) {
      avancar(map(y, 20, 127, 0, 255));
    } else if (y < -20) {
      recuar(map(abs(y), 20, 127, 0, 255));
    } else if (x > 20) {
      virarDireita(map(x, 20, 127, 0, 255));
    } else if (x < -20) {
      virarEsquerda(map(abs(x), 20, 127, 0, 255));
    } else {
      pararMotores();
    }
    
    // Pequeno delay para estabilidade
    delay(10); 
  } else {
    pararMotores();
  }
}

// Funções de movimento permanecem iguais (estão corretas)
void avancar(int v) {
  digitalWrite(MOTOR_ESQUERDO_PIN1, HIGH);
  digitalWrite(MOTOR_ESQUERDO_PIN2, LOW);
  digitalWrite(MOTOR_DIREITO_PIN1, HIGH);
  digitalWrite(MOTOR_DIREITO_PIN2, LOW);
  ledcWrite(CANAL_ESQUERDO, v);
  ledcWrite(CANAL_DIREITO, v);
}

void recuar(int v) {
  digitalWrite(MOTOR_ESQUERDO_PIN1, LOW);
  digitalWrite(MOTOR_ESQUERDO_PIN2, HIGH);
  digitalWrite(MOTOR_DIREITO_PIN1, LOW);
  digitalWrite(MOTOR_DIREITO_PIN2, HIGH);
  ledcWrite(CANAL_ESQUERDO, v);
  ledcWrite(CANAL_DIREITO, v);
}

void virarEsquerda(int v) {
  digitalWrite(MOTOR_ESQUERDO_PIN1, LOW);
  digitalWrite(MOTOR_ESQUERDO_PIN2, HIGH);
  digitalWrite(MOTOR_DIREITO_PIN1, HIGH);
  digitalWrite(MOTOR_DIREITO_PIN2, LOW);
  ledcWrite(CANAL_ESQUERDO, v);
  ledcWrite(CANAL_DIREITO, v);
}

void virarDireita(int v) {
  digitalWrite(MOTOR_ESQUERDO_PIN1, HIGH);
  digitalWrite(MOTOR_ESQUERDO_PIN2, LOW);
  digitalWrite(MOTOR_DIREITO_PIN1, LOW);
  digitalWrite(MOTOR_DIREITO_PIN2, HIGH);
  ledcWrite(CANAL_ESQUERDO, v);
  ledcWrite(CANAL_DIREITO, v);
}

void pararMotores() {
  ledcWrite(CANAL_ESQUERDO, 0);
  ledcWrite(CANAL_DIREITO, 0);
}
}

void loop() {
  // put your main code here, to run repeatedly:

}
