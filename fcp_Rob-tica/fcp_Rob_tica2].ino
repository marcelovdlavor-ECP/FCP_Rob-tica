#include <Bluepad32.h>

// CONFIGURAÇÃO DAS 2 PONTES H BTS7960
#define RPWM_ESQ 12
#define LPWM_ESQ 13
#define RPWM_DIR 27
#define LPWM_DIR 14

// CONFIGURAÇÃO DOS LEDS DE STATUS
#define LED_AZUL_PIN 2
#define LED_VERMELHO_PIN 4

// PWM ESP32
#define FREQUENCIA_PWM 1000
#define RESOLUCAO_PWM 8
#define CANAL_RPWM_ESQ 0
#define CANAL_LPWM_ESQ 1
#define CANAL_RPWM_DIR 2
#define CANAL_LPWM_DIR 3

ControllerPtr controle = nullptr;
// Variáveis para o controle dos LEDs sem travar o código (Máquina de Estados)
enum EstadoLed { LED_PARADO, PISCAR_AZUL, PISCAR_VERMELHO };
EstadoLed estadoAtualLed = LED_PARADO;
unsigned long tempoLedAnterior = 0;
int passoSequenciaLed = 0;

// FUNÇÕES DE CONTROLE DE MOTORES

void pararMotores() {
    ledcWrite(CANAL_RPWM_ESQ, 0);
    ledcWrite(CANAL_LPWM_ESQ, 0);
    ledcWrite(CANAL_RPWM_DIR, 0);
    ledcWrite(CANAL_LPWM_DIR, 0);
}

void controlarMotores(ControllerPtr ctl) {
    // --- MOTOR ESQUERDO (Controlado por L1 e L2) ---
    if (ctl->l1()) { // Frente
        ledcWrite(CANAL_RPWM_ESQ, 255);
        ledcWrite(CANAL_LPWM_ESQ, 0);
    }
    else if (ctl->l2()) { // Trás
        ledcWrite(CANAL_RPWM_ESQ, 0);
        ledcWrite(CANAL_LPWM_ESQ, 255); 
    }
    else {
        ledcWrite(CANAL_RPWM_ESQ, 0);
        ledcWrite(CANAL_LPWM_ESQ, 0);
    }

    // --- MOTOR DIREITO (Controlado por R1 e R2) ---
    if (ctl->r1()) { // Frente
        ledcWrite(CANAL_RPWM_DIR, 255);
        ledcWrite(CANAL_LPWM_DIR, 0);
    }
    else if (ctl->r2()) { // Trás
        ledcWrite(CANAL_RPWM_DIR, 0);
        ledcWrite(CANAL_LPWM_DIR, 255);
    }
    else {
        ledcWrite(CANAL_RPWM_DIR, 0);
        ledcWrite(CANAL_LPWM_DIR, 0);
    }
}
// GERENCIADOR DOS LEDS (NON-BLOCKING)

void gerenciarLedsPiscarem() {
    if (estadoAtualLed == LED_PARADO) return;

    unsigned long tempoAtual = millis();
    // Sequência para o LED Azul (Conectado)
    if (estadoAtualLed == PISCAR_AZUL) {
        if (passoSequenciaLed == 0) {
            digitalWrite(LED_AZUL_PIN, HIGH);
            tempoLedAnterior = tempoAtual;
            passoSequenciaLed = 1;
        } 
        else if (passoSequenciaLed == 1 && (tempoAtual - tempoLedAnterior >= 2000)) { // 2 Segundos Aceso
            digitalWrite(LED_AZUL_PIN, LOW);
            tempoLedAnterior = tempoAtual;
            passoSequenciaLed = 2;
        } 
        else if (passoSequenciaLed == 2 && (tempoAtual - tempoLedAnterior >= 500)) {  // 0.5 Segundo Apagado
            digitalWrite(LED_AZUL_PIN, HIGH);
            tempoLedAnterior = tempoAtual;
            passoSequenciaLed = 3;
        } 
        else if (passoSequenciaLed == 3 && (tempoAtual - tempoLedAnterior >= 2000)) { // 2 Segundos Aceso
            digitalWrite(LED_AZUL_PIN, LOW);
            estadoAtualLed = LED_PARADO; // Fim da animação
        }
    }
    // Sequência para o LED Vermelho (Desconectado)
    else if (estadoAtualLed == PISCAR_VERMELHO) {
        if (passoSequenciaLed == 0) {
            digitalWrite(LED_VERMELHO_PIN, HIGH);
            tempoLedAnterior = tempoAtual;
            passoSequenciaLed = 1;
        } 
        else if (passoSequenciaLed == 1 && (tempoAtual - tempoLedAnterior >= 2000)) { // 2 Segundos Aceso
            digitalWrite(LED_VERMELHO_PIN, LOW);
            tempoLedAnterior = tempoAtual;
            passoSequenciaLed = 2;
        } 
        else if (passoSequenciaLed == 2 && (tempoAtual - tempoLedAnterior >= 500)) {  // 0.5 Segundo Apagado
            digitalWrite(LED_VERMELHO_PIN, HIGH);
            tempoLedAnterior = tempoAtual;
            passoSequenciaLed = 3;
        } 
        else if (passoSequenciaLed == 3 && (tempoAtual - tempoLedAnterior >= 2000)) { // 2 Segundos Aceso
            digitalWrite(LED_VERMELHO_PIN, LOW);
            estadoAtualLed = LED_PARADO; // Fim da animação
        }
    }
}
// CALLBACKS DO BLUEPAD32

void onConnectedController(ControllerPtr ctl) {
    if (controle == nullptr) {
        controle = ctl;
        Serial.println("Controle conectado!");
        // Ativa a sequência do LED Azul sem travar o código
        estadoAtualLed = PISCAR_AZUL;
        passoSequenciaLed = 0;
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    if (controle == ctl) {
        controle = nullptr;
        pararMotores();
        Serial.println("Controle desconectado!");
        // Ativa a sequência do LED Vermelho sem travar o código
        estadoAtualLed = PISCAR_VERMELHO;
        passoSequenciaLed = 0;
    }
}
// SETUP

void setup() {
    Serial.begin(115200);
    pinMode(LED_AZUL_PIN, OUTPUT);
    pinMode(LED_VERMELHO_PIN, OUTPUT);
    digitalWrite(LED_AZUL_PIN, LOW);
    digitalWrite(LED_VERMELHO_PIN, LOW);
    // CONFIGURA PWM MOTOR ESQUERDO
    ledcSetup(CANAL_RPWM_ESQ, FREQUENCIA_PWM, RESOLUCAO_PWM);
    ledcAttachPin(RPWM_ESQ, CANAL_RPWM_ESQ);
    ledcSetup(CANAL_LPWM_ESQ, FREQUENCIA_PWM, RESOLUCAO_PWM);
    ledcAttachPin(LPWM_ESQ, CANAL_LPWM_ESQ);
    // CONFIGURA PWM MOTOR DIREITO
    ledcSetup(CANAL_RPWM_DIR, FREQUENCIA_PWM, RESOLUCAO_PWM);
    ledcAttachPin(RPWM_DIR, CANAL_RPWM_DIR);
    ledcSetup(CANAL_LPWM_DIR, FREQUENCIA_PWM, RESOLUCAO_PWM);
    ledcAttachPin(LPWM_DIR, CANAL_LPWM_DIR);

    pararMotores();

    BP32.setup(&onConnectedController, &onDisconnectedController);
    Serial.println("Aguardando controle Bluetooth...");
}
// LOOP PRINCIPAL

void loop() {
    BP32.update();
    if (controle && controle->isConnected()) {
        controlarMotores(controle);
    }
    // Executa o piscar dos LEDs de forma independente e contínua
    gerenciarLedsPiscarem();

    delay(1);
}