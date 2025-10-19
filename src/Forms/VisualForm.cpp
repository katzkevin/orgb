//
//  VisualForm.cpp
//  orgb
//
//  Created by Kevin Katz on 10/28/21.
//

#include "VisualForm.hpp"

VisualForm::VisualForm(std::string name) {
    this->name = name;
    parameters.setName(name);
}
