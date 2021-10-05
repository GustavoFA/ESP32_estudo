// Temporizador e Interrupção Interna ESP32
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/timer.html


/* NOTAS:
 * função timer_isr_callback_add gera um erro no Arduino IDE falando que ela não foi declarada
 * função timer_group_intr_clr_in_isr gera um erro no Arduino IDE falando que ela não foi decladara
*/ 

// Referência:
// https://github.com/espressif/esp-idf/blob/3e370c4296247b349aa3b9a0076c05b9946d47dc/examples/peripherals/timer_group/main/timer_group_example_main.c

// Biblioteca para controle dos temporizadores
#include <driver/timer.h>
// Biblioteca para controle das GPIOs
#include <driver/gpio.h>

char *tag = "teste";


// Rotina de serviço de interrupção
void IRAM_ATTR isr_callback(void *args) {

  gpio_set_level(GPIO_NUM_2, 0);

  // Finaliza a interrupção
  TIMERG0.int_clr_timers.t0 = 1; // Supostamente limpa o bit de interrupção --> https://www.esp32.com/viewtopic.php?t=12931

}

void setup() {

  // Configurar GPIO para validar interrupção
  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
  gpio_set_level(GPIO_NUM_2, 1);


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
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMER_BASE_CLK/80); 

  // Ativar a interrupção de temporizador
  // esp_err_t timer_enable_intr(timer_group_t group_num, timer_idx_t timer_num)
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);

  // Configura uma ISR para a interrupção
  // esp_err_t timer_isr_register(timer_group_t group_num, timer_idx_t timer_num, void (*fn)(void *), void *arg, int intr_alloc_flags, timer_isr_handle_t *handle)
  /*
      intr_alloc_flags = prioridade da interrupção
   */
  timer_isr_register(TIMER_GROUP_0, TIMER_0, &isr_callback, &tag, 2, NULL);

  // Iniciar contagem do temporizador
  // esp_err_ttimer_start(timer_group_tgroup_num, timer_idx_ttimer_num)
  timer_start(TIMER_GROUP_0, TIMER_0);

}


void loop() {

}
