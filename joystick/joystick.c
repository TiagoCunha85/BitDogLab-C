#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "ssd1306.h"  // Biblioteca para OLED 1306 (certifique-se de ter a biblioteca correta)

#define LEDR 12
#define LEDG 11
#define LEDB 13

// Definições de pinos do joystick
#define JOY_UP_PIN 14
#define JOY_DOWN_PIN 15
#define JOY_SELECT_PIN 16

int menu_option = 0;  // Variável para armazenar a opção selecionada

void led_rgb_put(bool r, bool g, bool b) {
    gpio_put(LEDR, r);
    gpio_put(LEDG, g);
    gpio_put(LEDB, b);
}

void setup_gpio() {
    gpio_init(LEDR);
    gpio_init(LEDG);
    gpio_init(LEDB);

    gpio_set_dir(LEDR, GPIO_OUT);
    gpio_set_dir(LEDG, GPIO_OUT);
    gpio_set_dir(LEDB, GPIO_OUT);

    // Inicialização dos pinos do joystick
    gpio_init(JOY_UP_PIN);
    gpio_init(JOY_DOWN_PIN);
    gpio_init(JOY_SELECT_PIN);
    
    gpio_set_dir(JOY_UP_PIN, GPIO_IN);
    gpio_set_dir(JOY_DOWN_PIN, GPIO_IN);
    gpio_set_dir(JOY_SELECT_PIN, GPIO_IN);
}

void display_menu(ssd1306_t *oled) {
    oled_clear(oled);
    oled_draw_text(oled, 0, 0, "Menu:");
    if (menu_option == 0) {
        oled_draw_text(oled, 0, 20, "> Blink");
        oled_draw_text(oled, 0, 40, "  Option 2");
        oled_draw_text(oled, 0, 60, "  Option 3");
    } else if (menu_option == 1) {
        oled_draw_text(oled, 0, 20, "  Blink");
        oled_draw_text(oled, 0, 40, "> Option 2");
        oled_draw_text(oled, 0, 60, "  Option 3");
    } else {
        oled_draw_text(oled, 0, 20, "  Blink");
        oled_draw_text(oled, 0, 40, "  Option 2");
        oled_draw_text(oled, 0, 60, "> Option 3");
    }
    oled_update(oled);
}

void blink_system() {
    while (true) {
        led_rgb_put(true, false, false);  // Vermelho
        sleep_ms(500);
        led_rgb_put(false, true, false);  // Verde
        sleep_ms(500);
        led_rgb_put(false, false, true);  // Azul
        sleep_ms(500);
        led_rgb_put(true, true, true);    // Branco
        sleep_ms(500);
    }
}

int main() {
    stdio_init_all();
    setup_gpio();

    // Inicialização do OLED
    ssd1306_t oled;
    ssd1306_init(&oled, 128, 64);
    oled_clear(&oled);

    while (true) {
        // Lógica de navegação no menu com o joystick
        if (gpio_get(JOY_UP_PIN) == 0) {
            menu_option = (menu_option == 0) ? 2 : menu_option - 1;
            sleep_ms(200);  // Debounce
        }
        if (gpio_get(JOY_DOWN_PIN) == 0) {
            menu_option = (menu_option == 2) ? 0 : menu_option + 1;
            sleep_ms(200);  // Debounce
        }
        if (gpio_get(JOY_SELECT_PIN) == 0) {
            if (menu_option == 0) {
                blink_system();  // Acesso ao sistema Blink
            } else if (menu_option == 1) {
                // Acesso a outra opção
            } else {
                // Acesso a outra opção
            }
            sleep_ms(200);  // Debounce
        }

        // Atualiza o display com o menu
        display_menu(&oled);
    }

    return 0;
}
