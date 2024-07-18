#pragma once
#include <string>
#include "Data.h"


bool UUID::operator==(const UUID &other) const {
    return value == other.value;
}
