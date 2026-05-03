#include <Bluepad32.h>

// --- Definição dos pinos dos motores ---
#define MOTOR_ESQUERDO_PIN1 32 
#define MOTOR_ESQUERDO_PIN2 33 
#define PWM_ESQUERDO 12

#define MOTOR_DIREITO_PIN1  25 
#define MOTOR_DIREITO_PIN2  26 
#define PWM_DIREITO  13

// --- Configuração do PWM ---
#define FREQ_PWM 5000
#define RESOLUCAO 8
#define CANAL_ESQUERDO 0
#define CANAL_DIREITO 1

// Variável para armazenar o ponteiro do controle conectado
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// --- Callbacks da Biblioteca ---
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("Controle conectado no slot %d\n", i);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("Controle desconectado do slot %d\n", i);
      myControllers[i] = nullptr;
      pararMotores(); // Segurança
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Configurar pinos de direção
  pinMode(MOTOR_ESQUERDO_PIN1, OUTPUT);
  pinMode(MOTOR_ESQUERDO_PIN2, OUTPUT);
  pinMode(MOTOR_DIREITO_PIN1, OUTPUT);
  pinMode(MOTOR_DIREITO_PIN2, OUTPUT);

  // Configuração de PWM (Utilizando o método tradicional do ESP32)
  ledcSetup(CANAL_ESQUERDO, FREQ_PWM, RESOLUCAO);
  ledcAttachPin(PWM_ESQUERDO, CANAL_ESQUERDO);
  ledcSetup(CANAL_DIREITO, FREQ_PWM, RESOLUCAO);
  ledcAttachPin(PWM_DIREITO, CANAL_DIREITO);

  // Inicializa o Bluepad32
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys(); // Opcional: limpa pareamentos antigos para evitar conflitos
  
  Serial.println("Aguardando conexão do controle...");
}

void loop() {
  // Atualiza os dados do Bluepad32
  BP32.update();

  // Processa o primeiro controle encontrado
  ControllerPtr myController = myControllers[0];

  if (myController && myController->isConnected()) {
    // No Bluepad32, os valores dos analógicos costumam ir de -512 a 511 ou -1024 a 1023
    // O PS4 especificamente costuma retornar até 511
    int y = myController->axisY();  // Stick Esquerdo Y
    int x = myController->axisRX(); // Stick Direito X

    // Zona morta ajustada para a resolução do Bluepad (aproximadamente 50)
    if (y < -50) { // No Bluepad, Negativo costuma ser PARA CIMA
       avancar(map(abs(y), 50, 511, 0, 255));
    } 
    else if (y > 50) { // Positivo costuma ser PARA BAIXO
       recuar(map(y, 50, 511, 0, 255));
    } 
    else if (x > 50) {
       virarDireita(map(x, 50, 511, 0, 255));
    } 
    else if (x < -50) {
       virarEsquerda(map(abs(x), 50, 511, 0, 255));
    } 
    else {
      pararMotores();
    }
  } else {
    pararMotores();
  }

  delay(10);
}

// --- Funções de Controle permanecem idênticas ---
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

