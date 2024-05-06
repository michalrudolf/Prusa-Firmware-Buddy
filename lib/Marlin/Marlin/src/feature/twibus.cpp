/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../inc/MarlinConfig.h"

#if ENABLED(EXPERIMENTAL_I2CBUS)

#include "twibus.h"
#include "hwio_pindef.h"
#include "i2c.hpp"

static constexpr uint16_t i2c_timeout = 100;

FORCE_INLINE char hex_nybble(const uint8_t n) {
  return (n & 0xF) + ((n & 0xF) < 10 ? '0' : 'A' - 10);
}

TWIBus twibus;

TWIBus::TWIBus() {
  reset();
}

void TWIBus::reset() {
  buffer_s = 0;
  buffer[0] = 0x00;
  read_buffer_available = 0;
  read_buffer_pos = 0;
}

bool TWIBus::read_buffer_has_byte() {
  return read_buffer_pos < read_buffer_available;
}

uint8_t TWIBus::read_buffer_read_byte() {
  if (!read_buffer_has_byte()) {
    return 0;
  }
  return read_buffer[read_buffer_pos++];
}

bool TWIBus::address(const uint8_t adr) {
  if (!WITHIN(adr, 8, 127)) {
    SERIAL_ECHO_MSG("Bad I2C address (8-127)");
    return false;
  }
  
  /*
   *  Restricted addresses:
   *    (EEPOM addresses are higher than 127 anyway)
   *    EepromCommandWrite (uint16_t)
   *      write_addr_memory = 0xA6;
   *      write_addr_registers = 0xAE;
   *    EepromCommandRead (uint16_t)
   *      read_addr_memory = 0xA7;
   *      read_addr_registers = 0xAF;
   *
   *  Special address list:
   *    IO Expander TCA6408A with 8 pins: 0x40 - 0x48
  */

  addr = adr;

  debug(F("address"), adr);
  return true;
}

void TWIBus::addbyte(const char c) {
  if (buffer_s >= COUNT(buffer)) return;
  buffer[buffer_s++] = c;
  debug(F("addbyte"), c);
}

void TWIBus::addbytes(char src[], uint8_t bytes) {
  debug(F("addbytes"), bytes);
  while (bytes--) addbyte(*src++);
}

void TWIBus::addstring(char str[]) {
  debug(F("addstring"), str);
  while (char c = *str++) addbyte(c);
}

void TWIBus::send() {
  debug(F("send"), addr);
  
  switch (addr) {
  case buddy::hw::expanderOutput1.slave_address:
    buddy::hw::expanderOutput1.write(buffer[0] ? buddy::hw::Pin::State::high : buddy::hw::Pin::State::low);
    break;
  case buddy::hw::expanderOutput2.slave_address:
    buddy::hw::expanderOutput2.write(buffer[0] ? buddy::hw::Pin::State::high : buddy::hw::Pin::State::low);
    break;
  case buddy::hw::expanderOutput3.slave_address:
    buddy::hw::expanderOutput3.write(buffer[0] ? buddy::hw::Pin::State::high : buddy::hw::Pin::State::low);
    break;
  case buddy::hw::expanderOutput4.slave_address:
    buddy::hw::expanderOutput4.write(buffer[0] ? buddy::hw::Pin::State::high : buddy::hw::Pin::State::low);
    break;
  case buddy::hw::expanderOutput5.slave_address:
    buddy::hw::expanderOutput5.write(buffer[0] ? buddy::hw::Pin::State::high : buddy::hw::Pin::State::low);
    break;
  case buddy::hw::expanderOutput6.slave_address:
    buddy::hw::expanderOutput6.write(buffer[0] ? buddy::hw::Pin::State::high : buddy::hw::Pin::State::low);
    break;
  case buddy::hw::expanderOutput7.slave_address:
    buddy::hw::expanderOutput7.write(buffer[0] ? buddy::hw::Pin::State::high : buddy::hw::Pin::State::low);
    break;
  case buddy::hw::expanderOutput8.slave_address:
    buddy::hw::expanderOutput8.write(buffer[0] ? buddy::hw::Pin::State::high : buddy::hw::Pin::State::low);
    break;
  }

  if (addr < buddy::hw::expanderOutput1.slave_address || addr > buddy::hw::expanderOutput8.slave_address) {
    // Unkown address
    i2c::Result ret = i2c::Transmit(hi2c2, addr << 1, buffer, buffer_s, i2c_timeout);
    check_hal_response(ret);
  }

  reset();
}

bool TWIBus::check_hal_response(i2c::Result response) {
  if (response == i2c::Result::ok) {
    return true;
  }

  switch (response) {
  case i2c::Result::error:
    SERIAL_ERROR_MSG("TWIBus::send failed with: ERROR");
    break;
  case i2c::Result::busy_after_retries:
    SERIAL_ERROR_MSG("TWIBus::send failed with: BUSY");
    break;
  case i2c::Result::timeout:
     SERIAL_ERROR_MSG("TWIBus::send failed with: TIMEOUT");
    break;
  default:
    SERIAL_ERROR_MSG("TWIBus::send failed with: UNKNOWN");
  }
  return false;
}

void TWIBus::echodata(uint8_t bytes, FSTR_P const pref, uint8_t adr, const uint8_t style/*=0*/) {
  union TwoBytesToInt16 { uint8_t bytes[2]; int16_t integervalue; };
  TwoBytesToInt16 ConversionUnion;

  while (bytes-- && read_buffer_has_byte()) {
    int value = read_buffer_read_byte();
    switch (style) {

      // Style 1, HEX DUMP
      case 1:
        SERIAL_CHAR(hex_nybble((value & 0xF0) >> 4));
        SERIAL_CHAR(hex_nybble(value & 0x0F));
        if (bytes) SERIAL_CHAR(' ');
        break;

      // Style 2, signed two byte integer (int16)
      case 2:
        if (bytes == 1)
          ConversionUnion.bytes[1] = (uint8_t)value;
        else if (bytes == 0) {
          ConversionUnion.bytes[0] = (uint8_t)value;
          // Output value in base 10 (standard decimal)
          SERIAL_ECHO(ConversionUnion.integervalue);
        }
        break;

      // Style 3, unsigned byte, base 10 (uint8)
      case 3:
        SERIAL_ECHO(value);
        if (bytes) SERIAL_CHAR(' ');
        break;

      // Default style (zero), raw serial output
      default:
        // This can cause issues with some serial consoles, Pronterface is an example where things go wrong
        SERIAL_CHAR(value);
        break;
    }
  }

  SERIAL_EOL();
}

bool TWIBus::request(const uint8_t bytes) {
  if (!addr) return false;

  debug(F("request"), bytes);

  if (bytes > TWIBUS_BUFFER_SIZE) {
    SERIAL_ERROR_MSG("TWIBus::request Tried to read more than max buffer size.");

    return false;
  }

  flush();

  i2c::Result ret = i2c::Receive(hi2c2, addr << 1, read_buffer, bytes, i2c_timeout);
  
  if (!check_hal_response(ret)) {
    return false;
  }

  read_buffer_available = bytes;
  return true;
}

void TWIBus::relay(const uint8_t bytes, const uint8_t style/*=0*/) {
  debug(F("relay"), bytes);

  if (request(bytes))
    echodata(bytes, F("i2c-reply"), addr, style);
}

uint8_t TWIBus::capture(char *dst, const uint8_t bytes) {
  reset();
  uint8_t count = 0;
  while (count < bytes && read_buffer_has_byte())
    dst[count++] = read_buffer_read_byte();

  debug(F("capture"), count);

  return count;
}

void TWIBus::flush() {
  read_buffer_available = 0;
  read_buffer_pos = 0;
}

#if ENABLED(DEBUG_TWIBUS)

  // static
  void TWIBus::prefix(FSTR_P const func) {
    SERIAL_ECHOPGM("TWIBus::", func, ": ");
  }
  void TWIBus::debug(FSTR_P const func, uint32_t adr) {
    if (DEBUGGING(INFO)) { prefix(func); SERIAL_ECHOLN(adr); }
  }
  void TWIBus::debug(FSTR_P const func, char c) {
    if (DEBUGGING(INFO)) { prefix(func); SERIAL_ECHOLN(c); }
  }
  void TWIBus::debug(FSTR_P const func, char str[]) {
    if (DEBUGGING(INFO)) { prefix(func); SERIAL_ECHOLN(str); }
  }

#endif

#endif // EXPERIMENTAL_I2CBUS
