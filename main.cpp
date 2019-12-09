#include <fstream>
#include <vector>
#include <map>
#include <queue>

void printUnloaded(std::ostream& out, int page, int frame);
void printLoaded(std::ostream& out, int page, int frame);
void printAlreadyLoaded(std::ostream& out, int page, int frame);
void printPageFaults(std::ostream& out, int pageFaults);

int main()
{
  std::ifstream in("input.txt");
  std::ofstream out("output.txt");
  
  int pageCount, frameCount, requestCount;
  in >> pageCount >> frameCount >> requestCount;
  
  std::vector<int> pageRequests(requestCount);
  
  for (int i = 0; i < requestCount; ++i)
  {
    in >> pageRequests[i];
  }
  
  std::vector<int> frames(frameCount, -1);
  std::map<int, int> pageMapping;
  
  for (int i = 0; i < pageCount; ++i)
  {
    pageMapping[i] = -1;
  }
  
  out << "FIFO\n";
  
  int nextFrame = 0, pageFaults = 0;
  
  for (int i = 0; i < requestCount; ++i)
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
      nextFrame = (nextFrame + 1) % frameCount;
      pageFaults += 1;
    }
    else
    {
      // The requested page is already loaded into a frame
      printAlreadyLoaded(out, page, mappedFrame);
    }
  }
  printPageFaults(out, pageFaults);
  
  out << "\nOptimal\n";
  
  return 0;
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
