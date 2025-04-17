#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "ws2812.pio.h"

//Definição de GPIOs
#define JOYSTICK_X 26  // ADC0
#define JOYSTICK_Y 27  // ADC1
#define BOTAO_A 5
#define BOTAO_B 6
#define LED_RED 13
#define LED_GREEN 11
#define I2C_SDA 14
#define I2C_SCL 15
//Display SSD1306
ssd1306_t ssd;

//Função para modularizar a inicialização do hardware
void inicializar_componentes(){
    stdio_init_all();

    // Inicializa botões
    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);
    
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);

    // Inicializa LED Verde (controle por botão)
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, 0);
    // Inicializa LED Verde (controle por botão)
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 0);

    // Inicializa ADC para leitura do Joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);

    //Inicializa I2C para o display SSD1306
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);  //Dados
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);  //Clock
    //Define como resistor de pull-up interno
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Inicializa display
    ssd1306_init(&ssd, 128, 64, false, 0x3C, i2c1);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Debounce do botão (evita leituras falsas)
bool debounce_botao(uint gpio){
    static uint32_t ultimo_tempo = 0;
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    if (gpio_get(gpio) == 0 && (tempo_atual - ultimo_tempo) > 200){ // 200ms de debounce
        ultimo_tempo = tempo_atual;
        return true;
    }
    return false;
}

//Função para as chamadas de interrupções nos botões A e do Joystick
void gpio_irq_handler(uint gpio, uint32_t events){
    if(debounce_botao(BOTAO_A)){
        gpio_put(LED_RED, !gpio_get(LED_RED)); // Atualiza LED
    }
    if(debounce_botao(BOTAO_B)){
        gpio_put(LED_GREEN, !gpio_get(LED_GREEN)); // Atualiza LED
    }
}

int main(){
    inicializar_componentes(); //Inicializar GPIOs, protocolos, comunicação...
    
    //Configura as chamadas de interrupções para os botões A e do Joystick
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while(true){

    }
    return 0;
}
