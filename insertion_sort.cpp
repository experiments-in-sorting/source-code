#include <tsal/tsal.hpp>

using namespace tsal;

#define MAX_VALUE 100000

void insertionSort(ThreadSynth& synth, int size, int* data) {
  int insertValue;
  int j;
  for (int i = 1; i < size; i++) {
    insertValue = data[i];
    j = i;
    while (j > 0 && data[j - 1] > insertValue) {
      MidiNote note = Util::scaleToNote(data[j], std::make_pair(0, MAX_VALUE), std::make_pair(C3, C7));
      synth.play(note, Timing::MICROSECOND, 50);
      
      data[j] = data[j - 1];
      j--;
    }
    data[j] = insertValue;
  }
}

int main() {
  Mixer mixer;
  ThreadSynth synth(&mixer);
  mixer.add(synth);
  synth.setEnvelopeActive(false);

  // Generate data
  const int size = 1000;
  int* data = new int[size];
  for (int i = 0; i < size; i++) {
    data[i] = rand() % MAX_VALUE;
  }
  // Sort the data
  insertionSort(synth, size, data);
} 

