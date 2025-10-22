//
//  LaserWaves.hpp
//  orgb
//
//  Created by Kevin Katz on 2/22/22.
//

#ifndef LaserWaves_hpp
#define LaserWaves_hpp

#include "BaseWaves.hpp"

class LaserWaves : public BaseWaves {
   public:
    explicit LaserWaves(std::string name);
    ~LaserWaves() override {};
};

#endif /* LaserWaves_hpp */
