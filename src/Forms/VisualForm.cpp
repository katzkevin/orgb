//
//  VisualForm.cpp
//  orgb
//
//  Created by Kevin Katz on 10/28/21.
//

#include "VisualForm.hpp"

VisualForm::VisualForm(std::string _name) {
    name = _name;
    parameters.setName(name);
}
