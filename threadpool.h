/*************************************************************************
	> File Name: threadpool.h
	> Author: nk_ysg 
	> Mail: nk_ysg@163.com 
	> Created Time: 2018-02-24 17:21:28
 ************************************************************************/
#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

#include <deque>
#include <string>
#include <vector>

using std::string;

class ThreadPool {

public:
  typedef std::function<void ()> Task; 
  explicit ThreadPool(const string& nameArg = string("ThreadPool"));
  ~ThreadPool();

  void start(int num_threads);
  void setmaxqueuesize(int max_size) { max_queue_size = max_size; }
  void stop();
  const string& name() const { return pool_name; }
  void run(Task&& f);

private:
  void runinthread();
  bool isfull() const;
  Task take();

  std::mutex queue_mtx;
  std::condition_variable not_empty;
  std::condition_variable not_full;
  size_t max_queue_size;
  string pool_name;
  std::vector<std::thread> threads;
  std::deque<Task> queue;
  bool running;
};


