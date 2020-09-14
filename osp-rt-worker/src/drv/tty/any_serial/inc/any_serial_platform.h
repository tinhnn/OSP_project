#pragma once
#ifdef ANY_SERIAL_MSVC
#   define APACKED
#   define AINLINE __inline
#elif defined(ACLANG) || defined(AGNUC)
#   define APACKED __attribute__((packed))
#   define AINLINE inline
#endif

#ifndef ANY_SERIAL_SHARED
#   define ANY_SERIAL_API
#elif defined(ANY_SERIAL_MSVC)
#   ifndef ANY_SERIAL_EXPORT
#       define ANY_SERIAL_API __declspec(dllimport)
#   else
#       define ANY_SERIAL_API __declspec(dllexport)
#   endif
#else
#   define ANY_SERIAL_API
#endif
/*EOF*/