#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#define LED_RED_PIN 13       // Pino do LED vermelho
#define LED_GREEN_PIN 11     // Pino do LED verde
#define LED_BLUE_PIN 12      // Pino do LED azul
#define BUTTON1_PIN 5        // Pino do botão 1
#define BUTTON2_PIN 6        // Pino do botão 2
#define WIFI_SSID "ap"       // Substitua pelo nome da sua rede Wi-Fi
#define WIFI_PASS "12345678" // Substitua pela senha da sua rede Wi-Fi

// Estado dos botões e contadores
char button1_message[50] = "Nenhum evento no botão 1";
char button2_message[50] = "Nenhum evento no botão 2";
int button1_count = 0;
int button2_count = 0;

// Buffer para resposta HTTP
char http_response[2048];

// Função para criar a resposta HTTP
void create_http_response() {
    snprintf(http_response, sizeof(http_response),
             "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
             "<!DOCTYPE html>"
             "<html>"
             "<head>"
             "  <meta charset=\"UTF-8\">"
             "  <title>Controle do LED e Botões</title>"
             "  <style>"
             "    body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 20px; }"
             "    h1 { color: #333; }"
             "    p { font-size: 18px; }"
             "    .button { display: inline-block; margin: 10px 0; padding: 10px; border-radius: 5px; text-decoration: none; font-size: 16px; }"
             "    .red { background-color: #FF6347; color: white; }"
             "    .green { background-color: #32CD32; color: white; }"
             "    .blue { background-color: #1E90FF; color: white; }"
             "    .yellow { background-color: #FFD700; color: black; }"
             "    .cyan { background-color: #00CED1; color: white; }"
             "    .magenta { background-color: #FF00FF; color: white; }"
             "    .button:hover { opacity: 0.8; }"
             "    .container { display: flex; flex-wrap: wrap; justify-content: space-between; }"
             "    .button p { margin: 0; padding: 0; }"
             "  </style>"
             "</head>"
             "<body>"
             "  <h1>Controle do LED e Botões</h1>"
             "  <div class=\"container\">"
             "    <a href=\"/led/red\" class=\"button red\"><p>LED Vermelho</p></a>"
             "    <a href=\"/led/green\" class=\"button green\"><p>LED Verde</p></a>"
             "    <a href=\"/led/blue\" class=\"button blue\"><p>LED Azul</p></a>"
             "    <a href=\"/led/yellow\" class=\"button yellow\"><p>LED Amarelo</p></a>"
             "    <a href=\"/led/cyan\" class=\"button cyan\"><p>LED Ciano</p></a>"
             "    <a href=\"/led/magenta\" class=\"button magenta\"><p>LED Magenta</p></a>"
             "    <a href=\"/update\" class=\"button\"><p>Atualizar Estado</p></a>"
             "  </div>"
             "  <h2>Estado dos Botões:</h2>"
             "  <p>Botão 1: %s (Pressionado %d vezes)</p>"
             "  <p>Botão 2: %s (Pressionado %d vezes)</p>"
             "</body>"
             "</html>\r\n",
             button1_message, button1_count, button2_message, button2_count);
}

// Função de callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }
    
    char *request = (char *)p->payload;
    
    // Processar as requisições para controlar os LEDs
    if (strstr(request, "GET /led/on")) {
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 0);
    } else if (strstr(request, "GET /led/off")) {
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 0);
    } else if (strstr(request, "GET /led/red")) {
        gpio_put(LED_RED_PIN, 1);
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 0);
    } else if (strstr(request, "GET /led/green")) {
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 1);
        gpio_put(LED_BLUE_PIN, 0);
    } else if (strstr(request, "GET /led/blue")) {
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 1);
    } else if (strstr(request, "GET /led/yellow")) {
        gpio_put(LED_RED_PIN, 1);
        gpio_put(LED_GREEN_PIN, 1);
        gpio_put(LED_BLUE_PIN, 0); // Mistura de vermelho e verde
    } else if (strstr(request, "GET /led/cyan")) {
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 1);
        gpio_put(LED_BLUE_PIN, 1); // Mistura de verde e azul
    } else if (strstr(request, "GET /led/magenta")) {
        gpio_put(LED_RED_PIN, 1);
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 1); // Mistura de vermelho e azul
    }
    
    create_http_response();
    
    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
    
    pbuf_free(p);
    
    return ERR_OK;
}

// Callback de conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}

// Inicia o servidor HTTP
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    
    if (!pcb || tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        return;
    }
    
    pcb = tcp_listen(pcb);
    
    tcp_accept(pcb, connection_callback);
}

// Monitora o estado dos botões
void monitor_buttons() {
    static bool button1_last_state = false;
    static bool button2_last_state = false;
    
    bool button1_state = !gpio_get(BUTTON1_PIN);
    bool button2_state = !gpio_get(BUTTON2_PIN);

    if (button1_state && !button1_last_state) {
        button1_count++;
        snprintf(button1_message, sizeof(button1_message), "Botão 1 foi pressionado!");
    }
    
    if (button2_state && !button2_last_state) {
        button2_count++;
        snprintf(button2_message, sizeof(button2_message), "Botão 2 foi pressionado!");
    }
    
    button1_last_state = button1_state;
    button2_last_state = button2_state;
}

int main() {
    stdio_init_all();
    
    sleep_ms(10000); // Aguarda a inicialização
    
    if (cyw43_arch_init()) return 1;
    
    cyw43_arch_enable_sta_mode();
    
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID,
                                            WIFI_PASS,
                                            CYW43_AUTH_WPA2_AES_PSK,
                                            10000)) return 1;

    // Inicializa os pinos do LED RGB
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);

    // Inicializa os pinos dos botões
    gpio_init(BUTTON1_PIN);
    gpio_set_dir(BUTTON1_PIN, GPIO_IN);
    gpio_pull_up(BUTTON1_PIN);

    gpio_init(BUTTON2_PIN);
    gpio_set_dir(BUTTON2_PIN, GPIO_IN);
    gpio_pull_up(BUTTON2_PIN);

    start_http_server();
    
   while (true) {
       cyw43_arch_poll();
       monitor_buttons();
       sleep_ms(100); // Intervalo de verificação dos botões
   }
   
   return 0;
}
