// Comunicação Sender

#include <esp_now.h>
#include <WiFi.h>

typedef struct mensagem{
  uint8_t velocidade;
  char direcao;
  bool chave;
}mensagem;

mensagem msg;

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

unsigned int lastTime;
const int intervalo = 1000;

void setup(){

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if(esp_now_init() != ESP_OK){
    Serial.println("Erro na inicializacao");
  }

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  lastTime = millis();
  
}


void loop(){

  // Envio das variáveis de controle
  if(millis() - lastTime >= intervalo){

    // Pega as variáveis armazenadas no controle e guarda na struct para serem enviadas
    msg.velocidade = 3;
    msg.direcao = 'f';
    msg.chave = false;

    esp_err_t envio_status = esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));

    if (envio_status == ESP_OK) {
      Serial.println("Mensagem enviada com sucesso");
    }
    else {
      Serial.println("Falha no envio da mensagem");
    }

    lastTime = millis();
  }


  
}
