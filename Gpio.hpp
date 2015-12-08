#ifndef GPIO_HPP
#define GPIO_HPP
#include <thread>

typedef bool (*GpioCallback)(short);

class Gpio
{
  private:
    short m_id;
    std::thread *m_pollingThread;

  public:
    typedef enum
    {
      GPIO_IN,
      GPIO_OUT
    } GpioDirection_t;

    typedef enum
    {
      GPIO_LOW,
      GPIO_HIGH
    } GpioState_t;

    typedef enum
    {
      GPIO_NONE,
      GPIO_RISING,
      GPIO_FALLING,
      GPIO_BOTH
    } GpioInterrupt_t;

    Gpio(short _id, GpioDirection_t _direction = GPIO_OUT);
    bool setDirection(GpioDirection_t _direction);
    bool setValue(GpioState_t _state);
    GpioState_t getValue();
    bool regist();
    bool unregister();
    bool setInterrupt(GpioInterrupt_t _mode, GpioCallback _callback = NULL);
    static void pollFunction(short _id, GpioCallback _callback);
    ~Gpio();
};


#endif
