#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;
std::condition_variable cv;

struct Node {
  int data;
  Node *next;
};

Node *head = nullptr;

void producer() {
  while (true) {
    {
      std::unique_lock<std::mutex> lock(mtx);
      Node *new_node = new Node();
      new_node->data = rand() % 100;
      new_node->next = head;
      head = new_node;
      std::cout << "thread " << std::this_thread::get_id() << " produced "
                << new_node->data << std::endl;
    }  // 锁在这里释放
    cv.notify_all();  // 通知应该在锁释放后
    std::this_thread::sleep_for(std::chrono::seconds(rand() % 3));
  }
}

void consumer() {
  while (true) {
    Node *node = nullptr;
    {
      std::unique_lock<std::mutex> lock(mtx);
      cv.wait(lock, [] { return head != nullptr; });
      node = head;
      head = node->next;
      std::cout << "thread " << std::this_thread::get_id() << " consumed "
                << node->data << std::endl;
    }  // 锁在这里释放
    delete node;  // 在锁外删除节点，避免持有锁时间过长
  }
}

int main() {
  std::thread producer_threads[5];
  std::thread consumer_threads[5];
  for (int i = 0; i < 5; i++) {
    producer_threads[i] = std::thread(producer);
  }
  for (int i = 0; i < 5; i++) {
    consumer_threads[i] = std::thread(consumer);
  }
  for (int i = 0; i < 5; i++) {
    producer_threads[i].join();
  }
  for (int i = 0; i < 5; i++) {
    consumer_threads[i].join();
  }
  return 0;
}