#include <stdint.h>

typedef int_least8_t least8;
typedef int_fast8_t fast8;

#if INT8_MAX
typedef int8_t i8;
#endif

typedef int_least16_t least16;
typedef int_fast16_t fast16;

#if INT16_MAX
typedef int16_t i16;
#endif

typedef int_least32_t least32;
typedef int_fast32_t fast32;

#if INT32_MAX
typedef int32_t i32;
#endif

typedef int_least64_t least64;
typedef int_fast64_t fast64;

#if INT64_MAX
typedef int64_t i64;
#endif
