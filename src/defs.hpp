#pragma once

#include <stddef.h>
#include <stdint.h>

#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))

#define KIBIBYTES(x) ((x)*1024)
#define MEBIBYTES(x) ((x)*KIBIBYTES(1024))
#define GIBIBYTES(x) ((x)*MEBIBYTES(1024))
