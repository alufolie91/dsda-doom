#ifndef __QS22J__
#define __QS22J__

#ifdef __cplusplus
extern "C" {
#endif

void qs22j(void *base, size_t nmemb, size_t size,
                                     int (*compar)(const void *, const void *));
#ifdef __cplusplus
} // extern "C"
#endif

#endif
