#include <fstream>
#include <vector>
#include <map>

void printUnloaded(std::ostream& out, int page, int frame);
void printLoaded(std::ostream& out, int page, int frame);
void printAlreadyLoaded(std::ostream& out, int page, int frame);
void printPageFaults(std::ostream& out, int pageFaults);
template <class Strategy>
void simulate(std::ostream& out, size_t pageCount, size_t frameCount, std::vector<int> const& pageRequests);

class FIFOStrategy;

class OptimalStrategy;

class LRUStrategy;

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
  simulate<FIFOStrategy>(out, pageCount, frameCount, pageRequests);
  
  // Do "Optimal" simulation
  out << "\nOptimal\n";
  simulate<OptimalStrategy>(out, pageCount, frameCount, pageRequests);
  
  // Do "LRU" simulation
  out << "\nLRU\n";
  simulate<LRUStrategy>(out, pageCount, frameCount, pageRequests);
  
  return 0;
}

struct Page
{
  int pageNumber;
  int nextRequest;
  int previousRequest;
  
  Page(int pageNumber, int nextRequest, int previousRequest) : pageNumber(pageNumber), nextRequest(nextRequest),
                                                               previousRequest(previousRequest)
  {}
};

template <class Strategy>
void simulate(std::ostream& out, size_t pageCount, size_t frameCount, std::vector<int> const& pageRequests)
{
  Strategy strategy;
  std::map<int, int> pageMapping;
  std::vector<Page> frames(frameCount, Page(-1, pageRequests.size(), -1));
  
  for (size_t i = 0; i < pageCount; ++i)
  {
    pageMapping[i] = -1;
  }
  
  size_t pageFaults = 0;
  
  for (size_t i = 0; i < pageRequests.size(); ++i)
  {
    int const page = pageRequests[i];
    int const mappedFrame = pageMapping[page];
    int const nextFrame = strategy.requestFrameForPage(page, frames, pageMapping);
    
    if (mappedFrame == -1)
    {
      // The requested page is not yet loaded into a frame
      if (frames[nextFrame].pageNumber != -1)
      {
        // The next frame already has a page loaded
        pageMapping[frames[nextFrame].pageNumber] = -1;
        printUnloaded(out, frames[nextFrame].pageNumber, nextFrame);
      }
      // Find the next request for the page currently being requested
      int nextRequest = pageRequests.size();
      for (size_t j = i + 1; j < pageRequests.size(); ++j)
      {
        if (pageRequests[j] == page)
        {
          nextRequest = j;
          break;
        }
      }
      frames[nextFrame] = Page(page, nextRequest, i);
      pageMapping[page] = nextFrame;
      printLoaded(out, page, nextFrame);
      pageFaults += 1;
    }
    else
    {
      // The requested page is already loaded into a frame
      frames[pageMapping[page]].previousRequest = i;
      printAlreadyLoaded(out, page, mappedFrame);
    }
  }
  printPageFaults(out, pageFaults);
}

class FIFOStrategy
{
  size_t m_nextFrame;
public:
  FIFOStrategy() : m_nextFrame(0)
  {}
  
  size_t requestFrameForPage(size_t const page, std::vector<Page> const& frames, std::map<int, int> const& pageMapping)
  {
    size_t frame = m_nextFrame;
    if (pageMapping.at(page) == -1)
    {
      m_nextFrame = (m_nextFrame + 1) % frames.size();
    }
    return frame;
  }
};

class OptimalStrategy
{
public:
  size_t requestFrameForPage(size_t const page, std::vector<Page> const& frames, std::map<int, int> const& pageMapping)
  {
    // Select the frame with the page that will not be requested for the longest time.
    size_t selectedFrame = 0;
    for (size_t i = 0; i < frames.size(); ++i)
    {
      // If there is an empty frame, select it
      if (frames[i].pageNumber == -1) return i;
      
      if (frames[i].nextRequest > frames[selectedFrame].nextRequest)
      {
        selectedFrame = i;
      }
    }
    return selectedFrame;
  }
};

class LRUStrategy
{
public:
  size_t requestFrameForPage(size_t const page, std::vector<Page> const& frames, std::map<int, int> const& pageMapping)
  {
    size_t selectedFrame = 0;
    for (size_t i = 0; i < frames.size(); ++i)
    {
      // If there is an empty frame, select it
      if (frames[i].pageNumber == -1) return i;
      
      if (frames[i].previousRequest < frames[selectedFrame].previousRequest)
      {
        selectedFrame = i;
      }
    }
    return selectedFrame;
  }
};

void printUnloaded(std::ostream& out, int page, int frame)
{
  out << "Page " << page << " unloaded from Frame " << frame << ", " << std::flush;
}

void printLoaded(std::ostream& out, int page, int frame)
{
  out << "Page " << page << " loaded into Frame " << frame << "\n" << std::flush;
}

void printAlreadyLoaded(std::ostream& out, int page, int frame)
{
  out << "Page " << page << " already in Frame " << frame << "\n" << std::flush;
}

void printPageFaults(std::ostream& out, int pageFaults)
{
  out << pageFaults << " page faults\n";
}
