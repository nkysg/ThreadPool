/*************************************************************************
	> File Name: ThreadPoolEx.h
	> Author: nk_ysg 
	> Mail: nk_ysg@163.com 
	> Created Time: 2018-02-24 17:21:28
 ************************************************************************/
#pragma once

#include <thread>

#include <string>
#include <vector>
#include <atomic>

#include "queue.h"

class ThreadPoolEx {

public:
  typedef std::function<void ()> Task; 
  explicit ThreadPoolEx(const std::string& nameArg = std::string("ThreadPoolEx"));
  ~ThreadPoolEx();

  void start(int num_threads);
  void stop();
  const std::string& name() const { return pool_name; }
  void run(Task&& task, int index);

private:
  void workthread(int index);
  void stoptask() {}
  std::string pool_name;
  std::vector<std::thread> threads;
  std::vector<std::shared_ptr<queue<Task>> > queues;
  std::atomic<bool> running;
};
