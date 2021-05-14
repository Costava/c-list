# vec

- Generic, type-safe [dynamic array](https://en.wikipedia.org/wiki/Dynamic_array) written in C.
- Uses macros to generate a struct (where array state is managed) and functions to act on that struct for any given type.

Dependencies:
- C99 standard library

## Usage

```C
#include <stdint.h>
#include "vec.h"

// Generate code for the type `int32_t`
// "i32" is suffixed to "vec" in the function names
VEC_GENERATE_FOR_TYPE(int32_t, i32)

int main(void) {
    veci32 depths;
    veci32_init(&depths, 32, VEC_GROW_MODE_MULTIPLY, 2);
    veci32_push_back(&depths, 1000);
    veci32_push_back(&depths, 3000);
    veci32_push_back(&depths, 4000);
    veci32_insert_at_shift(&depths, 2000, 1);
    const int32_t latest = veci32_pop_back(&depths); // 4000

    veci32_deinit(&depths);
    return 0;
}
```

Alternatively, use macros `VEC_GENERATE_HEADER_CODE` and `VEC_GENERATE_IMPLEMENTATION_CODE`
in separate header and `.c` files respectively.  
`VEC_GENERATE_FOR_TYPE` calls both of the other two for convenience.  
All three macros take the same two arguments: `type` and `suffix`.  
See `test/main.c` for an example of the convenience macro.  
See `test/car.c` and `test/car.h` for an example of the separate macros.

See `VEC_FOR_ALL_FUNCS` in `vec.h` for a list of all functions.
Descriptions are above the definitions in `vec.h`.

Feel free to open an issue to ask a question.

## License

BSD 2-Clause License. See file `LICENSE.txt`.
