#include"../src/config.h"
#include"../src/log.h"
#include<yaml-cpp/yaml.h>

rapier::ConfigVar<int>::ptr g_int_value_config = rapier::Config::Lookup("system.port", (int)8080, "system port");
rapier::ConfigVar<float>::ptr g_float_valuex_config = rapier::Config::Lookup("system.port", (float)8080, "system port");
rapier::ConfigVar<float>::ptr g_float_value_config = rapier::Config::Lookup("system.value", (float)10.2f, "system value");
rapier::ConfigVar<std::vector<int>>::ptr g_int_vec_config = rapier::Config::Lookup("system.int_vec", std::vector<int>{1,2}, "system int vector");
rapier::ConfigVar<std::list<int>>::ptr g_int_list_config = rapier::Config::Lookup("system.int_list", std::list<int>{3,4}, "system int list");
rapier::ConfigVar<std::set<int>>::ptr g_int_set_config = rapier::Config::Lookup("system.int_set", std::set<int>{5,6}, "system int set");
rapier::ConfigVar<std::unordered_set<int>>::ptr g_int_unordered_set_config = rapier::Config::Lookup("system.int_unordered_set", std::unordered_set<int>{5,6}, "system int unordered_set");
rapier::ConfigVar<std::map<std::string, int>>::ptr g_str_int_map_config = rapier::Config::Lookup("system.str_int_map", std::map<std::string, int>{{"k",6}}, "system str_int map");
rapier::ConfigVar<std::unordered_map<std::string, int>>::ptr g_str_int_unordered_map_config = rapier::Config::Lookup("system.str_int_unordered_map", std::unordered_map<std::string, int>{{"k",6}}, "system str_int unordered_map");


void print_yaml(const YAML::Node& node, int level){
  if(node.IsScalar()){
    RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<< std::string(level*4,' ')
      <<node.Scalar()<<" - "<<node.Type()<<" - "<<level;
  }else if(node.IsNull()){
    RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<< std::string(level*4,' ')
      <<"NULL - "<<node.Type()<<" - "<<level;
  }else if (node.IsMap()) {
    for(auto it = node.begin(); it!=node.end();++it){
      RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<< std::string(level*4,' ')
        <<it->first <<" - "<<it->second.Type()<<" - "<<level;
      print_yaml(it->second, level+1);
    }
  }else if (node.IsSequence()) {
    for(size_t i =0;i<node.size();++i){
      RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<< std::string(level*4,' ')
        << i <<" - "<<node[i].Type()<<" - "<<level;
      print_yaml(node[i], level+1);
    }
  }
}

void test_yaml(){
  YAML::Node root = YAML::LoadFile("bin/conf/log.yml");
  print_yaml(root,0);
  //RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<root;
}

void tets_config(){

  RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<"before: "<<g_int_value_config->getValue();
  RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<"before: "<<g_float_value_config->toString();
//use {} to contain the code block to avoid redefining the parameter v
#define XX(g_val_cnf, name, prefix) \
  { \
    auto& v = g_val_cnf->getValue(); \
    for(auto& i : v ){ \
      RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<#prefix " " #name ": "<<i; \
    } \
    RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<#prefix " " #name "yaml: "<<g_val_cnf->toString(); \
  }

#define XX_M(g_val_cnf, name, prefix) \
  { \
    auto& v = g_val_cnf->getValue(); \
    for(auto& i : v ){ \
      RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<#prefix " " #name ": {" \
        <<i.first<<" - "<<i.second<<"}"; \
    } \
    RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<#prefix " " #name "yaml: "<<g_val_cnf->toString(); \
  }
  XX(g_int_vec_config, int_vec,before);
  XX(g_int_list_config, int_list,before);
  XX(g_int_set_config, int_set,before);
  XX(g_int_unordered_set_config, int_unordered_set,before);
  XX_M(g_str_int_map_config, str_int_map,before);
  XX_M(g_str_int_unordered_map_config, str_int_unordered_map,before);

  YAML::Node root = YAML::LoadFile("../bin/conf/log.yml");
  rapier::Config::LoadFromYaml(root);
  
  RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<"after: "<<g_int_value_config->getValue();
  RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<"after: "<<g_float_value_config->toString();


  XX(g_int_vec_config, int_vec,after);
  XX(g_int_list_config, int_list,after);
  XX(g_int_set_config, int_set,after);
  XX(g_int_unordered_set_config, int_unordered_set,after);
  XX_M(g_str_int_map_config, str_int_map,after);
  XX_M(g_str_int_unordered_map_config, str_int_unordered_map,after);

}

class Person{
public:
  std::string _name;
  int _age=0;
  bool _sex=0;
  std::string toString() const {
    std::stringstream ss;
    ss<<"{Person name="<<_name
      <<" age="<<_age
      <<" sex="<<_sex
      <<"}";
    return ss.str();
  }

  bool operator==(const Person& oth) const{
    return _name == oth._name
      && _age == oth._age
      && _sex == oth._sex;
  }
};
namespace rapier {
  template<>
  class LexicalCast<std::string,Person> {
  public:
    Person operator()(const std::string& str){
      YAML::Node node = YAML::Load(str);
      Person p;
      p._name = node["name"].as<std::string>();
      p._age = node["age"].as<int>();
      p._sex = node["sex"].as<bool>();
      return p;
    }
  };

  template<>
  class LexicalCast<Person, std::string>{
  public:
    std::string operator()(const Person& p){
      YAML::Node node;
      node["name"]=p._name;
      node["age"]=p._age;
      node["sex"]=p._sex;
      std::stringstream ss;
      ss<<node;
      return ss.str();
    }
  };
}



rapier::ConfigVar<Person>::ptr g_person = rapier::Config::Lookup("class.person", Person(), "system person");
rapier::ConfigVar<std::map<std::string, Person>>::ptr g_person_map = rapier::Config::Lookup("class.map", std::map<std::string, Person>(), "system person");
rapier::ConfigVar<std::map<std::string, std::vector<Person>>>::ptr g_person_vec_map = rapier::Config::Lookup("class.vec_map", std::map<std::string, std::vector<Person>>(), "system person");

void test_class(){
  RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<"before: "<<g_person->getValue().toString()<<" - \n"<<g_person->toString();

#define XX_PM(g_var, prefix) \
  { \
    auto m = g_person_map->getValue(); \
    for(auto& i : m){ \
      RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<< #prefix <<" : "<<i.first<<" - "<<i.second.toString(); \
    } \
      RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<< #prefix <<" : size="<<m.size(); \
  } 
  
  g_person->addListener(10,[](const Person& oldVal,const Person& newVal){
    RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<"old val: "<<oldVal.toString()
    <<"new Val:"<<newVal.toString();  
  });
  

  XX_PM(g_person_map, "class.map before");

  RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<"before: "<<g_person_vec_map->toString();
  

  YAML::Node root = YAML::LoadFile("../bin/conf/log.yml");
  rapier::Config::LoadFromYaml(root);
  
  RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<"after: "<<g_person->getValue().toString()<<" - \n"<<g_person->toString();
  XX_PM(g_person_map, "class.map after");
  RAPIER_LOG_INFO(RAPIER_LOG_ROOT())<<"after: "<<g_person_vec_map->toString();
 
}

int main (int argc, char *argv[]) {
  // tets_config();
  //test_yaml();
  test_class();
  return 0;
}
