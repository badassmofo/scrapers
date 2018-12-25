//
//  main.cpp
//  events
//
//  Created by Rory B. Bellows on 19/12/2017.
//  Copyright © 2017 Rory B. Bellows. All rights reserved.
//

#include <iostream>
#include <functional>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <map>
#include <mutex>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

template <typename F>class func_traits : public func_traits<decltype(&F::operator())> {};

template <typename C, typename R, typename ...A> class func_traits<R(C::*)(A...) const> {
public:
  typedef std::function<R(A...)> function;
};

template <typename F> typename func_traits<F>::function to_function(F& lambda) {
  return static_cast<typename func_traits<F>::function>(lambda);
}

template <typename T=void> class func_wrapper final {
  struct final {
    void * function;
    const std::type_info * signature;
  } _func;
  bool sig_check;
  
public:
  template <typename F>
  func_wrapper(F lambda, bool check=false) : sig_check(check) {
    auto __func = new decltype(to_function(lambda))(to_function(lambda));
    _func.function  = static_cast<void *>(__func);
    _func.signature = &typeid(__func);
  }
  
  ~func_wrapper() {
    delete static_cast<std::function<void()>*>(_func.function);
  }
  
  template <typename R> func_wrapper<R> as() {
    return func_wrapper<R>(*static_cast<std::function<R()>*>(_func.function));
  }
  
  template <typename ...A> T operator() (A&& ...args) {
    auto __func = static_cast<std::function<T(A ...)>*>(_func.function);
    
    if (sig_check && typeid(__func) != *(_func.signature))
      throw std::bad_typeid();
    
    if (!std::is_same<T, void>::value)
      return (*__func)(std::forward<A>(args)...);
      
    (*__func)(std::forward<A>(args)...);
  }
};

using event_fn_t = func_wrapper<>;
using event_listener_t = std::tuple<event_fn_t*, bool>;

class emitter_t {
  std::map<std::string, std::vector<event_listener_t>> events;
  std::mutex _events_mtx;
  
public:
  emitter_t() {
    std::for_each(events.begin(), events.end(), [](std::pair<std::string, std::vector<event_listener_t>> pair) {
      std::vector<event_listener_t>& listeners = pair.second;
      std::for_each(listeners.begin(), listeners.end(), [](event_listener_t& listener) {
        delete std::get<0>(listener);
      });
    });
    events.clear();
  }
  
  template <typename F> void on(const std::string& event, F&& lambda) {
    std::unique_lock<std::mutex> locker(_events_mtx);
    events[event].emplace_back(new event_fn_t{std::forward<F>(lambda)}, false);
  }
  
  template <typename F> void once(const std::string& event, F&& lambda) {
    std::unique_lock<std::mutex> locker(_events_mtx);
    events[event].emplace_back(new event_fn_t{std::forward<F>(lambda)}, true);
  }
  
  template <typename ...A> void emit(const std::string& event, A&& ...args) {
    std::unique_lock<std::mutex> locker(_events_mtx);
    std::vector<event_listener_t>& listeners = events[event];
    std::vector<std::vector<event_listener_t>::iterator> once_listener;
    
    for (auto listener = listeners.begin(); listener != listeners.end(); listener++) {
      event_fn_t* on = std::get<0>(*listener);
      bool once = std::get<1>(*listener);
      
      if (on) {
        (*on) (std::forward<A>(args)...);
        if (once) {
          delete on;
          std::get<0>(*listener) = nullptr;
          once_listener.emplace_back(listener);
        }
      }
    }
    
    std::for_each(once_listener.begin(), once_listener.end(), [&listeners](std::vector<event_listener_t>::iterator& iterator) {
      listeners.erase(iterator);
    });
    
    listeners.shrink_to_fit();
  }
  
  ssize_t count(const std::string& event) {
    std::unique_lock<std::mutex> locker(_events_mtx);
    auto event_listeners = events.find(event);
    
    if (event_listeners == events.end())
      return 0;
    
    return events[event].size();;
  }
};

#include <sstream>
#include <thread>

int main(int argc, const char * argv[]) {
  emitter_t emitter;
  
  emitter.on("event", [&emitter](int data) {
    std::ostringstream osstream;
    osstream << "[Listener 1] data: " << data << '\n';
    std::cout<<osstream.str();
  });
  
  emitter.on("event", [&emitter](int data) {
    std::ostringstream osstream;
    osstream << "[Listener 2] data: " << data << '\n';
    std::cout<<osstream.str();
  });
  
  emitter.once("event", [&emitter](int data) {
    std::ostringstream osstream;
    osstream << "[Once Listener] data: " << data << '\n';
    std::cout<<osstream.str();
  });
  
  std::cout<< emitter.count("event")<<" listeners for 'event'\n";
  
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; i++) {
    threads.emplace_back([&emitter, i]() {
      emitter.emit("event", i);
    });
  }
  
  for (auto &t: threads)
    t.join();
  
  std::cout<< emitter.count("event") << " listeners for 'event'\n";
  
  return 0;
}
