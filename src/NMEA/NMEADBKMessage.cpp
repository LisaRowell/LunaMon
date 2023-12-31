/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2023 Lisa Rowell
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

#include "NMEADBKMessage.h"

#include "NMEATenthsUInt16.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "Util/StringTools.h"
#include "Util/PlacementNew.h"
#include "Util/Logger.h"

NMEADBKMessage::NMEADBKMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEADBKMessage::parse(NMEALine &nmeaLine) {
    if (nmeaLine.isEncapsulatedData()) {
        logger << logWarning << talker << " DBK message in unsupported encapsulated format" << eol;
        return false;
    }

    if (!depthFeet.extract(nmeaLine, talker, "DBK", "Depth Feet", true)) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "DBK", "f")) {
        return false;
    }

    if (!depthMeters.extract(nmeaLine, talker, "DBK", "Depth Meters", true)) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "DBK", "M")) {
        return false;
    }

    if (!depthFathoms.extract(nmeaLine, talker, "DBK", "Depth Fathoms", true)) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "DBK", "F")) {
        return false;
    }

    return true;
}

enum NMEAMsgType NMEADBKMessage::type() const {
    return NMEA_MSG_TYPE_DBK;
}

void NMEADBKMessage::log() const {
    logger << logDebugNMEA << talker << " DBK: Depth " << depthFeet << "', " << depthMeters
           << " m, " << depthFathoms << " ftm" << eol;
}

NMEADBKMessage *parseNMEADBKMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEADBKMessage *message = new (nmeaMessageBuffer)NMEADBKMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
