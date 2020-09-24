#ifndef __PATTERNS_H
#define __PATTERNS_H


void map (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  void (*worker)(void *v1, const void *v2) // [ v1 = op (v2) ]
);

void stencil (
  void *dest, 
  void *src, 
  size_t nJob, 
  size_t sizeJob, 
  size_t width,         //Number of adjacent data units to be mapped
  void (*worker)(void *v1, const void *v2, const void *v3)
);

void codeFusionStencil (
  void *dest, 
  void *src, 
  size_t nJob, 
  size_t sizeJob, 
  size_t width,         //Number of adjacent data units to be mapped
  void (*worker)(void *v1, const void *v2, const void *v3)
);

void codeAndCacheFusionStencil (
 void *dest,
 void *src,
 size_t nJob,
 size_t sizeJob,
 size_t width,          //Number of adjacent data units to be mapped
 void (*worker)(void *v1, const void *v2, const void *v3)
);

void noFusionStencil (
  void *dest,
  void *src,
  size_t nJob,
  size_t sizeJob,
  size_t width,         //Number of adjacent data units to be mapped
  void (*worker)(void *v1, const void *v2, const void *v3)
);

void reduce (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  void (*worker)(void *v1, const void *v2, const void *v3) // [ v1 = op (v2, v3) ]
);

void scan (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  void (*worker)(void *v1, const void *v2, const void *v3) // [ v1 = op (v2, v3) ]
);

int pack (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  const int *filter     // Filer for pack
);

void gather (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  const int *filter,    // Filter for gather
  int nFilter           // # elements in the filter
);

void scatter (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  const int *filter     // Filter for scatter
);

/**
 *   Baseline implementation for comparisons
 */
void sequentialScatter (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  const int *filter     // Filter for scatter
);

/*
*   Does nothing.
*   Used to estimate the filter creation time in the calling funtion.
*/
void noOpScatter (
  void *dest,           // Not used
  void *src,            // Not used
  size_t nJob,          // Not used
  size_t sizeJob,       // Not used
  const int *filter     // Not used
);

/**
 *   General case Permutation Scatter; not optimized for small outputs.
 */
void multiScatter (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  const int *filter     // Filter for scatter
);

/**
 *   Permutation Scatter specialized in small outputs that fill completelly
 */
void lowOutputScatter (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  const int *filter     // Filter for scatter
);

/**
 *   Atomic implementation of Scatter using fine-grain locking
 */
void lockScatter (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  const int *filter     // Filter for scatter
);

/**
 *   Scatter implementation allowing race conditions
 */
void raceConditionsScatter (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  const int *filter     // Filter for scatter
);

void pipeline (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  void (*workerList[])(void *v1, const void *v2), // one function for each stage of the pipeline
  size_t nWorkers       // # stages in the pipeline
);

void farm (
  void *dest,           // Target array
  void *src,            // Source array
  size_t nJob,          // # elements in the source array
  size_t sizeJob,       // Size of each element in the source array
  void (*worker)(void *v1, const void *v2),  // [ v1 = op (22) ]
  size_t nWorkers       // # workers in the farm
);

#endif
