//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2018 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#include "Audio.hxx"
#include "AudioQueue.hxx"

#include <cmath>

namespace {
  constexpr double R_MAX = 30.;
  constexpr double R = 1.;

  Int16 mixingTableEntry(uInt8 v, uInt8 vMax)
  {
    return static_cast<Int16>(
      floor(0x7fff * double(v) / double(vMax) * (R_MAX + R * double(vMax)) / (R_MAX + R * double(v)))
    );
  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Audio::Audio()
  : myAudioQueue(0),
    myCurrentFragment(0)
{
  for (uInt8 i = 0; i <= 0x1e; i++) myMixingTableSum[i]= mixingTableEntry(i, 0x1e);
  for (uInt8 i = 0; i <= 0x0f; i++) myMixingTableIndividual[i] = mixingTableEntry(i, 0x0f);

  reset();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Audio::reset()
{
  myCounter = 0;
  mySampleIndex = 0;

  myChannel0.reset();
  myChannel1.reset();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Audio::setAudioQueue(shared_ptr<AudioQueue> queue)
{
  myAudioQueue = queue;

  myCurrentFragment = myAudioQueue->enqueue();
  mySampleIndex = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Audio::tick()
{ switch (myCounter) {
    case 9:
    case 81:
      myChannel0.phase0();
      myChannel1.phase0();

      break;

    case 37:
    case 149:
      phase1();
      break;
  }

  if (++myCounter == 228) myCounter = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Audio::phase1()
{
  uInt8 sample0 = myChannel0.phase1();
  uInt8 sample1 = myChannel1.phase1();

  if (!myAudioQueue) return;

  if (myAudioQueue->isStereo()) {
    myCurrentFragment[2*mySampleIndex] = myMixingTableIndividual[sample0];
    myCurrentFragment[2*mySampleIndex + 1] = myMixingTableIndividual[sample1];
  } else {
    myCurrentFragment[mySampleIndex] = myMixingTableSum[sample0 + sample1];
  }

  if (++mySampleIndex == myAudioQueue->fragmentSize()) {
    mySampleIndex = 0;
    myCurrentFragment = myAudioQueue->enqueue(myCurrentFragment);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AudioChannel& Audio::channel0()
{
  return myChannel0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AudioChannel& Audio::channel1()
{
  return myChannel1;
}