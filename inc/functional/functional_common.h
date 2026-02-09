/******************************************************************************
* djinterp [core]                                          functional_common.h
*
* Function pointer type definitions for the functional programming module.
*   Provides predicates, transformers, consumers, producers, comparators,
* accumulators, reducers, and generic callback types. Each type also has a
* contextual variant that accepts an additional void* context parameter.
*
* NAMING CONVENTIONS:
*   d_predicate         - function returning bool, taking one argument
*   d_transformer       - function transforming input to output
*   d_consumer          - function consuming value, returning void
*   d_producer          - function producing value, taking no input
*   d_comparator        - function comparing two values
*   d_accumulator       - function combining accumulated value with element
*
* path:      \inc\functional\fn_types.h
* link(s):   TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.02.09
******************************************************************************/

#ifndef DJINTERP_FUNCTIONAL_COMMON_
#define DJINTERP_FUNCTIONAL_COMMON_ 1

#include <stddef.h>
#include "..\djinterp.h"


///////////////////////////////////////////////////////////////////////////////
///             I.    PREDICATES AND COMPARATORS                            ///
///////////////////////////////////////////////////////////////////////////////

// d_predicate
//   function pointer: function returning bool for a given element.
// Used for filtering, testing, and boolean operations.
// _context may be NULL.
typedef bool (*d_predicate)(const void* _element,
                            void*       _context);

// d_binary_predicate
//   function pointer: function returning bool for two elements.
// Used for comparisons, equality checks, and binary tests.
// _context may be NULL.
typedef bool (*d_binary_predicate)(const void* _element1, 
                                   const void* _element2, 
                                   void*       _context);

// d_comparator
//   function pointer: three-way comparison returning negative/zero/positive.
// Returns <0 if _element1 < _element2, 0 if equal, >0 if greater.
// _context may be NULL.
typedef int (*d_comparator)(const void* _element1, 
                            const void* _element2, 
                            void* _context);


///////////////////////////////////////////////////////////////////////////////
///             II.   TRANSFORMERS AND MAPPERS                              ///
///////////////////////////////////////////////////////////////////////////////

// d_transformer
//   function pointer: function transforming input to output.
// Output is written to _output parameter. Returns success status.
// _context may be NULL.
typedef bool (*d_transformer)(const void* _input,
                              void* _output,
                              void* _context);

// d_mapper
//   type alias: alias for d_transformer (common terminology).
typedef d_transformer d_mapper;


///////////////////////////////////////////////////////////////////////////////
///             III.  CONSUMERS AND PRODUCERS                               ///
///////////////////////////////////////////////////////////////////////////////


// d_consumer
//   function pointer: function consuming a value without producing output.
// Used for side-effects like printing, logging, or updating state.
// _context may be NULL.
typedef void (*d_consumer)(void* _element,
                           void* _context);

// d_consumer_const
//   function pointer: consumer that doesn't modify the element.
// _context may be NULL.
typedef void (*d_consumer_const)(const void* _element,
                                 void*       _context);

// d_producer
//   function pointer: function producing a value with no input.
// Output is written to _output parameter. Returns success status.
// _context may be NULL.
typedef bool (*d_producer)(void* _output,
                           void* _context);


///////////////////////////////////////////////////////////////////////////////
///             IV.   ACCUMULATORS AND REDUCERS                             ///
///////////////////////////////////////////////////////////////////////////////

// d_accumulator
//   function pointer: function combining accumulated value with new element.
// Used in fold/reduce operations. Output is written to _accumulated.
// Returns success status. _context may be NULL.
typedef bool (*d_accumulator)(void*       _accumulated,
                              const void* _element,
                              void*       _context);

// d_reducer
//   function pointer: function combining two elements into one.
// Used for reduction operations. Output is written to _result.
// _context may be NULL.
typedef bool (*d_reducer)(const void* _element1,
                          const void* _element2,
                          void*       _result,
                          void*       _context);


///////////////////////////////////////////////////////////////////////////////
///             V.    GENERIC CALLBACKS AND OPERATIONS                      ///
///////////////////////////////////////////////////////////////////////////////

// d_unary_operation
//   function pointer: operation taking one input and producing one output.
// _context may be NULL.
typedef bool (*d_unary_operation)(const void* _input,
                                  void*       _output,
                                  void*       _context);

// d_binary_operation
//   function pointer: operation taking two inputs and producing one output.
// _context may be NULL.
typedef bool (*d_binary_operation)(const void* _input1,
                                   const void* _input2,
                                   void*       _output,
                                   void*       _context);

// d_callback
//   function pointer: generic callback with optional context parameter.
// _context may be NULL.
typedef void (*d_callback)(void* _context);

// d_hasher
//   function pointer: function computing hash value for an element.
// _context may be NULL.
typedef size_t (*d_hasher)(const void* _element,
                           void*       _context);

// d_cloner
//   function pointer: function creating a deep copy of an element.
// Output is written to _destination. Returns success status.
// _context may be NULL.
typedef bool (*d_cloner)(const void* _source,
                         void*       _destination,
                         void*       _context);


#endif  // DJINTERP_FUNCTIONAL_COMMON_
