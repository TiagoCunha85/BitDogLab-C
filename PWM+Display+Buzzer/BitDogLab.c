#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "ssd1306.h"  // Biblioteca para o display OLED SSD1306

// Definição de pinos
#define VRX 26 // Joystick X (não usado para navegação)
#define VRY 27 // Joystick Y
#define SW  22 // Botão do Joystick

#define LED_RGB 15 // LED RGB
#define BUZZER  14 // Buzzer

// Inicialização do display OLED
#define I2C_PORT i2c0
#define SDA_PIN 4
#define SCL_PIN 5

// Configuração do menu
const char *menu_items[] = {
    "1. Rodar Joystick LED",
    "2. Tocar Buzzer",
    "3. Ligar LED RGB"
};
const int menu_size = sizeof(menu_items) / sizeof(menu_items[0]);
int menu_index = 0; // Item do menu selecionado

// Inicializa o joystick
void setup_joystick() {
    adc_init();
    adc_gpio_init(VRY);
    gpio_init(SW);
    gpio_set_dir(SW, GPIO_IN);
    gpio_pull_up(SW);
}

// Leitura do eixo Y do joystick
int read_joystick_y() {
    adc_select_input(1);
    return adc_read();
}

// Configuração do display OLED
ssd1306_t oled;

void setup_oled() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Inicialização correta do display
    SSD1306_init(&oled);  // Corrigido para usar SSD1306_init com o nome correto
}

// Exibe o menu na tela
void display_menu() {
    SSD1306_clear(&oled);  // Alterado para SSD1306_clear
    for (int i = 0; i < menu_size; i++) {
        if (i == menu_index) {
            SSD1306_draw_text(&oled, 0, i * 10, ">"); // Cursor
        }
        SSD1306_draw_text(&oled, 10, i * 10, menu_items[i]);
    }
    SSD1306_show(&oled);  // Alterado para SSD1306_show
}

// Função para processar a seleção do menu
void execute_menu_action() {
    switch (menu_index) {
        case 0:
            printf("Executando Joystick LED...\n");
            // Chamar código do joystick LED
            break;
        case 1:
            printf("Tocando Buzzer...\n");
            gpio_put(BUZZER, 1);
            sleep_ms(500);
            gpio_put(BUZZER, 0);
            break;
        case 2:
            printf("Ligando LED RGB...\n");
            gpio_put(LED_RGB, 1);
            sleep_ms(500);
            gpio_put(LED_RGB, 0);
            break;
    }
}

// Loop principal
int main() {
    stdio_init_all();
    setup_oled();
    setup_joystick();
    gpio_init(LED_RGB);
    gpio_set_dir(LED_RGB, GPIO_OUT);
    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);

    display_menu();

    while (1) {
        int joy_y = read_joystick_y();
        if (joy_y < 1000 && menu_index > 0) {
            menu_index--;
            display_menu();
            sleep_ms(200);
        } else if (joy_y > 3000 && menu_index < menu_size - 1) {
            menu_index++;
            display_menu();
            sleep_ms(200);
        }

        if (gpio_get(SW) == 0) {
            execute_menu_action();
            sleep_ms(500);
        }
    }
}
