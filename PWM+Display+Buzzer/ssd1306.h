#ifndef SSD1306_H
#define SSD1306_H

#include "ssd1306_i2c.h"

// Definir as dimensões do display apenas se não forem definidas
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH 128
#endif

#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT 64
#endif

// Definir o endereço I2C do display apenas se não for definido
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR 0x3C  // Endereço padrão do SSD1306
#endif

// Estrutura para armazenar as configurações do display
typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t *buffer;
} ssd1306_t;

// Declaração das funções
extern void calc_render_area_buflen(struct render_area *area);
extern void SSD1306_send_cmd(uint8_t cmd);
extern void SSD1306_send_cmd_list(uint8_t *buf, int num);
extern void SSD1306_send_buf(uint8_t buf[], int buflen);
extern void SSD1306_init(void);
extern void SSD1306_scroll(bool on);
extern void render(uint8_t *buf, struct render_area *area);
extern void SetPixel(uint8_t *buf, int x, int y, bool on);
extern void DrawLine(uint8_t *buf, int x0, int y0, int x1, int y1, bool on);
extern void WriteChar(uint8_t *buf, int16_t x, int16_t y, uint8_t ch);
extern void WriteString(uint8_t *buf, int16_t x, int16_t y, char *str);

#endif // SSD1306_H
