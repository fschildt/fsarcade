#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <iostream>

#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))

#define InvalidDefaultCase assert(0)

#define KIBIBYTES(x) ((x)*1024)
#define MEBIBYTES(x) ((x)*KIBIBYTES(1024))
#define GIBIBYTES(x) ((x)*MEBIBYTES(1024))
