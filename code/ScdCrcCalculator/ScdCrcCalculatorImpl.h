#ifndef _SCDCRCCALCULATORIMPL_H
#define _SCDCRCCALCULATORIMPL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Windows.h>

LPCSTR CalcCrcImpl(LPCSTR lpFilePath, char szErrorMessage[], int nBufferSize);

#ifdef __cplusplus
}
#endif

#endif /* _SCDCRCCALCULATORIMPL_H */
