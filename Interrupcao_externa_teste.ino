// Teste de 2 interrupções externas


#include <driver/gpio.h>


#define bot1 GPIO_NUM_18
#define bot2 GPIO_NUM_19
#define led1 GPIO_NUM_4 // Verde
#define led2 GPIO_NUM_5 // Branco

uint32_t i = 1, j = 1;

char *tag = "teste";

// ISR 
void isr_function_1(void *arg){

  i++;
  if(i%2){
    gpio_set_level(led1, 1);
  }else{
    gpio_set_level(led1, 0);
  }

  uint32_t gpio_intr_status = READ_PERI_REG(GPIO_STATUS_REG); //
  SET_PERI_REG_MASK(GPIO_STATUS_W1TC_REG, gpio_intr_status); 
}

// ISR 
void isr_function_2(void *arg){

  j++;
  if(j%2){
    gpio_set_level(led2, 1);
  }else{
    gpio_set_level(led2, 0);
  }

  uint32_t gpio_intr_status = READ_PERI_REG(GPIO_STATUS_REG); 
  SET_PERI_REG_MASK(GPIO_STATUS_W1TC_REG, gpio_intr_status); 
}

void setup() {

  // Botão 1
  gpio_pad_select_gpio(bot1);
  gpio_set_direction(bot1, GPIO_MODE_INPUT);
  gpio_set_pull_mode(bot1, GPIO_PULLUP_ONLY);
  gpio_set_intr_type(bot1, GPIO_INTR_NEGEDGE);
  gpio_intr_enable(bot1);
  gpio_install_isr_service(ESP_INTR_FLAG_LEVEL2);
  gpio_isr_handler_add(bot1, isr_function_1, tag);

  // Led 1
  gpio_pad_select_gpio(led1);
  gpio_set_direction(led1, GPIO_MODE_OUTPUT);
  gpio_set_level(led1, 0);
  
  // Botão 2
  gpio_pad_select_gpio(bot2);
  gpio_set_direction(bot2, GPIO_MODE_INPUT);
  gpio_set_pull_mode(bot2, GPIO_PULLUP_ONLY);
  gpio_set_intr_type(bot2, GPIO_INTR_NEGEDGE);
  gpio_intr_enable(bot2);
  gpio_install_isr_service(ESP_INTR_FLAG_LEVEL5);
  gpio_isr_handler_add(bot2, isr_function_2, tag);

  // Led 2
  gpio_pad_select_gpio(led2);
  gpio_set_direction(led2, GPIO_MODE_OUTPUT);
  gpio_set_level(led2, 0);


}


void loop() {   

}
