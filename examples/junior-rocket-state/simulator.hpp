// Copyright: 2023, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "junior-rocket-state.hpp"

namespace far::junior {

void load_and_drive(const char* first_stage_filename, const char* second_stage_filename, JuniorRocketState&);

}
