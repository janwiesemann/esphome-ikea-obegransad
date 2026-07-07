#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"

#include "ikea_obegraensad.h"

namespace esphome {
namespace obegraensadpanel {

static const char *const TAG = "obegraensadpanel";

void Panel::setup() {
  this->spi_setup();
}

void Panel::update() {
  this->do_update_();

  this->write_array(buffer.data(), buffer.size());
  this->latch_pin->digital_write(1);
  delay_microseconds_safe(1);
  this->latch_pin->digital_write(0);
}

void Panel::dump_config() {
  LOG_DISPLAY("", "Panel", this);
  LOG_PIN("  Latch Pin: ", this->latch_pin);
  LOG_UPDATE_INTERVAL(this);
}

void HOT Panel::draw_pixel_at(int x, int y, Color color) {
  Panel::translate_coordinate_rotation(x, y);
  auto[byte_index, bit_index] = Panel::translate_coordinate_to_byte_and_bit(x, y);

  if (color.is_on())
    buffer[byte_index] |= 1 << bit_index;
  else
    buffer[byte_index] &= ~(1 << bit_index);
}

void HOT Panel::translate_coordinate_rotation(int& x, int& y) {
  // Copy&Paste from esphome::display::DisplayBuffer::draw_pixel_at(int, int, Color)
  switch (this->rotation_) {
    case esphome::display::DISPLAY_ROTATION_0_DEGREES:
      break;
    case esphome::display::DISPLAY_ROTATION_90_DEGREES:
      std::swap(x, y);
      x = this->get_width_internal() - x - 1;
      break;
    case esphome::display::DISPLAY_ROTATION_180_DEGREES:
      x = this->get_width_internal() - x - 1;
      y = this->get_height_internal() - y - 1;
      break;
    case esphome::display::DISPLAY_ROTATION_270_DEGREES:
      std::swap(x, y);
      y = this->get_height_internal() - y - 1;
      break;
  }
}

const uint8_t byte_lookup[Panel::NUMBER_OF_BYTES_PER_PANEL] = { 2, 0, 3 ,1, 4, 6, 5, 7 };
std::tuple<Panel::byte_index_t, Panel::bit_index_t> HOT Panel::translate_coordinate_to_byte_and_bit(int x, int y) {
  //The compiler will optimize this into a much smaller form.

  const size_t pixel_index = y * NUMBER_OF_COLUMNS + x; //Index of the Pixel, which will used

  const size_t raw_byte_index = pixel_index / 8; //Untranslated index of the byte, which contains the Pixel

  const size_t panel_index = raw_byte_index / NUMBER_OF_BYTES_PER_PANEL; //Index of the Panel, which is displaying the pixel
  const size_t panel_raw_byte_index = raw_byte_index % NUMBER_OF_BYTES_PER_PANEL; //Index of the byte, on the current Panel, which contains the pixel
  const size_t panel_translated_byte_index = byte_lookup[panel_raw_byte_index]; //Translated index of the actuall byte order on the Panel

  const size_t translated_byte_index = panel_index * NUMBER_OF_BYTES_PER_PANEL + panel_translated_byte_index; //The translated byte index

  const bool reverse_bit_order = (y % 2) != 0; //Reverse the bit-order on every second row.
  size_t bit_in_byte = pixel_index % 8; //Index of the pixel-bit in relation to a full pixel.
  if(reverse_bit_order)
    bit_in_byte = 7 - bit_in_byte;

  return {static_cast<byte_index_t>(translated_byte_index), static_cast<bit_index_t>(bit_in_byte)};
}

}  // namespace obegraensadpanel
}  // namespace esphome
