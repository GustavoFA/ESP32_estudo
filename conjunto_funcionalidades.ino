// Junção de funcionalidades: PWM, ADC, interrupção externa, interna e WiFi(AP e WebServer)

// Biblioteca para GPIO ESP32
#include <driver/gpio.h>
// Biblioteca para ADC ESP32
#include <driver/adc.h>
// Biblioteca para PWM ESP32
#include <driver/mcpwm.h>
// Biblioteca para Temporizador
#include <driver/timer.h>
// Biblioteca para Wifi
#include <WiFi.h>


#define input_gpio GPIO_NUM_18
#define gpio_pwm0a GPIO_NUM_5
#define gpio_pwm0b GPIO_NUM_4
#define led GPIO_NUM_2

// Credenciais para acesso do WiFi ESP32
const char* ssid     = "ESP32-Access-Point";

// Variável para armazenar o HTTP request
String header;

// Variável auxiliar para guardar o estado do led
String ledState = "off";

char *tag_ = "teste";
char *tag = "teste";
int val = 0;

// Número da porta do Web Server
WiFiServer server(80);

// ISR
void isr_function(void *arg){

  if(!gpio_get_level(input_gpio)){  // Muda o duty cycle dos LED_PWM

      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 100);
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 100);

  }
  uint32_t gpio_intr_status = READ_PERI_REG(GPIO_STATUS_REG);
  SET_PERI_REG_MASK(GPIO_STATUS_W1TC_REG, gpio_intr_status); 
}

// Rotina de serviço de interrupção
void IRAM_ATTR isr_callback(void *args) {
  
  val = adc1_get_raw(ADC1_CHANNEL_0);
  
  if(val <= 1000){
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 20);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 20);
  }
  if(val <= 2000 && val >= 1000){
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 50);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 50);
  }
  if(val >= 3000){
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 80);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 80);
  }
  

  // Finaliza a interrupção
  TIMERG0.int_clr_timers.t0 = 1; // Supostamente limpa o bit de interrupção --> https://www.esp32.com/viewtopic.php?t=12931
  TIMERG0.hw_timer[0].update=1;
  TIMERG0.hw_timer[0].config.alarm_en = 1;
}

// Função para WebServer com botões que interagem com a lógica do ESP32
void WebServer_function(){
  // Verifica os clientes
  WiFiClient client = server.available();  

  // Quando houver um cliente
  if (client) {                             
    String currentLine = "";                // String para armazernar dados do cliente
    while (client.connected()) {            // Loop para verificar atualizações do cliente
      if (client.available()) {             
        char c = client.read();             
        header += c;
        if (c == '\n') {      

          if (currentLine.length() == 0) {
  
            if (header.indexOf("GET /Led/on") >= 0) {
              ledState = "on";
              gpio_set_level(led, 1);
            } else if (header.indexOf("GET /Led/off") >= 0) {
              ledState = "off";
              gpio_set_level(led, 0);
            }
            
            //HTML 
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            

            client.println("<body><h1>ESP32 Web Server</h1>"); 
            client.println("<p>Led - State " + ledState + "</p>");
            // Verificação do status do Led para atualizar página       
            if (ledState=="off") {
              client.println("<p><a href=\"/Led/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Led/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c;      
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }
}


void setup() {

  gpio_set_direction(led, GPIO_MODE_OUTPUT);
  gpio_set_level(led, 0);

  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, gpio_pwm0a);  // Identifica a GPIO x para o MCPWM0A
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, gpio_pwm0b);  // Identifica a GPIO y para o MCPWM0B

  mcpwm_config_t pwm_config;

  pwm_config.frequency = 10;
  pwm_config.cmpr_a = 50;
  pwm_config.cmpr_b = 50;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
  pwm_config.counter_mode = MCPWM_UP_COUNTER;

  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);


  // esp_err_t adc1_config_width(adc_bits_width_t width_bit)
  /* Configura todos os canais do ADC 1
   * width_bit = ADC_WIDTH_X, resolução do ADC em que x = BIT_y (-> y = 9, 10, 11 e 12), MAX
   */
  adc1_config_width(ADC_WIDTH_BIT_12);

  // esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten)
  // channel = ADC1_CHANNEL_z, z = número de GPIO que está ligada ao canal 1
  // atten = ADC_ATTEN_DB_w, w = 0, 2_5, 6, 11
  /*
  +----------+-------------+-----------------+
  |          | attenuation | suggested range |
  |    SoC   |     (dB)    |      (mV)       |
  +==========+=============+=================+
  |          |       0     |    100 ~  950   |
  |          +-------------+-----------------+
  |          |       2.5   |    100 ~ 1250   |
  |   ESP32  +-------------+-----------------+
  |          |       6     |    150 ~ 1750   |
  |          +-------------+-----------------+
  |          |      11     |    150 ~ 2450   |
  +----------+-------------+-----------------+
  */
  adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11);

  // GPIOs podem ter funções diferentes de GPIO (ver tópico 4.10 do technical reference manual)
  // Definindo a função da GPIO que queremos usar como uma GPIO
  gpio_pad_select_gpio(input_gpio);

  // Selecionar nossa GPIO como OUTPUT ou INPUT ou OUTPUT
  // esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode)
  /*
   * gpio_num = GPIO_NUM_X, sendo X = número da sua GPIO 
   * mode = GPIO_MODE_Y, sendo Y = modo da sua GPIO --> DISABLE, INPUT, OUTPUT, OUTPUT_OD (open-drain mode), INPUT_OUTPUT_OD (open-drain mode) e INPUT_OUTPUT
   */
  gpio_set_direction(input_gpio, GPIO_MODE_INPUT);

  // Configurar a GPIO como pull up ou pull down
  // esp_err_t gpio_set_pull_mode(gpio_num_t gpio_num, gpio_pull_mode_t pull)  
  /*
   * gpio_num = GPIO_NUM_X, sendo X = número da sua GPIO
   * pull = GPIO_Z, sendo Z = formas de pull --> PULLUP_ONLY, PULLDOWN_ONLY, PULLUP_UP_PULLDOWN e FLOATING
   */
   
  gpio_set_pull_mode(input_gpio, GPIO_PULLUP_ONLY);

  // Selecionar o tipo de disparador (trigger)
  // esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type)
  /* 
   *  gpio_num = GPIO_NUM_X, sendo X = número da sua GPIO
   *  intr_type = GPIO_INTR_W, sendo W = forma de interrupção --> DISABLE, POSEDGE (borda de subida), NEGEDGE (borda de descida), ANYEDGE (ambas as bordas),
   *  LOW_LEVEL (dispara com entrada baixa), HIGH_LEVEL (dispara com entrada alta) e MAX.
   */
  gpio_set_intr_type(input_gpio, GPIO_INTR_NEGEDGE);

  // Habilitar o módulo de interrupção do sinal da GPIO
  // esp_err_t gpio_intr_enable(gpio_num_t gpio_num)
  /*
   * pio_num = GPIO_NUM_X, sendo X = número da sua GPIO
   */
   /* NOTAS:
    * Não usar interrupção na GPIO36 e 39 quando estiver usando ADC ou WiFi com sleep mode ativo
    */
  gpio_intr_enable(input_gpio);

  // Registrar a Rotina de serviço de interrupção (ISR)
  // esp_err_t gpio_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, gpio_isr_handle_t *handle)
  /*
   * fn = Função para a ISR
   * arg = Parametro para passar para a ISR
   * intr_alloc_flags = ESP_INTR_FLAG_Q, sendo Q = (https://github.com/pycom/esp-idf-2.0/blob/master/components/esp32/include/esp_intr_alloc.h) 
   * handle = ponteiro para retornar a ISR
   */
  gpio_isr_register(isr_function, tag, ESP_INTR_FLAG_LEVEL1, NULL);

  // Pegar o nível da GPIO
  // int gpio_get_level(gpio_num_t gpio_num)

  // Struct para configuração do timer
  timer_config_t timer_config = {};

  // Prescaler
  timer_config.divider = 80;  // O clock está funcionando a 80 MHz

  // Configura se o contador será crescente ou decrescente
  timer_config.counter_dir = TIMER_COUNT_UP; // Contagem ascendente

  // Define se iremos inicializar o temporizador ou deixar ele pausado, até chamarmos timer_start()
  /*
     TIMER_PAUSE -> Começar pausado
     TIMER_START -> Já começa contando, inicia com timer_init()
  */
  timer_config.counter_en = TIMER_PAUSE;

  // Configura se haverá alarme ou não
  /*
     TIMER_ALARM_DIS -> Sem alarme
     TIMER_ALARM_EN  -> Com alarme
  */
  timer_config.alarm_en = TIMER_ALARM_EN;

  // Configura se o temporizador irá se auto reiniciar
  /*
     TIMER_AUTORELOAD_DIS -> Desativa o auto reiniciar
     TIMER_AUTORELOAD_EN -> Ativa o auto reiniciar
  */
  timer_config.auto_reload = TIMER_AUTORELOAD_EN;

  // esp_err_t timer_init(timer_group_t group_num, timer_idx_t timer_num, const timer_config_t *config)
  /*
     group_num = TIMER_GROUP_X, x = 0, 1 e MAX | ->
     timer_num = TIMER_Z, z = 0, 1 e MAX | -> Seleciona o temporizador que será utilizado
     config = ponteiro para onde está alocado a struct criada para configuração
  */
  timer_init(TIMER_GROUP_0, TIMER_0, &timer_config);

  // Configurar o valor inicial do temporizador
  // esp_err_t timer_set_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t load_val)
  /*
     group_num = TIMER_GROUP_X, x = 0, 1 e MAX
     timer_num = TIMER_Z, z = 0, 1 e MAX
     load_val = valor a ser escrito no temporizador ao iniciá-lo
  */
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);

  // Configurar o valor do temporizador do alarme
  // esp_err_t timer_set_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t alarm_value)
  /*
     group_num = TIMER_GROUP_X, x = 0, 1 e MAX
     timer_num = TIMER_Z, z = 0, 1 e MAX
     alarm_value = valor para ocorrer a interrupção
  */
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMER_BASE_CLK/40); 

  // Ativar a interrupção de temporizador
  // esp_err_t timer_enable_intr(timer_group_t group_num, timer_idx_t timer_num)
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);

  // Configura uma ISR para a interrupção
  // esp_err_t timer_isr_register(timer_group_t group_num, timer_idx_t timer_num, void (*fn)(void *), void *arg, int intr_alloc_flags, timer_isr_handle_t *handle)
  /*
      intr_alloc_flags = prioridade da interrupção
   */
  timer_isr_register(TIMER_GROUP_0, TIMER_0, &isr_callback, &tag_, ESP_INTR_FLAG_LEVEL2, NULL);

  // Iniciar contagem do temporizador
  // esp_err_ttimer_start(timer_group_tgroup_num, timer_idx_ttimer_num)
  timer_start(TIMER_GROUP_0, TIMER_0);

  // Inicio o WiFi como modo de Ponto de acesso (AP)
  WiFi.softAP(ssid);
  
  // Inicio o servidor Web
  server.begin();

}


void loop() {   

  WebServer_function();

}
