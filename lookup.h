#ifndef __LOOKUP_H__
#define __LOOKUP_H__

// Put the function prototype for your lookup() procedure here.

// In your definition of LOOKUP, use (not define) the macros ADDR and SIZE
// for the names of the look-up table and size variables used in your C code.
// The actual definitions of ADDR and SIZE go in your C code, not here.

uint8_t lookup(uint8_t row, uint8_t col, uint8_t size,const uint8_t *table);

#endif // __LOOKUP_H__
