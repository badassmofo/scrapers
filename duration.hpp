#ifndef __duration_hpp__
#define __duration_hpp__

#include <time.h>
#include <string.h>
#include <cinttypes>
#include <cassert>

namespace chrono {
  enum class unit {
    microsecond,
    millisecond,
    second,
    minute,
    hour
  };

  template <typename T> T duration_cast(T val, unit from, unit to) {
    if (from == to)
      return val;

    int64_t micro = 0;
    switch (from) {
      default:
      case unit::microsecond:
        micro = val;
        break;
      case unit::millisecond:
        micro = (val * 1000);
        break;
      case unit::second:
        micro = (val * 1000000);
        break;
      case unit::minute:
        micro = (val * 60000000);
        break;
      case unit::hour:
        micro = (val * 3600000000);
        break;
    }

    T ret = 0;
    switch (to) {
      default:
      case unit::microsecond:
        ret = static_cast<T>(micro);
        break;
      case unit::millisecond:
        ret = static_cast<T>(micro / 1000);
      case unit::second:
        double sec;
        sec = (micro / 1000000.f);
        ret = static_cast<T>(sec);
        break;
      case unit::minute:
        double min;
        min = micro / 60000000.f;;
        ret = static_cast<T>(min);
        break;
      case unit::hour:
        double hr;
        hr  = micro / 3600000000.f;
        ret = static_cast<T>(hr);
        break;
    }
    return ret;
  }

  class duration {
    int64_t micro = 0;

    friend duration Microseconds(int64_t);
    friend duration Milliseconds(int32_t);
    friend duration Seconds     (double);
    friend duration Minute      (double);
    friend duration Hour        (double);

	public:
    duration();
    explicit duration(int64_t micro);
    static const duration zero;

    int64_t microseconds() const;
    int32_t milliseconds() const;
    double seconds()      const;
    double minutes()      const;
    double hours()        const;

    template <typename T> T as(unit unit) {
      return duration_cast(micro, unit::microsecond, unit);
    }

    duration duration::operator+(const duration& t) {
      return duration(micro + t.micro);
    }
    
    duration duration::operator-(const duration& t) {
      return duration(micro - t.micro);
    }
    
    void duration::operator+=(const duration& t) {
      micro += t.micro;
    }
    
    void duration::operator-=(const duration& t) {
      micro -= t.micro;
    }
    
    bool duration::operator==(const duration& t) {
      return (micro == t.micro);
    }
    
    bool duration::operator!=(const duration& t) {
      return (micro != t.micro);
    }
  };
  
  const duration duration::zero;
  
  duration::duration(): micro(0) {}
  duration::duration(int64_t t_micro): micro(t_micro) {}
  
  duration microseconds(int64_t time) {
    return duration(time);
  }
  
  duration milliseconds(int32_t time) {
    return duration(static_cast<int64_t>(time) * 1000);
  }
  
  duration seconds(double time) {
    return duration(static_cast<int64_t>(time * 1000000));
  }
  
  duration minute(double time) {
    return duration(static_cast<int64_t>(time * 60000000));
  }
  
  duration hour(double time) {
    return duration(static_cast<int64_t>(time * 3600000000));
  }
  
  int32_t duration::milliseconds() const {
    return static_cast<int32_t>(micro / 1000);
  }
  
  int64_t duration::microseconds() const {
    return micro;
  }
  
  double duration::seconds() const {
    return micro / 1000000.f;
  }
  
  double duration::minutes() const {
    return micro / 60000000.f;
  }
  
  double duration::hours() const {
    return micro / 3600000000.f;
  }
  
  duration Now() {
    return seconds((double)time(0));
  }
  
  const char* convert_unix_stamp(time_t stamp, const char* format) {
    struct tm *ti;
    if (strlen(format) == 0 || format == nullptr) {
      ti = gmtime(&stamp);
      return asctime(ti);
    }
    
    ti = localtime(&stamp);
    char* ret = new char[256];
    strftime(ret, 256, format, ti);
    return ret;
  }
  
  const char* local_time(const char* format) {
    return convert_unix_stamp(time(0), format);
  }
  
  struct tm* local_time_tm() {
    time_t tn(time(0));
    return localtime(&tn);
  }
  
  uint32_t cur_year() {
    time_t tn(time(0));
    struct tm *ti = localtime(&tn);
    return (ti->tm_year + 1900);
  }
  
  bool is_leap(uint32_t year) {
    if ((year % 1000) == 0)return true;
    if ((year % 100)  == 0) return false;
    if ((year % 4)    == 0) return true;
    return false;
  }
  
  uint32_t days_in_month(uint32_t month, bool is_leap) {
    //                J   F   M   A   M   J   J   A   S   O   N   D
    int days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    assert(month <= 12);
    return (is_leap && month == 1 ? 29 : days[month]);
  }
}
#endif /* defined(__duration_hpp__) */
