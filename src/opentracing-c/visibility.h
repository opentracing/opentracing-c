
#ifndef OPENTRACINGC_EXPORT_H
#define OPENTRACINGC_EXPORT_H

#ifdef OPENTRACINGC_STATIC_DEFINE
#  define OPENTRACINGC_EXPORT
#  define OPENTRACINGC_NO_EXPORT
#else
#  ifndef OPENTRACINGC_EXPORT
#    ifdef opentracingc_EXPORTS
        /* We are building this library */
#      define OPENTRACINGC_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define OPENTRACINGC_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef OPENTRACINGC_NO_EXPORT
#    define OPENTRACINGC_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef OPENTRACINGC_DEPRECATED
#  define OPENTRACINGC_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef OPENTRACINGC_DEPRECATED_EXPORT
#  define OPENTRACINGC_DEPRECATED_EXPORT OPENTRACINGC_EXPORT OPENTRACINGC_DEPRECATED
#endif

#ifndef OPENTRACINGC_DEPRECATED_NO_EXPORT
#  define OPENTRACINGC_DEPRECATED_NO_EXPORT OPENTRACINGC_NO_EXPORT OPENTRACINGC_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef OPENTRACINGC_NO_DEPRECATED
#    define OPENTRACINGC_NO_DEPRECATED
#  endif
#endif

#endif /* OPENTRACINGC_EXPORT_H */
