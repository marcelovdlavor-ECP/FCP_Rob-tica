#include <Bluepad32.h>
// CONFIGURAÇÃO DAS PONTES H BTS7960
// -------- MOTOR ESQUERDO --------
#define RPWM_ESQ 12
#define LPWM_ESQ 13
// -------- MOTOR DIREITO --------
#define RPWM_DIR 27
#define LPWM_DIR 14
// PWM ESP32
#define FREQUENCIA_PWM 1000
#define RESOLUCAO_PWM 8

#define CANAL_RPWM_ESQ 0
#define CANAL_LPWM_ESQ 1

#define CANAL_RPWM_DIR 2
#define CANAL_LPWM_DIR 3

#define LED_STATUS_PIN 2
ControllerPtr controle = nullptr;
// FUNÇÕES DE CONTROLE DE MOTORES
void pararMotores() {
    ledcWrite(CANAL_RPWM_ESQ, 0);
    ledcWrite(CANAL_LPWM_ESQ, 0);
    ledcWrite(CANAL_RPWM_DIR, 0);
    ledcWrite(CANAL_LPWM_DIR, 0);
}

void controlarMotores(ControllerPtr ctl) {
    // MOTOR ESQUERDO
    // R1 (Frente)
    if (ctl->r1()) {
        ledcWrite(CANAL_RPWM_ESQ, 255);
        ledcWrite(CANAL_LPWM_ESQ, 0);
    }
    // R2 (Trás) - analogico/gatilho
    else if (ctl->r2()) {
        ledcWrite(CANAL_RPWM_ESQ, 0);
        ledcWrite(CANAL_LPWM_ESQ, 255); 
    }
    else {
        ledcWrite(CANAL_RPWM_ESQ, 0);
        ledcWrite(CANAL_LPWM_ESQ, 0);
    }
    // MOTOR DIREITO
    // L1 (Frente)
    if (ctl->l1()) {
        ledcWrite(CANAL_RPWM_DIR, 255);
        ledcWrite(CANAL_LPWM_DIR, 0);
    }
    // L2 (Trás) - analogico/gatilho
    else if (ctl->l2()) {
        ledcWrite(CANAL_RPWM_DIR, 0);
        ledcWrite(CANAL_LPWM_DIR, 255);
    }
    else {
        ledcWrite(CANAL_RPWM_DIR, 0);
        ledcWrite(CANAL_LPWM_DIR, 0);
    }
}

// CALLBACKS DO BLUEPAD32

void onConnectedController(ControllerPtr ctl) {
    if (controle == nullptr) {
        controle = ctl;
        Serial.println("Controle conectado!");
        digitalWrite(LED_STATUS_PIN, HIGH); // Acende o LED azul
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    if (controle == ctl) {
        controle = nullptr;
        pararMotores();
        Serial.println("Controle desconectado!");
        digitalWrite(LED_STATUS_PIN, LOW); // Apaga o LED azul
    }
}
// SETUP
void setup() {
    Serial.begin(115200);
     // Configura o pino do LED como saída e o mantém desligado inicialmente
    pinMode(LED_STATUS_PIN, OUTPUT);
    digitalWrite(LED_STATUS_PIN, LOW);
    
    // Otimização: Aumentar a velocidade do loop desabilitando logs desnecessários se possível
    // BP32.enableVirtualDevice(false);
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

    // Inicializa estado dos motores como parados
    pararMotores();

    // INICIA BLUETOOTH

    BP32.setup(&onConnectedController, &onDisconnectedController);
    Serial.println("Aguardando controle Bluetooth...");
}
// LOOP
void loop() {
    // Atualiza o estado do Bluepad32 (processa eventos Bluetooth)
    BP32.update();
    if (controle && controle->isConnected()) {
        controlarMotores(controle);
    }
    // Otimização: Reduzir o delay para 1ms ou remover, permitindo leitura mais rápida.
    // Como o ESP32 roda FreeRTOS por baixo, um delay de 1ms é suficiente para ceder processamento.
    delay(1);
}
