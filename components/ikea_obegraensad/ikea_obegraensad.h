#pragma once

#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/spi/spi.h"
#include <array>
#include <tuple>

namespace esphome {
namespace obegraensadpanel {

class Panel final : public display::Display,
              public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW, spi::CLOCK_PHASE_LEADING,
                                    spi::DATA_RATE_1KHZ> {
 public:
  static constexpr size_t NUMBER_OF_COLUMNS = 16;
  static constexpr size_t NUMBER_OF_ROWS = 16;
  static constexpr size_t NUMBER_OF_PIXELS = NUMBER_OF_COLUMNS * NUMBER_OF_ROWS;
  static constexpr size_t BUFFER_SIZE = NUMBER_OF_PIXELS / 8;
  static constexpr size_t NUMBER_OF_BYTES_PER_PANEL = 8;

  float get_setup_priority() const override { return setup_priority::PROCESSOR; }

  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_BINARY; }

  int get_width_internal() override { return NUMBER_OF_COLUMNS; }

  int get_height_internal() override { return NUMBER_OF_ROWS; }

  void setup() override;
  void update() override;
  void dump_config() override;

  void draw_pixel_at(int x, int y, Color color) override;

  void set_latch_pin(GPIOPin *latch_pin) { this->latch_pin = latch_pin; }

 protected:
  using pixel_buffer = std::array<uint8_t, BUFFER_SIZE>;
  using byte_index_t = uint8_t;
  using bit_index_t = uint8_t;

  void translate_coordinate_rotation(int& x, int& y);
  std::tuple<byte_index_t, bit_index_t> translate_coordinate_to_byte_and_bit(int x, int y);

  GPIOPin *latch_pin{};
  pixel_buffer buffer;
};

}  // namespace obegraensadpanel
}  // namespace esphome
