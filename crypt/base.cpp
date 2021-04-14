#include "base.h"

std::string Crypt::Base::encode(std::string v){
  for(size_t i = 0; i < v.length(); i++){
    v[i] = ' ' + '~' - v[i];
  }
  
  return v;
}

std::string Crypt::Base::decode(std::string v){
  return Crypt::Base::encode(v);
}
