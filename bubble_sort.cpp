#include <iostream>
#include <vector>
#include <cassert>
#include <tsal.hpp>
#include <tsgl.h>
#include <omp.h>
#include <unistd.h>
#include "sortUtilities.h"
using namespace tsal;

void bubbleSort(vector<int>& data, ThreadSynth& synth, tsgl::Rectangle** rectangles, bool audio, bool graphics) {
  int temp;
  const int size = data.size();
  for (int i = 0; i < size; i++) {
    for (int j = size - 1; j > i; j--) {
      MidiNote note = Util::scaleToNote(data[j], std::make_pair(0, MAX_VALUE), std::make_pair(C3, C7));
      if (audio) synth.play(note, Timing::MICROSECOND, 50);
              if (graphics)
        {
          rectangles[j]->setHeight(data[j-1]);
          rectangles[j-1]->setHeight(temp);
        }
      
      if (data[j] < data[j - 1]) {
        temp = data[j];
        data[j] = data[j - 1];
        data[j - 1] = temp;

      }
    }
  }
}

int main(int argc, char** argv) {
  bool graphics = true;
  bool audio = true;
  int opt;

  while ((opt = getopt(argc, argv, "ag")) != -1) {
    switch (opt) {
      case 'a':
        audio = true;
        break;
      case 'g':
        graphics = true;
        break;
      default:
        break;
    }
  }
  
  //tsal setup
  Mixer mixer;
  ThreadSynth synth(&mixer);
  mixer.add(synth);
  synth.setEnvelopeActive(false);
  
  // Generate data
  const int size = 1000; 
  vector<int> data(size);
  initialize(data);
  assert( !sorted(data) );

  //tsgl setup
  tsgl::Canvas* can;
  int w = size * 1.3;
  int h = w/2;
  if (graphics) {
    can = new tsgl::Canvas(0, 0, w, h, "Bubble Sort", tsgl::BLACK);
    can->start();
  }

  // rectangles
  tsgl::Rectangle** rectangles = new tsgl::Rectangle*[size];
  if (graphics) {
    float start = -can->getWindowWidth() * .45;
    float width = can->getWindowWidth() * .9 / size;
    for (int i = 0; i < size; i++) {
      rectangles[i] = new tsgl::Rectangle(start + i * width, 0, 0, width, data[i], 0, 0, 0, tsgl::RED);
      rectangles[i]->setIsOutlined(false);
      can->add(rectangles[i]);
    }
  }
  
  

  // Sort the data
  double startTime = omp_get_wtime();
  bubbleSort(data, synth, rectangles, audio, graphics);
  double stopTime = omp_get_wtime();

  assert( sorted(data) );
  std::cout << "Time taken to sort " << size << " items: "
            << stopTime - startTime << " seconds" << std::endl;
  synth.stop();
  can->wait();
}
