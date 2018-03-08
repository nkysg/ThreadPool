/*************************************************************************
	> File Name: queue.h
	> Author: nk_ysg
	> Mail: nk_ysg@163.com 
	> Created Time: 2018-03-06 17:00:24
 ************************************************************************/

#pragma once

#include <vector>
#include <mutex>
#include <condition_variable>

template<typename T>
class queue {
public:
  queue(): valid_flag(true)
  {}

  bool push(T&& x) {
    std::unique_lock<std::mutex> lk(mu);
    if (!valid_flag) {
      return false;
    }
    if (elems.empty()) {
      not_empty.notify_one();
    }
    elems.push_back(std::move(x));
    return true;
  }

  void fetchall(std::vector<T>& c) {
    std::unique_lock<std::mutex> lk(mu);
    while (elems.empty()) {
      not_empty.wait(lk); 
    }
    c.swap(elems);
  }

  size_t size() {
    std::unique_lock<std::mutex> lk(mu);
    return elems.size();
  }

  void setinvalid() {
    std::unique_lock<std::mutex> lk(mu);
    valid_flag = false;
  }

  bool valid() {
    std::unique_lock<std::mutex> lk(mu);
    return valid_flag;
  }
  private:
  std::mutex mu;
  std::condition_variable not_empty;
  std::vector<T> elems;
  bool valid_flag;
};
