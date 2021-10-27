// Teste de 2 interrupções por temporizadores

// Biblioteca para controle dos temporizadores
#include <driver/timer.h>
// Biblioteca para controle das GPIOs
#include <driver/gpio.h>

#define led1 GPIO_NUM_4
#define led2 GPIO_NUM_5

char *tag = "teste";

// Variável auxiliar 
unsigned int i = 1, j = 1;


// Rotina de serviço de interrupção 1
void IRAM_ATTR isr_callback_1(void *args) {

  // Alternância do LED
  if(i%2){
    gpio_set_level(led1, 1);
  }else{
    gpio_set_level(led1, 0);
  }
  
  i++;

  // Finaliza a interrupção 
  TIMERG0.int_clr_timers.t0 = 1;  
  TIMERG0.hw_timer[0].update=1;
  TIMERG0.hw_timer[0].config.alarm_en = 1;

}

// Rotina de serviço de interrupção 2
void IRAM_ATTR isr_callback_2(void *args) {

  // Alternância do LED
  if(j%2){
    gpio_set_level(led2, 1);
  }else{
    gpio_set_level(led2, 0);
  }
  
  j++;

  // Finaliza a interrupção
  TIMERG0.int_clr_timers.t1 = 1;  
  TIMERG0.hw_timer[1].update=1;
  TIMERG0.hw_timer[1].config.alarm_en = 1;

}

void setup() {

  gpio_pad_select_gpio(led1);
  gpio_set_direction(led1, GPIO_MODE_OUTPUT);
  gpio_set_level(led1, 0);

  // Configuração
  timer_config_t timer_config = {};
  timer_config.divider = 80;  // O clock está funcionando a 80 MHz
  timer_config.counter_dir = TIMER_COUNT_UP; // Contagem ascendente
  timer_config.counter_en = TIMER_PAUSE;
  timer_config.alarm_en = TIMER_ALARM_EN;
  timer_config.auto_reload = TIMER_AUTORELOAD_EN;


  // Temporizador 1
  
  timer_init(TIMER_GROUP_0, TIMER_0, &timer_config);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMER_BASE_CLK/80); 
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, &isr_callback_1, &tag, 2, NULL);

  // Temporizador 2
  
  timer_init(TIMER_GROUP_0, TIMER_1, &timer_config);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_1, 0);
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_1, TIMER_BASE_CLK/800); 
  timer_enable_intr(TIMER_GROUP_0, TIMER_1);
  timer_isr_register(TIMER_GROUP_0, TIMER_1, &isr_callback_2, &tag, 5, NULL);

  // Led 1
  
  gpio_pad_select_gpio(led1);
  gpio_set_direction(led1, GPIO_MODE_OUTPUT);
  gpio_set_level(led1, 0);

  // Led 2
  
  gpio_pad_select_gpio(led2);
  gpio_set_direction(led2, GPIO_MODE_OUTPUT);
  gpio_set_level(led2, 0);
  
  // Inicialização dos temporizadores
  
  timer_start(TIMER_GROUP_0, TIMER_0);
  timer_start(TIMER_GROUP_0, TIMER_1);

}


void loop() {

}
