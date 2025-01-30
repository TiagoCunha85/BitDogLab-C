#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#define LED_PIN_R 12
#define LED_PIN_G 13
#define LED_PIN_B 14

#define WIFI_SSID "ap"
#define WIFI_PASS "12345678"

// Estado do LED RGB
typedef struct {
    uint8_t r, g, b;
    uint8_t effect;
    bool effect_running;
} led_state_t;

led_state_t led_state = {0, 0, 0, 0, false};

// Resposta HTML
const char HTTP_RESPONSE[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
"<!DOCTYPE html><html><head><style>"
"body { font-family: Arial, sans-serif; text-align: center; }"
"button { padding: 10px; margin: 5px; cursor: pointer; }"
".color-btn { width: 100px; height: 50px; border: none; color: white; }"
"</style></head><body>"
"<h1>Controle do LED RGB</h1>"
"<button class='color-btn' style='background:red;' onclick='location.href=\"/color/red\"'>Vermelho</button>"
"<button class='color-btn' style='background:green;' onclick='location.href=\"/color/green\"'>Verde</button>"
"<button class='color-btn' style='background:blue;' onclick='location.href=\"/color/blue\"'>Azul</button>"
"<button class='color-btn' style='background:yellow;' onclick='location.href=\"/color/yellow\"'>Amarelo</button>"
"<button class='color-btn' style='background:purple;' onclick='location.href=\"/color/purple\"'>Roxo</button>"
"<button class='color-btn' style='background:cyan;' onclick='location.href=\"/color/cyan\"'>Ciano</button>"
"<button class='color-btn' style='background:white; color:black;' onclick='location.href=\"/color/white\"'>Branco</button>"
"<button class='color-btn' style='background:black; color:white;' onclick='location.href=\"/color/off\"'>Desligar</button>"
"<h2>Efeitos</h2>"
"<button onclick='location.href=\"/effect/blink\"'>Piscar</button>"
"<button onclick='location.href=\"/effect/fade\"'>Fade</button>"
"<button onclick='location.href=\"/effect/rainbow\"'>Rainbow</button>"
"<button onclick='location.href=\"/effect/stop\"'>Parar</button>"
"</body></html>\r\n";

void setup_rgb_led() {
    gpio_init(LED_PIN_R);
    gpio_init(LED_PIN_G);
    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    gpio_put(LED_PIN_R, r);
    gpio_put(LED_PIN_G, g);
    gpio_put(LED_PIN_B, b);
    led_state.r = r;
    led_state.g = g;
    led_state.b = b;
}

static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }
    char *request = (char *)p->payload;
    if (strstr(request, "GET /color/red"))        set_rgb(1, 0, 0);
    else if (strstr(request, "GET /color/green")) set_rgb(0, 1, 0);
    else if (strstr(request, "GET /color/blue"))  set_rgb(0, 0, 1);
    else if (strstr(request, "GET /color/yellow")) set_rgb(1, 1, 0);
    else if (strstr(request, "GET /color/purple")) set_rgb(1, 0, 1);
    else if (strstr(request, "GET /color/cyan"))   set_rgb(0, 1, 1);
    else if (strstr(request, "GET /color/white"))  set_rgb(1, 1, 1);
    else if (strstr(request, "GET /color/off"))    set_rgb(0, 0, 0);

    tcp_write(tpcb, HTTP_RESPONSE, strlen(HTTP_RESPONSE), TCP_WRITE_FLAG_COPY);
    pbuf_free(p);
    return ERR_OK;
}

static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}

static void start_http_server() {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb || tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) return;
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);
    printf("Servidor HTTP rodando na porta 80...\n");
}

int main() {
    stdio_init_all();
    sleep_ms(5000);
    printf("Iniciando Wi-Fi...\n");
    if (cyw43_arch_init()) return 1;
    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) return 1;
    setup_rgb_led();
    start_http_server();
    while (true) cyw43_arch_poll();
    return 0;
}