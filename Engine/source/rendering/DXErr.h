#pragma once
#include <cstdlib>

#define HR(hr) do { if(FAILED(hr)) return false; } while (false)

#define HRF(hr) do { if(FAILED(hr)) exit(EXIT_FAILURE); } while (false)

//TODO: improve this macro to provide error information