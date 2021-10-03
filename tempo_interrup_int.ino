// Temporizador e Interrupção Interna ESP32
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/timer.html


// Referência:
// https://github.com/espressif/esp-idf/blob/3e370c4296247b349aa3b9a0076c05b9946d47dc/examples/peripherals/timer_group/main/timer_group_example_main.c

// Biblioteca para controle dos temporizadores
#include <driver/timer.h>


void setup(){

  timer_config_t timer_config = {};

  timer_config.divider = 16;  // Divisão do clock, 160 MHz ---> 10 MHz

  
  timer_config.counter_dir = TIMER_COUNT_UP; // Contagem ascendente
  
  // Define se iremos inicializar o temporizador ou deixar ele pausado, até chamarmos timer_init()
  /*
   * TIMER_PAUSE -> Começar pausado 
   * TIMER_START -> Já começa contando
   */
  timer_config.counter_en = TIMER_PAUSE;
  
  // Configura se haverá alarme ou não
  /*
   * TIMER_ALARM_DIS -> Sem alarme
   * TIMER_ALARM_EN  -> Com alarme
   */
  timer_config.alarm_en = TIMER_ALARM_EN;

  // Configura se o temporizador irá se auto reiniciar
  timer_config.auto_reload = auto_reload;

  // esp_err_t timer_init(timer_group_t group_num, timer_idx_t timer_num, const timer_config_t *config)
  /*
   * group_num = TIMER_GROUP_X, x = 0, 1 e MAX | -> 
   * timer_num = TIMER_Z, z = 0, 1 e MAX | -> Seleciona o temporizador que será utilizado
   * config = ponteiro para onde está alocado a struct criada para configuração
   */
  timer_init(TIMER_GROUP_0, TIMER_0, &timer_config);

  // Configurar o valor inicial do temporizador
  // esp_err_t timer_set_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t load_val)
  /*
   * group_num = TIMER_GROUP_X, x = 0, 1 e MAX
   * timer_num = TIMER_Z, z = 0, 1 e MAX
   * load_val = valor a ser escrito no temporizador
   */
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);

}


void loop(){
  
}
