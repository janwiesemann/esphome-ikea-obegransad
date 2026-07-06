from esphome import pins
import esphome.codegen as cg
from esphome.components import display, spi
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_PAGES

DEPENDENCIES = ["spi"]

CONF_PIN_LATCH = "latch_pin"

obegraensadpanel_ns = cg.esphome_ns.namespace("obegraensadpanel")
Panel = obegraensadpanel_ns.class_("Panel", display.Display, spi.SPIDevice)

CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Panel),
            cv.Required(CONF_PIN_LATCH): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.polling_component_schema("200ms"))
    .extend(spi.spi_device_schema(cs_pin_required=False)),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await display.register_display(var, config)
    await spi.register_spi_device(var, config, write_only=True)

    latch_pin = await cg.gpio_pin_expression(config[CONF_PIN_LATCH])
    cg.add(var.set_latch_pin(latch_pin))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
