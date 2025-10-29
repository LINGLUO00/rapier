#ifndef __RAPIER_CONFIG_H__
#define __RAPIER_CONFIG_H__

#include<memory>
#include<sstream>
#include<boost/lexical_cast.hpp>
#include<yaml-cpp/yaml.h>
#include"./log.h"
#include<vector>
#include<list>
#include<map>
#include<set>
#include<unordered_map>
#include<unordered_set>
#include<functional>


namespace rapier {
  class ConfigVarBase{
  public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description="")
      :_name(name)
      ,_description(description){
        std::transform(_name.begin(),_name.end(), _name.begin(), ::tolower);
      }
    virtual ~ConfigVarBase();
    const std::string& getName() const {return _name;}//get the name of the config variable
    const std::string& getDescription() const {return _description;}//get the description of the config variable

    virtual std::string toString()=0;//transform _var to string
    virtual bool fromString(const std::string& val)=0;//transform val from string to set _var 
    virtual std::string getTypeName() const = 0;

  protected:
    std::string _name;
    std::string _description;
  };

  template<class F, class T>
  class LexicalCast{
  public:
    T operator()(const F& v){
      return boost::lexical_cast<T>(v);
    }
  };
  
  template<class T>
    class LexicalCast<std::string,std::vector<T>> {
    public:
      std::vector<T> operator()(const std::string& str){
        YAML::Node node = YAML::Load(str);
        typename std::vector<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++) {
          ss.str("");
          ss<<node[i];
          vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
      }
    };
 
  template<class F>
    class LexicalCast<std::vector<F>, std::string>{
    public:
      std::string operator()(const std::vector<F>& v){
        YAML::Node node;
        for (auto& i : v) {
          node.push_back(YAML::Load(LexicalCast<F, std::string>()(i)));
        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
      }
    };
    

  template<class T>
    class LexicalCast<std::string,std::list<T>> {
    public:
      std::list<T> operator()(const std::string& str){
        YAML::Node node = YAML::Load(str);
        typename std::list<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++) {
          ss.str("");
          ss<<node[i];
          vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
      }
    };
 
  template<class F>
    class LexicalCast<std::list<F>, std::string>{
    public:
      std::string operator()(const std::list<F>& v){
        YAML::Node node;
        for (auto& i : v) {
          node.push_back(YAML::Load(LexicalCast<F, std::string>()(i)));
        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
      }
    };

  template<class T>
    class LexicalCast<std::string,std::set<T>> {
    public:
      std::set<T> operator()(const std::string& str){
        YAML::Node node = YAML::Load(str);
        typename std::set<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++) {
          ss.str("");
          ss<<node[i];
          vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
      }
    };
 
  template<class F>
    class LexicalCast<std::set<F>, std::string>{
    public:
      std::string operator()(const std::set<F>& v){
        YAML::Node node;
        for (auto& i : v) {
          node.push_back(YAML::Load(LexicalCast<F, std::string>()(i)));
        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
      }
    };
  
    
  template<class T>
    class LexicalCast<std::string,std::unordered_set<T>> {
    public:
      std::unordered_set<T> operator()(const std::string& str){
        YAML::Node node = YAML::Load(str);
        typename std::unordered_set<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++) {
          ss.str("");
          ss<<node[i];
          vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
      }
    };
 
  template<class F>
    class LexicalCast<std::unordered_set<F>, std::string>{
    public:
      std::string operator()(const std::unordered_set<F>& v){
        YAML::Node node;
        for (auto& i : v) {
          node.push_back(YAML::Load(LexicalCast<F, std::string>()(i)));
        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
      }
    };

  template<class T>
    class LexicalCast<std::string,std::map<std::string, T>> {
    public:
      std::map<std::string, T> operator()(const std::string& str){
        YAML::Node node = YAML::Load(str);
        typename std::map<std::string, T> vec;
        std::stringstream ss;
        for (auto it = node.begin();it!=node.end();++it){
          ss.str("");
          ss<<it->second;
          vec.insert(std::make_pair(it->first.Scalar(),LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
      }
    };
 
  template<class F>
    class LexicalCast<std::map<std::string, F>, std::string>{
    public:
      std::string operator()(const std::map<std::string, F>& v){
        YAML::Node node;
        for (auto& i : v) {
          node[i.first] = YAML::Load(LexicalCast<F, std::string>()(i.second));
        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
      }
    };

  template<class T>
    class LexicalCast<std::string,std::unordered_map<std::string, T>> {
    public:
      std::unordered_map<std::string, T> operator()(const std::string& str){
        YAML::Node node = YAML::Load(str);
        typename std::unordered_map<std::string, T> vec;
        std::stringstream ss;
        for (auto it = node.begin();it!=node.end();++it){
          ss.str("");
          ss<<it->second;
          vec.insert(std::make_pair(it->first.Scalar(),LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
      }
    };
 
  template<class F>
    class LexicalCast<std::unordered_map<std::string, F>, std::string>{
    public:
      std::string operator()(const std::unordered_map<std::string, F>& v){
        YAML::Node node;
        for (auto& i : v) {
          node[i.first] = YAML::Load(LexicalCast<F, std::string>()(i.second));
        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
      }
    };

  //FromStr T operator()(const std::string&)
  //ToStr std::string operator()(const T&)
  template<class T, class FromStr=LexicalCast<std::string,T>, class ToStr=LexicalCast<T, std::string>>
  class ConfigVar : public ConfigVarBase{
  public:
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;//当一个变化事件发生的时候，应该要能够callback通知到代码级别
    ConfigVar(const std::string& name, const T& default_val, const std::string& description="")
      :ConfigVarBase(name,description)
      ,_val(default_val){}

    std::string toString() override {
      try{
        //return boost::lexical_cast<std::string> (_val);
        return ToStr()(_val);
      }catch(const std::exception& e){
        RAPIER_LOG_ERROR(RAPIER_LOG_ROOT())<<"ConfigVar::toString execprion"
          <<e.what()<<"convert:"<<typeid(_val).name()<<" to string";
      }
      return "";
    }

    virtual bool fromString(const std::string& val) override{
      try{
        //_val = boost::lexical_cast<T>(val);
        setValue(FromStr()(val));
      }catch(const std::exception& e){
        RAPIER_LOG_ERROR(RAPIER_LOG_ROOT())<<"ConfigVar::toString execprion"
          <<e.what()<<"convert: string to "<<typeid(_val).name()<<" to string";
      }
      return false;
    }

    const T& getValue() const {return _val;}
    void setValue(const T& v) {
      if(v==_val){ return;}
      for(auto& i:_cbs){
        i.second(_val,v);
      }
      _val=v;
    }
    std::string getTypeName() const override {return typeid(T).name();}

    void addListener(uint64_t key, on_change_cb cb){
      _cbs[key]=cb;
    }
    void delListener(uint64_t key){
      _cbs.erase(key);
    }
    void clearListener(){
      _cbs.clear();
    }
    on_change_cb getListener(uint64_t key){
      auto it = _cbs.find(key);
      return it == _cbs.end() ? nullptr : it->second;
    }
  private:
    T _val;
    //变更回调函数组
    std::map<uint64_t, on_change_cb> _cbs;
  };

  class Config{
  public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name, 
        const T& default_val, const std::string& description=""){
      auto it = _datas.find(name);
      if (it != _datas.end()) {
        auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        if (tmp) {
          RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<"Lookup name="<<name<<"exists";
          return tmp;
        }else {
          RAPIER_LOG_ERROR(RAPIER_LOG_ROOT())<<"Lookup name="<<name<<" exists but type not "
            <<typeid(T).name()<<" real type: "<<it->second->getTypeName()
            <<" "<<it->second->toString();
          return nullptr;
        }        
      }
      if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos){
        RAPIER_LOG_ERROR(RAPIER_LOG_ROOT())<<"Lookup name invalid "<<name;
        throw std::invalid_argument(name);
      }
      typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_val, description));
      _datas[name]=v;
      return  v;
    } 
    
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name){
      auto it = _datas.find(name);
      if(it == _datas.end()){
        return nullptr;
      }
      return std::dynamic_pointer_cast<ConfigVar<T>> (it->second);
    } 
    static void LoadFromYaml(const YAML::Node& root);

    static ConfigVarBase::ptr LookupBase(const std::string& name);
  private:
    static ConfigVarMap _datas;
  };


}

#endif
