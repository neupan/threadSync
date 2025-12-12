#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

class SyncQueue {
 public:
  SyncQueue(int maxSize) : mMaxSize(maxSize) {}

  void push(int item) {
    {
      std::unique_lock<std::mutex> lock(mMutex);
      mCondFull.wait(lock, [this] { return mQue.size() < mMaxSize; });
      mQue.push(item);
      std::cout << "thread " << std::this_thread::get_id() << " pushed " << item
                << std::endl;
    }
    mCondEmpty.notify_one();
  }

  int pop() {
    int item = 0;
    {
      std::unique_lock<std::mutex> lock(mMutex);
      mCondEmpty.wait(lock, [this] { return !mQue.empty(); });
      item = mQue.front();
      mQue.pop();
      std::cout << "thread " << std::this_thread::get_id() << " popped " << item
                << std::endl;
    }
    mCondFull.notify_one();
    return item;
  }

 private:
  std::queue<int> mQue;
  int mMaxSize;
  std::mutex mMutex;
  std::condition_variable mCondEmpty;
  std::condition_variable mCondFull;
};

int main() {
  SyncQueue syncQueue(10);
  std::thread producer_threads[5];
  std::thread consumer_threads[5];
  for (int i = 0; i < 5; i++) {
    producer_threads[i] = std::thread([&syncQueue, i] {
      while (true) {
        syncQueue.push(i + 100);
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 3));
      }
    });
  }
  for (int i = 0; i < 5; i++) {
    consumer_threads[i] = std::thread([&syncQueue] {
      while (true) {
        syncQueue.pop();
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 3));
      }
    });
  }

  for (int i = 0; i < 5; i++) {
    producer_threads[i].join();
  }
  for (int i = 0; i < 5; i++) {
    consumer_threads[i].join();
  }

  return 0;
}
