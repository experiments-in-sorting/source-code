#include <tsal.hpp>
#include <tsgl.h>
#include <omp.h>

using namespace tsal;

#define MAX_VALUE 100000
#define TEST false

#if TEST
#include <assert.h>
#endif

enum MergeState {
  S_MERGE = 1,
  S_SHIFT = 2,
  S_WAIT = 3,
  S_DONE = 4,
  S_HIDE = 5
};

struct sortData {
  tsgl::ColorFloat color;      // Color of the thread
  MergeState state;            //Current state of the threads
  int first, last,             //Start and end of our block
    left, right,               //Indices of two numbers to compare
    fi, hi, li,                //Indices of first middle and last numbers in a set
    depth;                     //Current depth of the merge
  int* a;                      //Array of numbers to sort
  int seg, segs;               //Current / total segments
  int size;

  sortData(int* arr, int f, int l, tsgl::ColorFloat c) {
    fi = hi = li = 0;        //Initialize indices
    left = right = 0;        //Initialize bounds
    color = c;
    a = arr;                 //Get a pointer to the array we'll be sorting
    first = f;               //Set the first element we need to worry about
    last = l;                //Set the last element we need to worry about
    depth = 0;               //We start at depth 0
    seg = 0; segs = 1;       //We start on segment -1, with a total of 1 segment
    while(segs < (l-f)) {    //If the current number of segments is more than the # of elements, we're done
      ++depth;               //Otherwise, increment the depth...
      segs *= 2;             //...and double the number of segments
    }
    state = S_SHIFT;         //Start Merging
    size = 2;
  }

  void restart(int l) {
    depth = 0;
    hi = last;
    right = hi+1;
    last = li = l;
    fi = left = first;
    state = S_MERGE;
    size *= 2;
  }

  void sortStep() {
    int tmp, pivot, jump;
    switch(state) {
      case S_SHIFT:
        pivot = jump = segs/2;
        fi = first; li = last;
        hi = (fi + li) / 2;    //Set our half index to the median of our first and last
        for (tmp = depth; tmp > 0; --tmp) {
          jump /= 2;
          if (seg < pivot) {
            pivot -= jump;
            li = hi;           //Set out last index to our old half index
          } else {
            pivot += jump;
            fi = hi+1;         //Set out first index to our old half index plus one
          }
          hi = (fi + li) / 2;  //Set our new half index to the median of our first and last
        }
        left = fi; right = hi+1;
        state = S_MERGE;           //We're ready to start Merging
        break;
      case S_MERGE:
        if (left > right || right > last) {
          seg = 0;                 //Reset our segment(s)
          segs /= 2;               //We're now using half as many segments
          state = (depth-- == 0) ? S_WAIT : S_SHIFT;
        } else if (right > li) {
          ++seg; state = S_SHIFT;  //Move on to the next segment and recalculate our first and last indices
        } else if (left <= hi && a[left] < a[right]) {
          ++left;
        } else {
          tmp = a[right];
          for (int x = right; x > left; --x)
            a[x] = a[x-1];
          a[left] = tmp;
          ++left; ++right; ++hi;
        }
        break;
      default:
        break;
    }
  }
};



/*!
 * \brief Visualization of the bottom-up mergesort algorithm.
 * \details Utilizes the sortData struct and sorts a number of items using the mergesort algorithm.
 * \details Uses lines to represent the items being sorted.
 * \details At the start, the items being sorted are all divided.
 * \details Once items have been sorted in one divided section, then sections are merged and the process repeats itself.
 * \details Different colors represent different sections being sorted.
 * \details Once all items have been sorted and merged, the animation stops and all lines are colored white.
 */
void mergeSortFunction(ThreadSynth& voice, tsgl::Canvas* can, int threads, int size, bool audio, bool graphics) {
  const int IPF = 1;      // Iterations per frame
  const int maxNumber = can.getWindowHeight();
  int* numbers = new int[size];       // Array to store the data
  tsgl::Rectangle** rectangles = new tsgl::Rectangle*[size];

  float start = -can.getWindowWidth() * .45;
  float width = can.getWindowWidth() * .9 / size;

  #if TEST
  int* testArray = new int[size];
  #endif

  for (int i = 0; i < size; i++)
    #if TEST
    {
    int n = rand() % maxNumber;
    numbers[i] = n;
    testArray[i] = n;
    }
    #else
    numbers[i] = rand() % maxNumber;
    #endif

  if (graphics) {
    for (int i = 0; i < size; i++) {
    rectangles[i] = new tsgl::Rectangle(start + i * width, 0, 0, width, numbers[i], 0, 0, 0, tsgl::RED);
    rectangles[i]->setIsOutlined(false);
    can.add(rectangles[i]);
    }
  }

  int bs = size / threads;
  int ex = size % threads;
  sortData** sd = new sortData*[1];
  int f = 0;
  int l = (ex == 0) ? bs-1 : bs;
  for (int i = 0; i < threads; ++i) {
    sd[i] = new sortData(numbers,f,l,tsgl::Colors::highContrastColor(i));
    f = l+1;
    if (i < ex-1) l += (bs + 1);
    else          l += bs;
  }
  {
    //std::cout << tid << std::endl;
    while (true) {
      int tid = 0;
      can.sleep();
      if (sd[tid]->state == S_WAIT) {  //Merge waiting threads
        voice.stop();

        if ((tid % sd[tid]->size) > 0) {
          sd[tid]->state = S_DONE;
        } else {
          int next = tid+sd[tid]->size/2;
          if (next < threads && sd[next]->state == S_DONE) {
            sd[next]->state = S_HIDE;
            
            sd[tid]->restart(sd[next]->last);
          }
        }
      }
      for (int i = 0; i < IPF; i++)
        sd[tid]->sortStep();

      can.pauseDrawing();

      double number;
      tsgl::ColorFloat color;
      MergeState state = sd[tid]->state;
      if (state != S_HIDE && state != S_DONE) {
        for (int i = sd[tid]->first; i < sd[tid]->last; ++i) {
          number = numbers[i];
          if (graphics) {
            if (state == S_WAIT || state == S_DONE) {
              color = tsgl::WHITE;
            } else {
                if (i == sd[tid]->right || i == sd[tid]->left)
                        color = tsgl::WHITE;
                    else if (i < sd[tid]->left)
                        color = sd[tid]->color;
                    else if (i >= sd[tid]->fi && i <= sd[tid]->li)
                        color = tsgl::Colors::blend(sd[tid]->color, tsgl::WHITE, 0.5f);
                    else
                        color = tsgl::Colors::blend(sd[tid]->color, tsgl::BLACK, 0.5f);
            }
            rectangles[i]->setHeight(number);
            rectangles[i]->setColor(color);
          }
          can.resumeDrawing();
          // If we are processing the item, play a sound
          if (i == sd[tid]->left) {
            MidiNote note = Util::scaleToNote<double>(number, std::make_pair(0, MAX_VALUE), std::make_pair(C3, C7));
            if (audio) voice.play(note, Timing::MICROSECOND, 50);
            // voice.play(C2 + (tid * 3) + 60 * (number / maxNumber), Timing::MICROSECOND, 50);
          }
        } 
      }
      if (state == S_WAIT) break;
    }
  }

  #if TEST
  std::sort(testArray, testArray + size);
  for (int i = 0; i < size; i++) {
    assert(numbers[i] == testArray[i]);
  }
  #endif

  for (int i = 0; i < threads; ++i)
    delete sd[i];
  delete [] sd;
  delete [] numbers;
  for (int i = 0; i < size; i++) {
    delete rectangles[i];
  }
  delete [] rectangles;
}

/** @example merge_sort.cpp
 * 
 * Merge sort is a sorting algorithm that can be done in parallel. As a result,
 * each thread involved in the algorithm is assigned an oscillator which plays
 * the pitch of the processed item plus a constant. Each thread has its own base
 * pitch so you can differentiate between them near the end of the sorting process.
 *
 * Parse the parameters\n
 * Create the mixer and oscillators\n
 * Start the merge sort algorithm
 * For each thread:
 * - Process an item in the merge step and set the oscillator pitch accordingly
 * - When complete with job, mute the oscillator 
 */
int main(int argc, char** argv) {
  bool audio = false;
  bool graphics = true;
  if (argc > 1) {
    if (std::string(argv[1]) == "-a") {
      audio = true;
    }
  }
  // int s = (argc > 1) ? atoi(argv[1]) : 1024;
  int s = 1000;
  if (s < 10) s = 10;
  int w = s * 1.3;
  int h = w/2;


  double startTime = omp_get_wtime();
  srand(1876);

    Mixer mixer;
    ThreadSynth voice(&mixer);
      mixer.add(voice);
      voice.setVolume(0.5);
      voice.setEnvelopeActive(false);      
    tsgl::Canvas* c = new tsgl::Canvas(0, 0, w, h, "Bottom-up Merge Sort", tsgl::BLACK);
    mergeSortFunction(voice, c, 1, s, audio, graphics);

  std::cout << "Time taken: " << omp_get_wtime() - startTime << " seconds" << std::endl;
  delete c;
}
