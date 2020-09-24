#ifndef __UNIT_H
#define __UNIT_H

typedef void (*TESTFUNCTION)(void *, size_t, size_t);

extern TESTFUNCTION testFunction[];

extern char *testNames[];

extern int nTestFunction;

void generateFilter(size_t n);

void setTestFunctions(void *pattern);

#endif
