/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2021-2023 Lisa Rowell
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Config.h"

#include <Arduino.h>

const char wifiSSID[] = "NETWORK_NAME";
const char wifiPassword[] = "YOUR_PASSWORD";

const uint16_t century = 2000;

const char mmsi[] = "YOUR_MMSI";

// We use this ID to destinquish between multiple instances of this type controller within the
// vessel.
const char controllerID[] = "1";

const char *nmeaWiFiSourceIPAddress = "YOUR_VESPERS_ADDR";
extern const uint16_t nmeaWiFiSourceTCPPort = 39150 /* Or whatever port your gear is using */;
