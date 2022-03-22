# C List

- Generic, type-safe [dynamic array](https://en.wikipedia.org/wiki/Dynamic_array) written in C.
- Uses macros to generate a struct (where array state is managed) and functions to act on that struct for any given type.

Dependencies:
- C99 standard library

## Usage

```C
#include <stdint.h>
#include "List.h"

// Generate code for the type `int32_t`
// "i32" is suffixed to "List" in the function names
LIST_GENERATE_FOR_TYPE(int32_t, i32);

int main(void) {
    Listi32 depths;
    Listi32_Init(&depths, 32, LIST_GROW_MODE_MULTIPLY, 2);
    Listi32_PushBack(&depths, 1000);
    Listi32_PushBack(&depths, 3000);
    Listi32_PushBack(&depths, 4000);
    Listi32_InsertAtShift(&depths, 2000, 1);
    const int32_t latest = Listi32_PopBack(&depths); // 4000

    Listi32_Deinit(&depths);
    return 0;
}
```

`LIST_GENERATE_FOR_TYPE` is a convenience macro that calls both
`LIST_GENERATE_HEADER_CODE` and `LIST_GENERATE_IMPLEMENTATION_CODE`.  
Alternatively, these two macros can be called separately
as seen in `test/car.h` and `test/car.c` respectively.  
All three macros take the same two arguments: `type` and `suffix`.

See `LIST_GENERATE_HEADER_CODE` in `List.h`
for a list of functions and their descriptions.

## License

BSD 2-Clause License. See file `LICENSE.txt`.

## Contributing

Not currently accepting contributions.  
Feel free to open an issue or open an issue just to ask a question.
