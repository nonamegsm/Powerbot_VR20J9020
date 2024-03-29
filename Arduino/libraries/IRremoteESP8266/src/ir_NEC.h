// Copyright 2009 Ken Shirriff
// Copyright 2017, 2018 David Conran

// NEC originally added from https://github.com/shirriff/Arduino-IRremote/

#ifndef IR_NEC_H_
#define IR_NEC_H_

#include <stdint.h>
#include "IRremoteESP8266.h"

// Supports:
//   Brand: Yamaha,  Model: RAV561 remote
//   Brand: Yamaha,  Model: RXV585B A/V Receiver

// Constants
// Ref:
//  http://www.sbprojects.com/knowledge/ir/nec.php
const uint16_t kNecTick = 558;
const uint16_t kNecHdrMarkTicks = 8;
const uint16_t kNecHdrMark = kNecHdrMarkTicks * kNecTick;
const uint16_t kNecHdrSpaceTicks = 8;
const uint16_t kNecHdrSpace = kNecHdrSpaceTicks * kNecTick;
const uint16_t kNecBitMarkTicks = 1;
const uint16_t kNecBitMark = kNecBitMarkTicks * kNecTick;
const uint16_t kNecOneSpaceTicks = 3;
const uint16_t kNecOneSpace = kNecOneSpaceTicks * kNecTick;
const uint16_t kNecZeroSpaceTicks = 1;
const uint16_t kNecZeroSpace = kNecZeroSpaceTicks * kNecTick;
const uint16_t kNecRptSpaceTicks = 4;
const uint16_t kNecRptSpace = kNecRptSpaceTicks * kNecTick;
const uint16_t kNecRptLength = 4;
const uint16_t kNecMinCommandLengthTicks = 193;
const uint32_t kNecMinCommandLength = kNecMinCommandLengthTicks * kNecTick;
const uint32_t kNecMinGap =
    kNecMinCommandLength -
    (kNecHdrMark + kNecHdrSpace + kNECBits * (kNecBitMark + kNecOneSpace) +
     kNecBitMark);
const uint16_t kNecMinGapTicks =
    kNecMinCommandLengthTicks -
    (kNecHdrMarkTicks + kNecHdrSpaceTicks +
     kNECBits * (kNecBitMarkTicks + kNecOneSpaceTicks) + kNecBitMarkTicks);

#endif  // IR_NEC_H_
