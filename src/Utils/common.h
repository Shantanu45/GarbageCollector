#pragma once

#ifdef GCAPI_EXPORTS
#define VIEW_INTERFACE __declspec(dllexport)
#else
#define VIEW_INTERFACE __declspec(dllimport)
#endif

#include <stdint.h>

/**
 * 32 bit machine word.
 */
using Word = uint32_t;