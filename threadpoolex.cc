#include "threadpoolex.h"

ThreadPoolEx::ThreadPoolEx(const std::string& nameArg)
  : pool_name(nameArg) {
  }

ThreadPoolEx::~ThreadPoolEx() {
  if (running.load()) {
    stop();
  }
}

void ThreadPoolEx::start(int num_threads) {
  running.store(true);
  threads.reserve(num_threads);
  for (int i = 0; i < num_threads;++i) {
    queues.emplace_back(new queue<Task>);
    threads.emplace_back(std::thread(std::bind(&ThreadPoolEx::workthread,this, i)));
  }
}

void ThreadPoolEx::stop() {
  running.store(false);     
  for (size_t i = 0; i < threads.size(); ++i) {
      queues[i]->push(std::bind(&ThreadPoolEx::stoptask, this));
  }
  for (auto& th: threads) {
    th.join();
  }
}

void ThreadPoolEx::run(Task&& task,int index) {
  queues[index]->push(std::move(task));
}

void ThreadPoolEx::workthread(int index) {
  std::vector<Task> tasks;
  while (running.load() == 1 || queues[index]->size()) {
    tasks.clear();
    queues[index]->fetchall(tasks);
    for (auto& task: tasks) {
      task();
    }
  }
  queues[index]->setinvalid();
}
