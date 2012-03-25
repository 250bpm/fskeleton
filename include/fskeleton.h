/*****************************************************************************/
/*  Handle DSO symbol visibility                                             */
/*****************************************************************************/

#if defined _WIN32
#   if defined DLL_EXPORT
#       define XS_EXPORT __declspec(dllexport)
#   else
#       define XS_EXPORT __declspec(dllimport)
#   endif
#else
#   if defined __SUNPRO_C  || defined __SUNPRO_CC
#       define XS_EXPORT __global
#   elif (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#       define XS_EXPORT __attribute__ ((visibility("default")))
#   else
#       define XS_EXPORT
#   endif
#endif

XS_EXPORT void *xs_extension;