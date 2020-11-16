#include <tsal.hpp>
#include <omp.h>

using namespace tsal;

#define MAX_VALUE 100000

void quickSort(ThreadSynth& synth, int* data, int low, int high, bool audio) {
  if (low < high) {
    // Partition
    int pivotValue = data[low];
    int pivot = low;
    for (int i = low + 1; i < high; i++) {
      MidiNote note = Util::scaleToNote(data[i], std::make_pair(0, MAX_VALUE), std::make_pair(C3, C7));
      if (audio) synth.play(note, Timing::MICROSECOND, 50);
      
      if (data[i] < pivotValue) {
        pivot++;
        std::swap(data[i], data[pivot]);
      }
    }
    std::swap(data[low], data[pivot]);
    
    quickSort(synth, data, low, pivot - 1, audio);
    quickSort(synth, data, pivot + 1, high, audio);
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
  const int size = 5000;
  int* data = new int[size];
  for (int i = 0; i < size; i++) {
    data[i] = rand() % MAX_VALUE;
  }
  
  // Sort the data
  quickSort(synth, data, 0, size, audio);

  std::cout << "Time taken: " << omp_get_wtime() - startTime << " seconds" << std::endl;
}
