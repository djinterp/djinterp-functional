/******************************************************************************
* djinterp [functional]                                    functional_common.h
*
* Common types, macros, and utilities for the functional programming module.
*   Provides function pointer type definitions (predicates, transformers,
* consumers, producers, comparators, accumulators, reducers, generic
* callbacks), inline/composition macro helpers, type-generic utilities
* (C11+), user-defined typed function wrappers, and commonly used utility
* functions (identity, constant, comparison, null-checking).
*
* NAMING CONVENTIONS:
*   d_predicate         - function returning bool, taking one argument
*   d_transformer       - function transforming input to output
*   d_consumer          - function consuming value, returning void
*   d_producer          - function producing value, taking no input
*   d_comparator        - function comparing two values
*   d_accumulator       - function combining accumulated value with element
*
* path:      \inc\functional\functional_common.h
* link(s):   TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.02.09
******************************************************************************/

#ifndef DJINTERP_FUNCTIONAL_COMMON_
#define DJINTERP_FUNCTIONAL_COMMON_ 1

#include <stddef.h>
#include <string.h>
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
                            void*       _context);


///////////////////////////////////////////////////////////////////////////////
///             II.   TRANSFORMERS AND MAPPERS                              ///
///////////////////////////////////////////////////////////////////////////////

// d_transformer
//   function pointer: function transforming input to output.
// Output is written to _output parameter. Returns success status.
// _context may be NULL.
typedef bool (*d_transformer)(const void* _input,
                              void*       _output,
                              void*       _context);

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


///////////////////////////////////////////////////////////////////////////////
///             VI.   INLINE AND COMPOSITION MACROS                         ///
///////////////////////////////////////////////////////////////////////////////

// D_FUNCTIONAL_LAMBDA
//   macro: creates a simple inline function-like expression.
// Note: this is a limited "lambda" that works in C89+.
#define D_FUNCTIONAL_LAMBDA(RETURN_TYPE, PARAM_TYPE,          \
                            PARAM_NAME, BODY)                 \
    D_INLINE RETURN_TYPE                           \
    D_CONCAT(d_lambda_, __LINE__)(PARAM_TYPE PARAM_NAME)     \
    BODY

// D_FUNCTIONAL_PREDICATE
//   macro: creates a simple predicate function.
// The generated function accepts and ignores a void* _context parameter.
#define D_FUNCTIONAL_PREDICATE(NAME, PARAM, CONDITION)        \
    D_INLINE bool                                  \
    NAME(const void* PARAM, void* _context)                   \
    {                                                         \
        (void)_context;                                       \
        return (CONDITION);                                   \
    }

// D_FUNCTIONAL_TRANSFORMER
//   macro: creates a simple transformer function.
// The generated function accepts and ignores a void* _context parameter.
#define D_FUNCTIONAL_TRANSFORMER(NAME, INPUT, OUTPUT, BODY)   \
    D_INLINE bool                                  \
    NAME(const void* INPUT, void* OUTPUT, void* _context)     \
    {                                                         \
        (void)_context;                                       \
        BODY                                                  \
        return true;                                          \
    }

// D_FUNCTIONAL_CONSUMER
//   macro: creates a simple consumer function.
// The generated function accepts and ignores a void* _context parameter.
#define D_FUNCTIONAL_CONSUMER(NAME, PARAM, BODY)              \
    D_INLINE void                                  \
    NAME(void* PARAM, void* _context)                         \
    {                                                         \
        (void)_context;                                       \
        BODY                                                  \
    }

// D_FUNCTIONAL_COMPOSE
//   macro: composes two function calls.
#define D_FUNCTIONAL_COMPOSE(F, G, X) F(G(X))


///////////////////////////////////////////////////////////////////////////////
///             VII.  TYPE-GENERIC HELPERS (C11+)                           ///
///////////////////////////////////////////////////////////////////////////////

#if D_ENV_LANG_IS_C11_OR_HIGHER

// D_FUNCTIONAL_CALL
//   macro: type-generic function call wrapper.
// Automatically handles casting for common types. Passes NULL context.
#define D_FUNCTIONAL_CALL(FN, ARG)                            \
    _Generic((ARG),                                           \
        int*:          FN((const void*)(ARG), NULL),          \
        double*:       FN((const void*)(ARG), NULL),          \
        char*:         FN((const void*)(ARG), NULL),          \
        void*:         FN((const void*)(ARG), NULL),          \
        const int*:    FN((const void*)(ARG), NULL),          \
        const double*: FN((const void*)(ARG), NULL),          \
        const char*:   FN((const void*)(ARG), NULL),          \
        const void*:   FN((ARG), NULL),                       \
        default:       FN((const void*)(ARG), NULL))

#endif  // D_ENV_LANG_IS_C11_OR_HIGHER


///////////////////////////////////////////////////////////////////////////////
///             VIII. USER-DEFINED FUNCTION WRAPPERS                        ///
///////////////////////////////////////////////////////////////////////////////

// D_FUNCTIONAL_DEFINE_PREDICATE
//   macro: defines a typed predicate wrapper for user types.
// Creates both a typed version and a void* wrapper for composition.
// The wrapper forwards _context to the typed version.
#define D_FUNCTIONAL_DEFINE_PREDICATE(NAME, TYPE)             \
    bool NAME##_typed(const TYPE* _element,                   \
                      void*       _context);                  \
    D_INLINE bool                                  \
    NAME(const void* _element, void* _context)                \
    {                                                         \
        return NAME##_typed((const TYPE*)_element,            \
                            _context);                        \
    }                                                         \
    bool NAME##_typed(const TYPE* _element,                   \
                      void*       _context)

// D_FUNCTIONAL_DEFINE_TRANSFORMER
//   macro: defines a typed transformer wrapper for user types.
#define D_FUNCTIONAL_DEFINE_TRANSFORMER(NAME,                 \
                                        INPUT_TYPE,           \
                                        OUTPUT_TYPE)          \
    bool NAME##_typed(const INPUT_TYPE* _input,               \
                      OUTPUT_TYPE*       _output,             \
                      void*              _context);           \
    D_INLINE bool                                  \
    NAME(const void* _input, void* _output, void* _context)   \
    {                                                         \
        return NAME##_typed((const INPUT_TYPE*)_input,        \
                            (OUTPUT_TYPE*)_output,            \
                            _context);                        \
    }                                                         \
    bool NAME##_typed(const INPUT_TYPE* _input,               \
                      OUTPUT_TYPE*       _output,             \
                      void*              _context)

// D_FUNCTIONAL_DEFINE_CONSUMER
//   macro: defines a typed consumer wrapper for user types.
#define D_FUNCTIONAL_DEFINE_CONSUMER(NAME, TYPE)              \
    void NAME##_typed(TYPE* _element,                         \
                      void* _context);                        \
    D_INLINE void                                  \
    NAME(void* _element, void* _context)                      \
    {                                                         \
        NAME##_typed((TYPE*)_element, _context);              \
    }                                                         \
    void NAME##_typed(TYPE* _element,                         \
                      void* _context)

// D_FUNCTIONAL_DEFINE_COMPARATOR
//   macro: defines a typed comparator wrapper for user types.
#define D_FUNCTIONAL_DEFINE_COMPARATOR(NAME, TYPE)            \
    int NAME##_typed(const TYPE* _a,                          \
                     const TYPE* _b,                          \
                     void*       _context);                   \
    D_INLINE int                                   \
    NAME(const void* _a, const void* _b, void* _context)      \
    {                                                         \
        return NAME##_typed((const TYPE*)_a,                  \
                            (const TYPE*)_b,                  \
                            _context);                        \
    }                                                         \
    int NAME##_typed(const TYPE* _a,                          \
                     const TYPE* _b,                          \
                     void*       _context)

// D_FUNCTIONAL_DEFINE_ACCUMULATOR
//   macro: defines a typed accumulator wrapper for user types.
#define D_FUNCTIONAL_DEFINE_ACCUMULATOR(NAME, ACC_TYPE,       \
                                        ELEM_TYPE)            \
    bool NAME##_typed(ACC_TYPE*        _accumulated,          \
                      const ELEM_TYPE* _element,              \
                      void*            _context);             \
    D_INLINE bool                                  \
    NAME(void* _accumulated, const void* _element,            \
         void* _context)                                      \
    {                                                         \
        return NAME##_typed((ACC_TYPE*)_accumulated,          \
                            (const ELEM_TYPE*)_element,       \
                            _context);                        \
    }                                                         \
    bool NAME##_typed(ACC_TYPE*        _accumulated,          \
                      const ELEM_TYPE* _element,              \
                      void*            _context)


///////////////////////////////////////////////////////////////////////////////
///             IX.   UTILITY FUNCTIONS                                     ///
///////////////////////////////////////////////////////////////////////////////

// i.    identity functions
bool     d_functional_identity_transformer(const void* _input, void* _output, void* _context);
bool     d_functional_identity_predicate(const void* _element, void* _context);

// ii.   constant functions
bool     d_functional_constant_true(const void* _element, void* _context);
bool     d_functional_constant_false(const void* _element, void* _context);

// iii.  comparison utilities
int      d_functional_compare_int(const void* _a, const void* _b, void* _context);
int      d_functional_compare_size_t(const void* _a, const void* _b, void* _context);
int      d_functional_compare_double(const void* _a, const void* _b, void* _context);
bool     d_functional_equal_int(const void* _a, const void* _b, void* _context);
bool     d_functional_equal_size_t(const void* _a, const void* _b, void* _context);

// iv.   predicate utilities
bool     d_functional_is_null(const void* _element, void* _context);
bool     d_functional_is_not_null(const void* _element, void* _context);

///////////////////////////////////////////////////////////////////////////////
///             X.    HIGHER-ORDER FUNCTIONS                                ///
///////////////////////////////////////////////////////////////////////////////

// i.    map
bool     d_functional_map(const void* _input, void* _output, size_t _count, size_t _element_size, d_transformer _transform, void* _context);
         
// ii.     fold
bool     d_functional_fold_left(const void* _input, size_t _count, size_t _element_size, void* _accumulator, d_accumulator _combine, void* _context);
bool     d_functional_fold_right(const void* _input, size_t _count, size_t _element_size, void* _accumulator, d_accumulator _combine, void* _context);
         
// iii.    iteration
void     d_functional_for_each(void* _input, size_t _count, size_t _element_size, d_consumer _apply, void* _context);
void     d_functional_for_each_const(const void* _input, size_t _count, size_t _element_size, d_consumer_const _apply, void* _context);
         
// iv.     quantifiers
bool     d_functional_any(const void* _input, size_t _count, size_t _element_size, d_predicate _test, void* _context);
bool     d_functional_all(const void* _input, size_t _count, size_t _element_size, d_predicate _test, void* _context);
bool     d_functional_none(const void* _input, size_t _count, size_t _element_size, d_predicate _test, void* _context);
size_t   d_functional_count_if(const void* _input, size_t _count, size_t _element_size, d_predicate _test, void* _context);
void*    d_functional_find_if(const void* _input, size_t _count, size_t _element_size, d_predicate _test, void* _context);


#endif  // DJINTERP_FUNCTIONAL_COMMON_
