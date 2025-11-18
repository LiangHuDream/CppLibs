#ifndef __COMMON_H__
#define __COMMON_H__

#if defined (_WIN32) && defined (_MSC_VER)
#   define CALLBACK __stdcall
#   if defined (WIDGETS_EXPORTS) || defined(Widgets_EXPORTS)
#       define QT_API __declspec(dllexport)
#   else
#       define QT_API __declspec(dllimport)
#   endif
#else // LINUX
#   define CALLBACK
#   define QT_API // __attribute__((visibility("default")))
#endif

#include <memory>
#include <functional>

#ifndef interface
#define interface struct
#endif

#endif // __COMMON_H__
