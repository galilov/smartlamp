//
// Created by Alexander Galilov <alexander.galilov@gmail.com>
// on 11/10/19.
//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include <string.h>

#include "WS2812B.h"

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define RMT_SRC_CLOCK   (80000000) // RMT source clock is APB CLK, 80 MHz by default
#define RMT_DIVIDER     (8) // 80MHz clock / 8 = 10MHz base clock for RMT
#define T0H             (400) // nanoseconds
#define T0L             (850) // nanoseconds
#define T1H             (850) // nanoseconds
#define T1L             (400) // nanoseconds

static const rmt_item32_t bit0 = {{{T0H * 100000L * RMT_DIVIDER / RMT_SRC_CLOCK, 1,
                                           T0L * 100000L * RMT_DIVIDER / RMT_SRC_CLOCK, 0}}}; //Logical 0

static const rmt_item32_t bit1 = {{{T1H * 100000L * RMT_DIVIDER / RMT_SRC_CLOCK, 1,
                                           T1L * 100000L * RMT_DIVIDER / RMT_SRC_CLOCK, 0}}}; //Logical 1

void WS2812B::_rmt_tx_init() {
    if (_isInitialized) {
        rmt_driver_uninstall(RMT_TX_CHANNEL);
    }
    rmt_config_t config;
    memset(&config, 0, sizeof(config));
    config.rmt_mode = RMT_MODE_TX;
    config.channel = RMT_TX_CHANNEL;
    config.gpio_num = _pin;
    config.mem_block_num = 1; // the block size is 32bit * 512
    config.tx_config.loop_en = 0;
    // disable the carrier
    config.tx_config.carrier_en = 0;
    config.tx_config.idle_output_en = 1;
    config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    // set the maximum clock divider to be able to output
    config.clk_div = RMT_DIVIDER;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0 /*rx_buf_size*/, 0/*intr_alloc_flags*/));
    _isInitialized = true;
}

void WS2812B::setLedCount(int nLeds, gpio_num_t pin) {
    _pin = pin;
    _items.clear();
    const int nItems = nLeds * 24;
    _items.reserve(nItems);
    for (auto i = 0; i < nItems; i++) {
        _items.push_back(bit0);
    }
    _rmt_tx_init();
}

void WS2812B::setLedColor(int index, int r, int g, int b) {
    auto offset = index * 24;
    for (auto color : {g, r, b}) {
        for (auto bitIndex = 7; bitIndex >= 0; --bitIndex) {
            if (0 != (color & (1 << bitIndex))) {
                _items[offset] = bit1;
            } else {
                _items[offset] = bit0;
            }
            offset++;
        }
    }
}

void WS2812B::show() {
    ESP_ERROR_CHECK(rmt_write_items(RMT_TX_CHANNEL, &_items[0], _items.size(), false));
}

WS2812B::~WS2812B() {
    if (_isInitialized) {
        rmt_driver_uninstall(RMT_TX_CHANNEL);
    }
}