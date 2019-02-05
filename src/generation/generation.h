#pragma once

#include "../model/model.h"
#include "generationParams.h"

#include <random>

Model generate(GenerationParams params, std::mt19937& mt);
