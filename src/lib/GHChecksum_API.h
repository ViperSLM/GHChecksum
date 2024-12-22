/* DLL-related stuff. Really only applies to DLL builds */
#ifndef GH_CHECKSUMAPI_H
#define GH_CHECKSUMAPI_H

#include <memory>
#define LIB_PIMPL std::unique_ptr
#define LIB_PIMPL_CREATE std::make_unique

#if defined(GH_STATIC_LIB)
#define APP_API
#define APP_API_EXTERN
#else

#if defined(_WIN32)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if defined(GH_EXPORT_DLL) && !defined(GHCHECKSUMS_APP)
#define APP_API __declspec(dllexport)
#define APP_API_EXTERN
#else
#define APP_API __declspec(dllimport)
#define APP_API_EXTERN extern
#endif
#endif
#endif

#endif