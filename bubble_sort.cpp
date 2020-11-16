#include <tsal.hpp>
#include <omp.h>

using namespace tsal;

#define MAX_VALUE 100000

void bubbleSort(ThreadSynth& synth, int size, int data[], bool audio) {
  int temp;
  for (int i = 0; i < size; i++) {
    for (int j = size - 1; j > i; j--) {
      MidiNote note = Util::scaleToNote(data[j], std::make_pair(0, MAX_VALUE), std::make_pair(C3, C7));
      if (audio) synth.play(note, Timing::MICROSECOND, 50);
      
      if (data[j] < data[j - 1]) {
        temp = data[j];
        data[j] = data[j - 1];
        data[j - 1] = temp;
      }
    }
  }
}

int main(int argc, char** argv) {
  bool audio = false;
  if (argc > 1) {
    if (std::string(argv[1]) == "-a") {
      audio = true;
    }
  }
  double startTime = omp_get_wtime();
  srand(1876);

  Mixer mixer;
  ThreadSynth synth(&mixer);
  mixer.add(synth);
  synth.setEnvelopeActive(false);
  
  // Generate the data
  const int size = 1000;
  int data[size];
  for (int i = 0; i < size; i++) {
    data[i] = rand() % MAX_VALUE;
  }
  // Sort the data
  bubbleSort(synth, size, data, audio);

  std::cout << "Time taken: " << omp_get_wtime() - startTime << " seconds" << std::endl;
}
