#define MAX_LED 300
#define MAX_CHANNEL 10

typedef struct led_color_t{
  uint8_t red;
  uint8_t blue;
  uint8_t green;
}led_color_t;


typedef struct channel_t{
  uint16_t number_of_nodes;
  uint16_t start_index;
}channel_t;

static led_color_t led_buff[MAX_LED];
