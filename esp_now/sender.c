// Comunicação Sender

// ESP-NOW Espressif : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html

// Protocolo de comunicação ESP-NOW (Espressif)
// Fazemos uma comunicação entre dois dispositivos sem a conexão fixa de WiFi entre eles
// Utilizamos o código MAC para fazer essa ponte de envio de dados
/* Para proteger os dados enviados é utilizado o método CCMP, sendo que o WiFi possui PMK (Primary Master
Key) e um LMK (Local Master Key), sendo que ambas possuem tamanho de 16 bytes.
*/

// Farme format
//  ------------------------------------------------------------------------------------------------------------
//  | MAC Header | Category Code | Organization Identifier | Random Values | Vendor Specific Content |   FCS   |
//  ------------------------------------------------------------------------------------------------------------
//    24 bytes         1 byte              3 bytes               4 bytes             7~255 bytes        4 bytes

// Vendor Specific Content
//  -------------------------------------------------------------------------------
//  | Element ID | Length | Organization Identifier | Type | Version |    Body    |
//  -------------------------------------------------------------------------------
//      1 byte     1 byte            3 bytes         1 byte   1 byte   0~250 bytes

// O Body do Vendor Specific Content contém os dados que pretendemos enviar

#include <esp_now.h>
#include <WiFi.h>

// Estrutura utilizada para o envio das variáveis (deve ser semelhante a struct que recebe)
typedef struct mensagem{
  uint8_t velocidade;
  char direcao;
  bool chave;
}mensagem;

// Criação da struct
mensagem msg;

// Endereço MAC utilizado para fazer a comunicação
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Atualize esta variável
// Para descobrir seu Mac Address faça o seguinte:

/*
 * Serial.begin(115200);
 * WiFi.mode(WIFI_MODE_STA);
 * Serial.println(WiFi.macAddress());
 */

// Variáveis para marcação de tempo, e assim, permitir que o envio seja feito a cada 1s (esse tempo pode ser menor)
unsigned int lastTime;
const int intervalo = 1000;

void setup(){

  // Serial para verificação do funcionamento
  Serial.begin(115200);

  // Fica no modo WiFi Station, e assim, pode se conectar a qualquer dispositivo
  WiFi.mode(WIFI_STA);

  // Inicio o ESP NOW
  if(esp_now_init() != ESP_OK){
    Serial.println("Erro na inicializacao");
  }else{
    ESP.restart(); // Caso não funcione, reinicio o ESP
  }

  // Informações do par a ser enviado os dados
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  // podemos adicionar uma chave de segurança na conexão
  uint8_t key[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  memcpy(peerInfo.lmk, key, 6); 
  peerInfo.channel = 0;
  peerInfo.encrypt = true;
  // ---

  // Adiciono o par especificado e valido tal conexão
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Falha no pareamento");
    return;
  }

  // Começo a marcar o tempo para futuramente enviar os dados a cada 1 segundo
  lastTime = millis();
  
}


void loop(){

  // Envio das variáveis a cada 1 segundo
  if(millis() - lastTime >= intervalo){

    // Atualizo as variáveis (isso poderia ser feito em outra função, por exemplo, por um controle ou sensor)
    msg.velocidade = 3;
    msg.direcao = 'f';
    msg.chave = false;

    // esp_err_t esp_now_send(const uint8_t *peer_addr, const uint8_t *data, size_t len)

    // Utilizo envio_status para checar se o envio dos dados ocorreu com sucesso
    esp_err_t envio_status = esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));

    if (envio_status == ESP_OK) {
      Serial.println("Mensagem enviada com sucesso");
    }
    else {
      Serial.println("Falha no envio da mensagem");
    }

    // Armazeno o último tempo depois de concluir o envio
    lastTime = millis();
  }


  
}
