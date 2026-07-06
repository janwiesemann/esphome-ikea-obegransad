#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"

#include "ikea_obegraensad.h"

namespace esphome {
namespace obegraensadpanel {

static const char *const TAG = "obegraensadpanel";

void Panel::setup() { this->spi_setup(); }

void Panel::update() {
  this->do_update_();

  this->write_array(buffer.data(), buffer.size());
  this->latch_pin->digital_write(1);
  delay_microseconds_safe(1);
  this->latch_pin->digital_write(0);

 /* std::string s;
  s.push_back('\n');
  for (size_t i = 0; i < BUFFER_SIZE; i++)
  {
    uint8_t b = buffer[i];
    for (size_t j = 0; j < 8; j++)
    {
      if((b & (1 << (7-j))) != 0)
        s.push_back('1');
      else
        s.push_back('0');
    }

    if(i % 2 == 0)
      s.push_back(' ');
    else
      s.push_back('\n');
  }
  ESP_LOGD(TAG, "%s", s.data());*/
}

void Panel::dump_config() {
  LOG_DISPLAY("", "Panel", this);
  LOG_SPI_DEVICE(this);
  LOG_PIN("  Latch Pin: ", this->latch_pin);
  LOG_UPDATE_INTERVAL(this);
}

const uint8_t byte_lookup[] = { 2, 0, 3 ,1, 4, 6, 5, 7 };

void Panel::draw_pixel_at(int x, int y, Color color) {
  const size_t pixel = y * NUMBER_OF_COLUMNS + x;
  const size_t byte_index = pixel / 8;
  const size_t panel_index = byte_index / 8;
  const size_t byte_on_panel = byte_index % 8;

  const uint8_t byte = panel_index * 8 + byte_lookup[byte_on_panel];
  const bool reverse_bits = (y % 2) != 0;

  size_t bit = pixel % 8;
  if(reverse_bits)
    bit= 7- bit;

  if (color.is_on())
    buffer[byte] |= 1 << bit;
  else
    buffer[byte] &= ~(1 << bit);
}

}  // namespace obegraensadpanel
}  // namespace esphome
