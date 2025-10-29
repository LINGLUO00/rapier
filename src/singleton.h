#ifndef __RAPIER_SINGLETON_H__
#define __RAPIER_SINGLETON_H__

namespace rapier {
  template<class T, class X = void, int N=0>
  class Singleton{
  public:
    static T* GetInstance(){
      static T v;
      return &v;
    }
  };
  template<class T, class X = void, int N=0>
  class SingletonPtr{
  public:
    static std::shared_ptr<T> GetInstance(){
      static std::shared_ptr<T> v(new T);
      return v;
    }
  };

  
}
#endif
