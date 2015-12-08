#include "Gpio.hpp"
#include <fstream>
#include <iostream>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/types.h>


Gpio::Gpio(short _id, GpioDirection_t _direction)
{
  this->m_id = _id;
  this->regist();
  this->setDirection(_direction);
  this->m_pollingThread = NULL;
}

bool Gpio::setDirection(GpioDirection_t _direction)
{
  std::ofstream f(std::string("/sys/class/gpio/gpio") + std::to_string(this->m_id)+"/direction", std::ios::out);
  if(!f)
  {
    return false;
  }
  std::string direction = "in";
  if(_direction == Gpio::GPIO_OUT)
  {
    direction = "out";
  }
  f << direction ;
  f.close();

  return true;
}

bool Gpio::setValue(GpioState_t _state)
{
  std::ofstream f(std::string("/sys/class/gpio/gpio") + std::to_string(this->m_id)+"/value", std::ios::out);
  if(!f)
  {
    return false;
  }
  short value = 0;
  if(_state == Gpio::GPIO_HIGH)
  {
    value = 1;
  }
  f << value ;
  f.close();

  return true;
}

Gpio::GpioState_t Gpio::getValue()
{
  std::ifstream f(std::string("/sys/class/gpio/gpio") + std::to_string(this->m_id)+"/value", std::ios::in);
  if(!f)
  {
    return Gpio::GPIO_LOW;
  }
  GpioState_t value = Gpio::GPIO_LOW;

  std::string buff;
  getline(f, buff);
  if(buff == "1")
  {
    value = Gpio::GPIO_HIGH;
  }
  return value;
}

bool Gpio::regist()
{
  std::ofstream f("/sys/class/gpio/export", std::ios::out);
  if(!f)
  {
    return false;
  }



  f << std::to_string(this->m_id) ;
  f.close();

  return true;
}

bool Gpio::unregister()
{
  std::ofstream f("/sys/class/gpio/export", std::ios::out);
  if(!f)
  {
    return false;
  }



  f << std::to_string(this->m_id) ;
  f.close();

  return true;
}

bool Gpio::setInterrupt(GpioInterrupt_t _mode, GpioCallback _callback)
{
  std::ofstream f(std::string("/sys/class/gpio/gpio") + std::to_string(this->m_id)+"/edge", std::ios::out);
  if(!f)
  {
    return false;
  }
  std::string edge = "none";
  if(_mode == Gpio::GPIO_RISING)
  {
    edge = "rising";
  }
  else
  {
    if(_mode == Gpio::GPIO_FALLING)
    {
      edge = "falling";
    }
    else
    {
      if(_mode == Gpio::GPIO_BOTH)
      {
        edge = "both";
      }
    }
  }

  f << edge;
  f.close();
  this->m_pollingThread = new std::thread(Gpio::pollFunction, this->m_id, _callback);
  return true;
}

void Gpio::pollFunction(short _id, GpioCallback _callback)
{
  struct pollfd fdPoll;
  std::string pollingFile = std::string("/sys/class/gpio/gpio") + std::to_string(_id)+"/value";
  int fd = open (pollingFile.c_str(), O_RDONLY );
  if(fd < 0)
  {
    std::cerr << "fd poll error" << std::endl;
    return;
  }

  fdPoll.fd = fd;
  fdPoll.events = POLLPRI | POLLERR;
  fdPoll.revents = 0;
  while(true)
  {
    int c;
    read(fd,&c, 1);
    int res = poll(&fdPoll, 1, -1);
    if(res < 0)
    {
      std::cerr << "poll error" << std::endl;
      return;
    }
    if(res == 0)
    {
      std::cerr << "timeout" << std::endl;
    }
    if (fdPoll.revents & POLLPRI)
    {
      lseek(fdPoll.fd, 0, SEEK_SET);
      if(_callback != NULL)
      {
        _callback(_id);
      } 
    }

  }
}

Gpio::~Gpio()
{
  this->unregister();
  if(this->m_pollingThread != NULL)
  {
    delete this->m_pollingThread; 
  }
}
