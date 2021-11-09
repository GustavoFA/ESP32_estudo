// Código do Receive

// ESP-NOW Espressif : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html

// Protocolo de comunicação ESP-NOW (Espressif)
// Fazemos uma comunicação entre dois dispositivos sem a conexão fixa de WiFi entre eles
// Utilizamos o código MAC para fazer essa ponte de envio de dados

#include <WiFi.h>
#include <esp_now.h>


// Estrutura exemplo para receber os dados (deve ser semelhante à struct do sender)
typedef struct mensagem{
  uint8_t velocidade;
  char direcao;
  bool chave;
}mensagem;

// Criação da struct
mensagem msg;

// Função de callback que é executada quando recebe algum dado
// No Docs da Espressif não é recomendado que está função faça operações longas
void func_recebimento(const uint8_t * mac, const uint8_t *incomingData, int len){
  // Copia as variáveis da struct enviada para a struct criada nesse código
  memcpy(&msg, incomingData, sizeof(msg));
  // Prints para verificar tal funcionamento
  Serial.println(msg.velocidade);
  Serial.println(msg.direcao);
  Serial.println(msg.chave);
  
}

void setup() {
  
  // Serial para verificão do funcionamento
  Serial.begin(115200);
  
  // WiFi Station podendo se conectar a qualquer fonte Access point
  // No caso, utilizando ESP-NOW não precisamo deixar nosso WiFi conectado a de outro dispositivo
  WiFi.mode(WIFI_STA);

  // Inicialização do ESP NOW
  if(esp_now_init() != ESP_OK){
    Serial.println("Erro na inicializacao");
  }

  // Função de registro de uma função callback para quando receber algum dado pela comunicação ESPNOW
  // esp_err_t esp_now_register_recv_cb(esp_now_recv_cb_t cb)
  /* Retorna:
   * ESP_OK -> Recebeu com sucesso
   * ESP_ERR_ESPNOW_NOT_INIT -> ESPNOW não está iniciado
   * ESP_ERR_ESPNOW_INTERNAL -> Erro interno
   
     cb -> Função callback 
   */
  // Faço uma checagem do recebimento, para se caso houver algum problema sabermos do que se trata
  if(esp_now_register_recv_cb(func_recebimento) == ESP_OK){
    Serial.println("Mensagem recebida com sucesso \n\n");
  }else if(esp_now_register_recv_cb(func_recebimento) == ESP_ERR_ESPNOW_NOT_INIT){
    Serial.println("ESPNOW nao foi inicializado");
  }else{
    Serial.println("Erro interno");
  }

}

void loop() {

}
