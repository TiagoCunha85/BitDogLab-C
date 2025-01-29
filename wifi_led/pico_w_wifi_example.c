#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#define LED_MATRIX_ROWS 5   // Número de linhas da matriz
#define LED_MATRIX_COLS 5   // Número de colunas da matriz

// Definir os pinos para as linhas e colunas da matriz
uint8_t row_pins[LED_MATRIX_ROWS] = {0, 1, 2, 3, 4};  // Ajuste os pinos conforme necessário
uint8_t col_pins[LED_MATRIX_COLS] = {5, 6, 7, 8, 9};  // Ajuste os pinos conforme necessário

#define LED_R_PIN 10  // LED Vermelho (RGB)
#define LED_G_PIN 11  // LED Verde (RGB)
#define LED_B_PIN 12  // LED Azul (RGB)

#define WIFI_SSID "ap"
#define WIFI_PASS "1020304050"

// Função para desligar todos os LEDs da matriz
void turn_off_all_leds() {
    for (int i = 0; i < LED_MATRIX_ROWS; i++) {
        gpio_put(row_pins[i], 0);  // Desliga as linhas
    }

    for (int i = 0; i < LED_MATRIX_COLS; i++) {
        gpio_put(col_pins[i], 0);  // Desliga as colunas
    }
}

// Função para acender um LED específico na matriz (cor laranja)
void set_led_laranja(int row, int col) {
    turn_off_all_leds();  // Primeiro desliga todos os LEDs

    // Acesse a linha e a coluna específicas
    gpio_put(row_pins[row], 1);  // Liga a linha
    gpio_put(col_pins[col], 1);  // Liga a coluna

    // Para laranja (vermelho + verde)
    gpio_put(LED_R_PIN, 1);  // Liga o LED Vermelho
    gpio_put(LED_G_PIN, 1);  // Liga o LED Verde
    gpio_put(LED_B_PIN, 0);  // Desliga o LED Azul
}

// Função de callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Processa a requisição HTTP
    char *request = (char *)p->payload;

    // Controle dos LEDs na matriz
    if (strstr(request, "GET /led/orange")) {
        // Acende um LED específico na cor laranja na matriz (exemplo: linha 2, coluna 3)
        set_led_laranja(2, 3);  // Altere conforme necessário
    }

    // Envia a resposta HTTP
    tcp_write(tpcb, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n", 45, TCP_WRITE_FLAG_COPY);
    pbuf_free(p);

    return ERR_OK;
}

// Função para configurar o servidor HTTP
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }

    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);

    printf("Servidor HTTP rodando na porta 80...\n");
}

int main() {
    stdio_init_all();
    sleep_ms(10000);
    printf("Iniciando servidor HTTP\n");

    // Inicializa o Wi-Fi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return 1;
    } else {
        printf("Conectado.\n");
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    // Configura os pinos para as linhas e colunas da matriz
    for (int i = 0; i < LED_MATRIX_ROWS; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
    }
    
    for (int i = 0; i < LED_MATRIX_COLS; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_OUT);
    }

    // Configura os LEDs RGB como saída
    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    // Desliga todos os LEDs inicialmente
    turn_off_all_leds();

    // Inicia o servidor HTTP
    start_http_server();
    
    // Loop principal
    while (true) {
        cyw43_arch_poll();
        sleep_ms(100);
    }

    cyw43_arch_deinit();
    return 0;
}
