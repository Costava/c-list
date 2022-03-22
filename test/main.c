#include <assert.h>
#include <stdio.h>

#include "car.h"
#include "List.h"

LIST_GENERATE_FOR_TYPE(int, int);
LIST_GENERATE_FOR_TYPE(unsigned char, uchar);

int main(void) {
    Listint widths;
    Listint_Init(&widths, 32, LIST_GROW_MODE_ADD, 16);
    assert(widths.length == 0); assert(widths.capacity == 32);
    Listint_PushBack(&widths, 5);
    assert(widths.length == 1); assert(widths.capacity == 32);
    assert(Listint_PopBack(&widths) == 5);
    assert(widths.length == 0); assert(widths.capacity == 32);
    Listint_ShrinkToFit(&widths);
    assert(widths.length == 0); assert(widths.capacity == 0);
    Listint_PushBack(&widths, 77);
    assert(widths.length == 1); assert(widths.capacity == 16);
    Listint_PushBack(&widths, 88);
    Listint_PushBack(&widths, 99);
    assert(widths.length == 3);
    assert(Listint_PopBack(&widths) == 99);
    assert(Listint_PopBack(&widths) == 88);
    assert(Listint_PopBack(&widths) == 77);

    Listuchar uchars;
    Listuchar_Init(&uchars, 1, LIST_GROW_MODE_MULTIPLY, 2);
    assert(uchars.length == 0); assert(uchars.capacity == 1);
    Listuchar_PushBack(&uchars, 'a');
    assert(uchars.length == 1); assert(uchars.capacity == 1);
    Listuchar_PushBack(&uchars, 'b');
    assert(uchars.length == 2); assert(uchars.capacity == 2);
    Listuchar_PushBack(&uchars, 'c');
    Listuchar_PushBack(&uchars, 'd');
    assert(uchars.length == 4); assert(uchars.capacity == 4);
    Listuchar_PushBack(&uchars, 'e');
    assert(uchars.length == 5); assert(uchars.capacity == 8);
    assert(Listuchar_PopBack(&uchars) == 'e');
    assert(uchars.length == 4); assert(uchars.capacity == 8);
    Listuchar_InsertAtSwap(&uchars, '1', 1);
    assert(uchars.length == 5); assert(uchars.capacity == 8);
    assert(uchars.buf[1] == '1');
    assert(uchars.buf[4] == 'b');
    Listuchar_InsertAtShift(&uchars, '2', 0);
    assert(uchars.length == 6); assert(uchars.capacity == 8);
    assert(uchars.buf[0] == '2');
    assert(uchars.buf[1] == 'a');
    assert(uchars.buf[2] == '1');
    assert(uchars.buf[3] == 'c');
    assert(uchars.buf[4] == 'd');
    assert(uchars.buf[5] == 'b');
    assert(Listuchar_RemoveAtSwap(&uchars, 2) == '1');
    assert(uchars.length == 5); assert(uchars.capacity == 8);
    assert(uchars.buf[0] == '2');
    assert(uchars.buf[1] == 'a');
    assert(uchars.buf[2] == 'b');
    assert(uchars.buf[3] == 'c');
    assert(uchars.buf[4] == 'd');
    Listuchar_RemoveAtShift(&uchars, 0);
    assert(uchars.length == 4); assert(uchars.capacity == 8);
    assert(uchars.buf[0] == 'a');
    assert(uchars.buf[1] == 'b');
    assert(uchars.buf[2] == 'c');
    assert(uchars.buf[3] == 'd');

    Listcar cars;
    Listcar_Init(&cars, 3, LIST_GROW_MODE_ADD, 2);
    assert(cars.length == 0); assert(cars.capacity == 3);
    Listcar_PushBack(&cars, (car){2000, 1000});
    Listcar_PushBack(&cars, (car){2001, 1001});
    Listcar_PushBack(&cars, (car){2002, 1002});
    Listcar_PushBack(&cars, (car){2003, 1003});
    assert(cars.length == 4); assert(cars.capacity == 5);

    Listint_Deinit(&widths);
    Listuchar_Deinit(&uchars);
    Listcar_Deinit(&cars);

    return 0;
}
