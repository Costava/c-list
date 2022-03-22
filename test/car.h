#ifndef CAR_H
#define CAR_H

#include "List.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct car {
	int year;
	int mass_kg;
} car;

// Would work the same to pass the type as 'struct car'
LIST_GENERATE_HEADER_CODE(car, car);

#ifdef __cplusplus
}
#endif

#endif // CAR_H
