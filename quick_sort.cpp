/* quick_sort.cpp
 *
 */

#include <iostream>
#include <vector>
#include <cassert>
#include <cstring>
#include <omp.h>
#include <tsal.hpp>
#include "sortUtilities.h"
using namespace std;


void split(vector<int>& data, int first, int last, int& pivotIndex, tsal::ThreadSynth& synth, bool enableAudio, int size) {
  int pivot = data[first];
  int saveFirst = first;
  bool onCorrectSide;
  ++first;
  do {
    onCorrectSide = true;
    while (onCorrectSide) {              // Move first toward last.
      if (data[first] > pivot) {
        onCorrectSide = false;
      } else {
        ++first;
        onCorrectSide = (first <= last);
      }
    }
    onCorrectSide = (first <= last);
    while (onCorrectSide) {               // Move last toward first.
      if (data[last] <= pivot) {
        onCorrectSide = false;
      } else {
        --last;
        onCorrectSide = (first <= last);
      }
    }
    if (first < last) {
      swap(data[first], data[last]);
      if (enableAudio) synth.play(tsal::C2 + 60 * (data[first] / size ) / 100, tsal::Timing::MICROSECOND, 75);
      ++first;
      --last;
    }
  } while (first <= last);

  pivotIndex = last;
  swap(data[saveFirst], data[pivotIndex]);
}

void quickSort(vector<int>& data, int first, int last, tsal::ThreadSynth& synth, bool enableAudio, int size) {
  if (first < last) {
    int pivotIndex;
    split(data, first, last, pivotIndex, synth, enableAudio, size);
    quickSort(data, first, pivotIndex-1, synth, enableAudio, size);
    quickSort(data, pivotIndex+1, last, synth, enableAudio, size);
  }
}

int main(int argc, char** argv) {
  bool audio = false;
  if (argc > 1) {
    if (std::string(argv[1]) == "-a") {
      audio = true;
    }
  }
  tsal::Mixer mixer;
  tsal::ThreadSynth voice(&mixer);
  mixer.add(voice);
  voice.setVolume(0.5);
  voice.setEnvelopeActive(false);      
  const int SIZE = 10000;
  vector<int> data(SIZE);
  initialize(data);
  assert( !sorted(data) );

  double startTime = omp_get_wtime();
  quickSort(data, 0, SIZE-1, voice, audio, SIZE);
  // quickSort(data, 0, SIZE-1);
  double stopTime = omp_get_wtime();

  assert( sorted(data) );
  std::cout << "Time taken to sort " << SIZE << " items: "
            << stopTime - startTime << " seconds" << std::endl;
}
