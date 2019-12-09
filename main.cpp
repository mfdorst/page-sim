#include <fstream>
#include <vector>
#include <map>
#include <queue>

void printUnloaded(std::ostream& out, int page, int frame);
void printLoaded(std::ostream& out, int page, int frame);
void printAlreadyLoaded(std::ostream& out, int page, int frame);
void printPageFaults(std::ostream& out, int pageFaults);
void simulate(std::ostream& out, size_t pageCount, size_t frameCount, std::vector<int> const& pageRequests);

struct Page
{
  int pageNumber;
  int nextRequest;
};

int main()
{
  std::ifstream in("input.txt");
  std::ofstream out("output.txt");
  
  // Read metadata - how many pages, frames, and requests to simulate
  size_t pageCount, frameCount, requestCount;
  in >> pageCount >> frameCount >> requestCount;
  
  // Read all page requests into a vector
  std::vector<int> pageRequests(requestCount);
  for (size_t i = 0; i < requestCount; ++i)
  {
    in >> pageRequests[i];
  }
  
  // Do "FIFO" simulation
  out << "FIFO\n";
  simulate(out, pageCount, frameCount, pageRequests);
  
  // Do "Optimal" simulation
  out << "\nOptimal\n";
  simulate(out, pageCount, frameCount, pageRequests);
  
  // Do "LRU" simulation
  simulate(out, pageCount, frameCount, pageRequests);
  
  return 0;
}

void simulate(std::ostream& out, size_t pageCount, size_t frameCount, std::vector<int> const& pageRequests)
{
  std::map<int, int> pageMapping;
  std::vector<int> frames(frameCount, -1);
  
  for (size_t i = 0; i < pageCount; ++i)
  {
    pageMapping[i] = -1;
  }
  
  size_t nextFrame = 0, pageFaults = 0;
  
  for (size_t i = 0; i < pageRequests.size(); ++i)
  {
    int const page = pageRequests[i];
    int const mappedFrame = pageMapping[page];
    
    if (mappedFrame == -1)
    {
      // The requested page is not yet loaded into a frame
      if (frames[nextFrame] != -1)
      {
        // The next frame already has a page loaded
        pageMapping[frames[nextFrame]] = -1;
        printUnloaded(out, frames[nextFrame], nextFrame);
      }
      frames[nextFrame] = page;
      pageMapping[page] = nextFrame;
      printLoaded(out, page, nextFrame);
      nextFrame = (nextFrame + 1) % frames.size();
      pageFaults += 1;
    }
    else
    {
      // The requested page is already loaded into a frame
      printAlreadyLoaded(out, page, mappedFrame);
    }
  }
  printPageFaults(out, pageFaults);
}

void printUnloaded(std::ostream& out, int page, int frame)
{
  out << "Page " << page << " unloaded from Frame " << frame << ", ";
}

void printLoaded(std::ostream& out, int page, int frame)
{
  out << "Page " << page << " loaded into frame " << frame << "\n";
}

void printAlreadyLoaded(std::ostream& out, int page, int frame)
{
  out << "Page " << page << " already in frame " << frame << "\n";
}

void printPageFaults(std::ostream& out, int pageFaults)
{
  out << pageFaults << " page faults\n";
}