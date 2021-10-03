// Interrupção ESP32

// Biblioteca para GPIO ESP32
#include <driver/gpio.h>
// Biblioteca para ADC ESP32
#include <driver/adc.h>
// Biblioteca para PWM ESP32
#include "driver/mcpwm.h"

#define input_gpio GPIO_NUM_18
#define gpio_pwm0a GPIO_NUM_5
#define gpio_pwm0b GPIO_NUM_4

char *tag = "teste";
int val = 0;

// ISR
void isr_function(void *arg){

  if(!gpio_get_level(input_gpio)){  // Muda o duty cycle dos LED_PWM

      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 100);
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 100);

  }
  uint32_t gpio_intr_status = READ_PERI_REG(GPIO_STATUS_REG);
  SET_PERI_REG_MASK(GPIO_STATUS_W1TC_REG, gpio_intr_status); 
}

void setup() {

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

}


void loop() {   

  val = adc1_get_raw(ADC1_CHANNEL_0);
  
  if(val >= 1024){
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0);
  }

}
