// Código do Receive

#include <WiFi.h>
#include <esp_now.h>

typedef struct mensagem{
  uint8_t velocidade;
  char direcao;
  bool chave;
}mensagem;

mensagem msg_recebida;

void func_recebimento(const uint8_t * mac, const uint8_t *incomingData, int len){

  memcpy(&msg_recebida, incomingData, sizeof(msg_recebida));
  Serial.println(msg_recebida.velocidade);
  Serial.println(msg_recebida.direcao);
  Serial.println(msg_recebida.chave);
  
}

void setup() {
  
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if(esp_now_init() != ESP_OK){
    Serial.println("Erro na inicializacao");
  }

  esp_now_register_recv_cb(func_recebimento);

}

void loop() {

}
