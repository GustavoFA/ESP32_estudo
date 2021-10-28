#include <driver/adc.h>

int val = 0;

void setup() {

  Serial.begin(115200);

  // esp_err_t adc1_config_width(adc_bits_width_t width_bit)
  /* Configura todos os canais do ADC 1
   * width_bit = ADC_WIDTH_X, resolução do ADC em que x = BIT_y (-> y = 9, 10, 11 e 12), MAX
   */
  adc1_config_width(ADC_WIDTH_BIT_12);
  // Para o ADC2 não utiliza essa função, pois, ela é configurada a cada vez que você ler o valor da GPIO (logo, ela é configurada em adc2_get_raw)
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

}

void loop() {

  val = adc1_get_raw(ADC1_CHANNEL_0);

  Serial.println();
  Serial.println(val);
  Serial.println();
  
}
