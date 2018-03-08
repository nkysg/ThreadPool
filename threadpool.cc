#include "threadpool.h"

ThreadPool::ThreadPool(const string& nameArg)
  : queue_mtx(),
  not_empty(),
  not_full(),
  max_queue_size(0),
  pool_name(nameArg),
  running(false)
  {
  }

ThreadPool::~ThreadPool() {
  if (running)
  {
    stop();
  }
}

void ThreadPool::start(int num_threads) {
  running = true;
  threads.reserve(num_threads);
  for (int i = 0; i < num_threads;++i) {
    threads.push_back(std::thread(std::bind(&ThreadPool::workthread,this)));
  }
}

void ThreadPool::stop() {
  {
    std::unique_lock<std::mutex> lock(queue_mtx);
    running = false;     
    not_empty.notify_all();
  }
  for (auto& th: threads)
    th.join();
}

void ThreadPool::run(Task&& task) {
  std::unique_lock<std::mutex> lock(queue_mtx);
  while (isfull())
  {
    not_full.wait(lock);
  }
  queue.push_back(std::move(task));
  not_empty.notify_one();
}


ThreadPool::Task ThreadPool::take() {
  std::unique_lock<std::mutex> lock(queue_mtx);
  while (running && queue.empty())
    not_empty.wait(lock);
  Task task;
  if (!queue.empty())
  {
    task = queue.front();
    queue.pop_front();
    if (max_queue_size > 0)
    {
      not_full.notify_one();
    }
  }
  return task;
}

bool ThreadPool::isfull() const {
  return max_queue_size > 0 && queue.size() >= max_queue_size;
}

void ThreadPool::workthread() {
  while (running) {
    Task task(take());
    if (task) {
      task();
    }
  }
}
