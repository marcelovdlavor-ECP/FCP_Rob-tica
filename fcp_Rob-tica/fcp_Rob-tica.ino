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

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// Função para parar tudo
void pararMotores() {
    ledcWrite(CANAL_ESQUERDO, 0);
    ledcWrite(CANAL_DIREITO, 0);
    digitalWrite(MOTOR_ESQUERDO_PIN1, LOW);
    digitalWrite(MOTOR_ESQUERDO_PIN2, LOW);
    digitalWrite(MOTOR_DIREITO_PIN1, LOW);
    digitalWrite(MOTOR_DIREITO_PIN2, LOW);
}

void processGamepad(ControllerPtr ctl) {
    // --- 1. EFEITOS DE LED E RUMBLE (Seu código original) ---

    if (ctl->b()) {
        static int led = 0;
        led++;
        ctl->setPlayerLEDs(led & 0x0f);
    }

    if (ctl->x()) {
        ctl->playDualRumble(0, 250, 0x80, 0x40);
    }

    // --- 2. LÓGICA DE MOVIMENTAÇÃO (CONTROLE TIPO TANQUE) ---

    // MOTOR ESQUERDO (L1 Frente / L2 Trás)
    if (ctl->l1()) {
        digitalWrite(MOTOR_ESQUERDO_PIN1, HIGH);
        digitalWrite(MOTOR_ESQUERDO_PIN2, LOW);
        ledcWrite(CANAL_ESQUERDO, 255);
    } 
    else if (ctl->l2()) {
        digitalWrite(MOTOR_ESQUERDO_PIN1, LOW);
        digitalWrite(MOTOR_ESQUERDO_PIN2, HIGH);
        ledcWrite(CANAL_ESQUERDO, 255);
    } 
    else {
        digitalWrite(MOTOR_ESQUERDO_PIN1, LOW);
        digitalWrite(MOTOR_ESQUERDO_PIN2, LOW);
        ledcWrite(CANAL_ESQUERDO, 0);
    }

    // MOTOR DIREITO (R1 Frente / R2 Trás)
    if (ctl->r1()) {
        digitalWrite(MOTOR_DIREITO_PIN1, HIGH);
        digitalWrite(MOTOR_DIREITO_PIN2, LOW);
        ledcWrite(CANAL_DIREITO, 255);
    } 
    else if (ctl->r2()) {
        digitalWrite(MOTOR_DIREITO_PIN1, LOW);
        digitalWrite(MOTOR_DIREITO_PIN2, HIGH);
        ledcWrite(CANAL_DIREITO, 255);
    } 
    else {
        digitalWrite(MOTOR_DIREITO_PIN1, LOW);
        digitalWrite(MOTOR_DIREITO_PIN2, LOW);
        ledcWrite(CANAL_DIREITO, 0);
    }
}

// --- Funções obrigatórias da Bluepad32 ---

void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            myControllers[i] = nullptr;
            pararMotores();
            break;
        }
    }
}

void setup() {
    Serial.begin(115200);

    // Configuração dos Pinos
    pinMode(MOTOR_ESQUERDO_PIN1, OUTPUT);
    pinMode(MOTOR_ESQUERDO_PIN2, OUTPUT);
    pinMode(MOTOR_DIREITO_PIN1, OUTPUT);
    pinMode(MOTOR_DIREITO_PIN2, OUTPUT);

    // Setup do PWM
    ledcSetup(CANAL_ESQUERDO, FREQ_PWM, RESOLUCAO);
    ledcAttachPin(PWM_ESQUERDO, CANAL_ESQUERDO);
    ledcSetup(CANAL_DIREITO, FREQ_PWM, RESOLUCAO);
    ledcAttachPin(PWM_DIREITO, CANAL_DIREITO);

    BP32.setup(&onConnectedController, &onDisconnectedController);
}

void loop() {
    BP32.update();
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        ControllerPtr myController = myControllers[i];
        if (myController && myController->isConnected()) {
            processGamepad(myController);
        }
    }
    delay(10);
}