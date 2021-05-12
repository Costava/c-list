#include <assert.h>
#include <stdio.h>

#include "car.h"

#define VEC_IMPLEMENTATION
#include "vec.h"

VEC_GENERATE_FOR_TYPE(int, int)
VEC_GENERATE_FOR_TYPE(unsigned char, uchar)

int main(void) {
    vecint widths;
    vecint_init(&widths, 32, VEC_GROW_MODE_ADD, 16);
    assert(widths.length == 0); assert(widths.capacity == 32);
    vecint_push_back(&widths, 5);
    assert(widths.length == 1); assert(widths.capacity == 32);
    assert(vecint_pop_back(&widths) == 5);
    assert(widths.length == 0); assert(widths.capacity == 32);
    vecint_shrink_to_fit(&widths);
    assert(widths.length == 0); assert(widths.capacity == 0);
    vecint_push_back(&widths, 77);
    assert(widths.length == 1); assert(widths.capacity == 16);
    vecint_push_back(&widths, 88);
    vecint_push_back(&widths, 99);
    assert(widths.length == 3);
    assert(vecint_pop_back(&widths) == 99);
    assert(vecint_pop_back(&widths) == 88);
    assert(vecint_pop_back(&widths) == 77);

    vecuchar uchars;
    vecuchar_init(&uchars, 1, VEC_GROW_MODE_MULTIPLY, 2);
    assert(uchars.length == 0); assert(uchars.capacity == 1);
    vecuchar_push_back(&uchars, 'a');
    assert(uchars.length == 1); assert(uchars.capacity == 1);
    vecuchar_push_back(&uchars, 'b');
    assert(uchars.length == 2); assert(uchars.capacity == 2);
    vecuchar_push_back(&uchars, 'c');
    vecuchar_push_back(&uchars, 'd');
    assert(uchars.length == 4); assert(uchars.capacity == 4);
    vecuchar_push_back(&uchars, 'e');
    assert(uchars.length == 5); assert(uchars.capacity == 8);
    assert(vecuchar_pop_back(&uchars) == 'e');
    assert(uchars.length == 4); assert(uchars.capacity == 8);
    vecuchar_insert_at_swap(&uchars, '1', 1);
    assert(uchars.length == 5); assert(uchars.capacity == 8);
    assert(uchars.buf[1] == '1');
    assert(uchars.buf[4] == 'b');
    vecuchar_insert_at_shift(&uchars, '2', 0);
    assert(uchars.length == 6); assert(uchars.capacity == 8);
    assert(uchars.buf[0] == '2');
    assert(uchars.buf[1] == 'a');
    assert(uchars.buf[2] == '1');
    assert(uchars.buf[3] == 'c');
    assert(uchars.buf[4] == 'd');
    assert(uchars.buf[5] == 'b');
    assert(vecuchar_remove_at_swap(&uchars, 2) == '1');
    assert(uchars.length == 5); assert(uchars.capacity == 8);
    assert(uchars.buf[0] == '2');
    assert(uchars.buf[1] == 'a');
    assert(uchars.buf[2] == 'b');
    assert(uchars.buf[3] == 'c');
    assert(uchars.buf[4] == 'd');
    vecuchar_remove_at_shift(&uchars, 0);
    assert(uchars.length == 4); assert(uchars.capacity == 8);
    assert(uchars.buf[0] == 'a');
    assert(uchars.buf[1] == 'b');
    assert(uchars.buf[2] == 'c');
    assert(uchars.buf[3] == 'd');

    veccar cars;
    veccar_init(&cars, 3, VEC_GROW_MODE_ADD, 2);
    assert(cars.length == 0); assert(cars.capacity == 3);
    veccar_push_back(&cars, (car){2000, 1000});
    veccar_push_back(&cars, (car){2001, 1001});
    veccar_push_back(&cars, (car){2002, 1002});
    veccar_push_back(&cars, (car){2003, 1003});
    assert(cars.length == 4); assert(cars.capacity == 5);

    vecint_deinit(&widths);
    vecuchar_deinit(&uchars);
    veccar_deinit(&cars);

    return 0;
}
