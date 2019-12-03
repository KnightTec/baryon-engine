#pragma once
#include <comdef.h>

// used for non-fatal erros
#define HR(hr) do { if(FAILED(hr)) return false; } while (false)

// used for fatal errors
#define HRF(hr) do { \
		if (FAILED(hr)) { \
			MessageBox(nullptr, _com_error(hr).ErrorMessage(), TEXT("DirectX Error"), MB_OK | MB_ICONERROR); \
			exit(EXIT_FAILURE); \
		} \
	} while (false)
