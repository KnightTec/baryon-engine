#pragma once

#define HR(hr) do { if(FAILED(hr)) return false; } while (false)

//TODO: improve this macro to provide error information