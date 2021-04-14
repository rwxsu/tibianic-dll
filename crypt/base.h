#ifndef __BASEH__
#define __BASEH__

#include <iostream>

namespace Crypt {
  namespace Base {
    std::string encode(std::string v);
    std::string decode(std::string v);
  }
}

#endif
