#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "ssd1306.h"
#include "play_audio.h"

#define count_of(arr) (sizeof(arr) / sizeof((arr)[0]))

// Definições de pinos para o I2C
const uint I2C_SDA_PIN = 14;
const uint I2C_SCL_PIN = 15;

// Definições dos pinos do joystick
const uint JOYSTICK_UP = 16;
const uint JOYSTICK_DOWN = 17;
const uint JOYSTICK_SELECT = 18;

// Variável para armazenar a opção selecionada
int opcao_atual = 0;

// Opções do menu
char *menu_opcoes[] = {
    "1. Iniciar",
    "2. Configuracoes",
    "3. Sair"
};

// Função para exibir o menu no OLED
void exibir_menu(uint8_t *buf, struct render_area *frame_area) {
    memset(buf, 0, SSD1306_BUF_LEN);
    
    int y = 0;
    for (int i = 0; i < count_of(menu_opcoes); i++) {
        if (i == opcao_atual) {
            // Indica a opção selecionada com ">"
            char opcao[20];
            snprintf(opcao, sizeof(opcao), "> %s", menu_opcoes[i]);
            WriteString(buf, 5, y, opcao);
        } else {
            WriteString(buf, 5, y, menu_opcoes[i]);
        }
        y += 8;  // Reduziu de 10 para 8 para evitar corte no display
    }
    render(buf, frame_area);
}

// Função para processar entradas do joystick
void processar_entrada() {
    if (gpio_get(JOYSTICK_UP) == 0) {
        opcao_atual = (opcao_atual - 1 + count_of(menu_opcoes)) % count_of(menu_opcoes);
        sleep_ms(200);
    } else if (gpio_get(JOYSTICK_DOWN) == 0) {
        opcao_atual = (opcao_atual + 1) % count_of(menu_opcoes);
        sleep_ms(200);
    } else if (gpio_get(JOYSTICK_SELECT) == 0) {
        printf("Opcao selecionada: %s\n", menu_opcoes[opcao_atual]);
        sleep_ms(500);
    }
}

int main() {
    stdio_init_all();
    setup_audio();

    // Configuração do I2C
    i2c_init(i2c1, SSD1306_I2C_CLK * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Inicializa o display OLED
    SSD1306_init();

    // Área de renderização
    struct render_area frame_area = {
        .start_col = 0,
        .end_col = SSD1306_WIDTH - 1,
        .start_page = 0,
        .end_page = SSD1306_NUM_PAGES - 1
    };
    calc_render_area_buflen(&frame_area);

    // Buffer para renderização
    uint8_t buf[SSD1306_BUF_LEN];
    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, &frame_area);

    // Configuração dos pinos do joystick
    gpio_init(JOYSTICK_UP);
    gpio_set_dir(JOYSTICK_UP, GPIO_IN);
    gpio_pull_up(JOYSTICK_UP);

    gpio_init(JOYSTICK_DOWN);
    gpio_set_dir(JOYSTICK_DOWN, GPIO_IN);
    gpio_pull_up(JOYSTICK_DOWN);

    gpio_init(JOYSTICK_SELECT);
    gpio_set_dir(JOYSTICK_SELECT, GPIO_IN);
    gpio_pull_up(JOYSTICK_SELECT);

    // Loop principal
    while (true) {
        processar_entrada();
        exibir_menu(buf, &frame_area);
        sleep_ms(100);
    }

    return 0;
}
