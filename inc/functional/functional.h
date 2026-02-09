/******************************************************************************
* djinterp [core]                                                 functional.h
*
* Cross-platform functional programming foundation for C.
*   This header provides function pointer types, composition utilities, and
* core functional programming patterns adapted for C. Includes predicates,
* transformers, consumers, producers, and higher-order operations like map,
* filter, fold, and compose.
*   Designed to be portable across C89, C99, and later standards with
* appropriate feature detection.
*
* NAMING CONVENTIONS:
*   d_predicate         - function returning bool, taking one argument
*   d_transformer       - function transforming input to output
*   d_consumer          - function consuming value, returning void
*   d_producer          - function producing value, taking no input
*   d_comparator        - function comparing two values
*   d_accumulator       - function combining accumulated value with element
*
* USAGE PATTERNS:
*   - Function pointers for callbacks and operations
*   - Higher-order functions (map, filter, fold)
*   - Function composition via macros and helpers
*   - Predicate combinators (and, or, not)
*
* path:      \inc\functional.h
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.02.06
******************************************************************************/

/*
TABLE OF CONTENTS
=================
I.    PLATFORM DETECTION AND CONFIGURATION
      -------------------------------------
      1.  Feature detection
      2.  Inline function support
      3.  Restrict keyword support
      4.  Compatibility macros

II.   FUNCTION POINTER TYPES
      -----------------------
      1.  Predicates and comparators
      2.  Transformers and mappers
      3.  Consumers and producers
      4.  Accumulators and reducers
      5.  Generic callbacks
      6.  Contextual variants (with void* context)

III.  PREDICATE COMBINATORS
      ----------------------
      1.  Logical operations (and, or, not)
      2.  Predicate composition
      3.  Contextual predicate combinators

IV.   FUNCTIONAL COMPOSITION
      ------------------------
      1.  Function composition
      2.  Partial application helpers
      3.  Currying utilities

V.    HIGHER-ORDER OPERATIONS
      ------------------------
      1.  Map operations
      2.  Filter operations
      3.  Fold/reduce operations
      4.  ForEach operations
      5.  Any/all/none operations

VI.   UTILITY FUNCTIONS
      ------------------
      1.  Identity functions
      2.  Constant functions
      3.  Comparison utilities
      4.  Boolean conversions

VII.  MACRO HELPERS
      --------------
      1.  Inline function wrappers
      2.  Lambda-like macro utilities
      3.  Function composition macros
*/

#ifndef DJINTERP_FUNCTIONAL_
#define DJINTERP_FUNCTIONAL_ 1


#include <stddef.h>
#include <string.h>
#include "..\djinterp.h"


// D_FUNCTIONAL_INLINE
//   macro: expands to inline keyword if available, otherwise empty.
#ifndef D_FUNCTIONAL_INLINE
    #if defined(__cplusplus)
        #define D_FUNCTIONAL_INLINE inline
    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
        #define D_FUNCTIONAL_INLINE inline
    #elif defined(__GNUC__) || defined(__clang__)
        #define D_FUNCTIONAL_INLINE __inline__
    #elif defined(_MSC_VER)
        #define D_FUNCTIONAL_INLINE __inline
    #else
        #define D_FUNCTIONAL_INLINE
    #endif
#endif

// D_FUNCTIONAL_RESTRICT
//   macro: expands to restrict keyword if available.
#ifndef D_FUNCTIONAL_RESTRICT
    #if defined(__cplusplus)
        #define D_FUNCTIONAL_RESTRICT
    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
        #define D_FUNCTIONAL_RESTRICT restrict
    #elif defined(__GNUC__) || defined(__clang__)
        #define D_FUNCTIONAL_RESTRICT __restrict
    #elif defined(_MSC_VER) && (_MSC_VER >= 1400)
        #define D_FUNCTIONAL_RESTRICT __restrict
    #else
        #define D_FUNCTIONAL_RESTRICT
    #endif
#endif

// D_FUNCTIONAL_CONST
//   macro: marker for const correctness in function pointers.
#define D_FUNCTIONAL_CONST const

// D_FUNCTIONAL_NOEXCEPT
//   macro: expands to noexcept if C++ or appropriate attribute.
#ifndef D_FUNCTIONAL_NOEXCEPT
    #if defined(__cplusplus) && (__cplusplus >= 201103L)
        #define D_FUNCTIONAL_NOEXCEPT noexcept
    #else
        #define D_FUNCTIONAL_NOEXCEPT
    #endif
#endif


///////////////////////////////////////////////////////////////////////////////
///             II.   FUNCTION POINTER TYPES                                ///
///////////////////////////////////////////////////////////////////////////////

// d_predicate
//   typedef: function returning bool for a given element.
// Used for filtering, testing, and boolean operations.
typedef bool (*d_predicate)(const void* _element);

// d_predicate_ctx
//   typedef: predicate with additional context parameter.
typedef bool (*d_predicate_ctx)(const void* _element, void* _context);

// d_binary_predicate
//   typedef: function returning bool for two elements.
// Used for comparisons, equality checks, and binary tests.
typedef bool (*d_binary_predicate)(const void* _element1,
                                   const void* _element2);

// d_binary_predicate_ctx
//   typedef: binary predicate with additional context parameter.
typedef bool (*d_binary_predicate_ctx)(const void* _element1,
                                       const void* _element2,
                                       void*       _context);

// d_transformer
//   typedef: function transforming input to output.
// Output is written to _output parameter. Returns success status.
typedef bool (*d_transformer)(const void* _input,
                              void*       _output);

// d_transformer_ctx
//   typedef: transformer with additional context parameter.
typedef bool (*d_transformer_ctx)(const void* _input,
                                  void*       _output,
                                  void*       _context);

// d_consumer
//   typedef: function consuming a value without producing output.
// Used for side-effects like printing, logging, or updating state.
typedef void (*d_consumer)(void* _element);

// d_consumer_const
//   typedef: consumer that doesn't modify the element.
typedef void (*d_consumer_const)(const void* _element);

// d_consumer_ctx
//   typedef: consumer with additional context parameter.
typedef void (*d_consumer_ctx)(void* _element, void* _context);

// d_producer
//   typedef: function producing a value with no input.
// Output is written to _output parameter. Returns success status.
typedef bool (*d_producer)(void* _output);

// d_producer_ctx
//   typedef: producer with additional context parameter.
typedef bool (*d_producer_ctx)(void* _output, void* _context);

// d_comparator
//   typedef: three-way comparison returning negative/zero/positive.
// Returns <0 if _element1 < _element2, 0 if equal, >0 if greater.
typedef int (*d_comparator)(const void* _element1,
                            const void* _element2);

// d_comparator_ctx
//   typedef: comparator with additional context parameter.
typedef int (*d_comparator_ctx)(const void* _element1,
                                const void* _element2,
                                void*       _context);

// d_accumulator
//   typedef: function combining accumulated value with new element.
// Used in fold/reduce operations. Output is written to _accumulated.
// Returns success status.
typedef bool (*d_accumulator)(void*       _accumulated,
                              const void* _element);

// d_accumulator_ctx
//   typedef: accumulator with additional context parameter.
typedef bool (*d_accumulator_ctx)(void*       _accumulated,
                                  const void* _element,
                                  void*       _context);

// d_reducer
//   typedef: function combining two elements into one.
// Used for reduction operations. Output is written to _result.
typedef bool (*d_reducer)(const void* _element1,
                          const void* _element2,
                          void*       _result);

// d_reducer_ctx
//   typedef: reducer with additional context parameter.
typedef bool (*d_reducer_ctx)(const void* _element1,
                              const void* _element2,
                              void*       _result,
                              void*       _context);

// d_mapper
//   typedef: alias for d_transformer (common terminology).
typedef d_transformer d_mapper;

// d_mapper_ctx
//   typedef: mapper with context.
typedef d_transformer_ctx d_mapper_ctx;

// d_unary_op
//   typedef: operation taking one input and producing one output.
typedef bool (*d_unary_op)(const void* _input, void* _output);

// d_binary_op
//   typedef: operation taking two inputs and producing one output.
typedef bool (*d_binary_op)(const void* _input1,
                            const void* _input2,
                            void*       _output);

// d_callback
//   typedef: generic callback with no parameters.
typedef void (*d_callback)(void);

// d_callback_ctx
//   typedef: generic callback with context parameter.
typedef void (*d_callback_ctx)(void* _context);

// d_hasher
//   typedef: function computing hash value for an element.
typedef size_t (*d_hasher)(const void* _element);

// d_hasher_ctx
//   typedef: hasher with additional context parameter.
typedef size_t (*d_hasher_ctx)(const void* _element, void* _context);

// d_cloner
//   typedef: function creating a deep copy of an element.
// Output is written to _output. Returns success status.
typedef bool (*d_cloner)(const void* _source, void* _destination);

// d_cloner_ctx
//   typedef: cloner with additional context parameter.
typedef bool (*d_cloner_ctx)(const void* _source,
                             void*       _destination,
                             void*       _context);


///////////////////////////////////////////////////////////////////////////////
///             III.  PREDICATE COMBINATORS                                 ///
///////////////////////////////////////////////////////////////////////////////

// struct d_predicate_and
//   struct: combines two predicates with logical AND.
struct d_predicate_and
{
    d_predicate pred1;
    d_predicate pred2;
};

// struct d_predicate_or
//   struct: combines two predicates with logical OR.
struct d_predicate_or
{
    d_predicate pred1;
    d_predicate pred2;
};

// struct d_predicate_not
//   struct: negates a predicate.
struct d_predicate_not
{
    d_predicate pred;
};

// struct d_predicate_and_ctx
//   struct: combines two context predicates with logical AND.
struct d_predicate_and_ctx
{
    d_predicate_ctx pred1;
    d_predicate_ctx pred2;
    void*           context1;
    void*           context2;
};

// struct d_predicate_or_ctx
//   struct: combines two context predicates with logical OR.
struct d_predicate_or_ctx
{
    d_predicate_ctx pred1;
    d_predicate_ctx pred2;
    void*           context1;
    void*           context2;
};

// struct d_predicate_not_ctx
//   struct: negates a context predicate.
struct d_predicate_not_ctx
{
    d_predicate_ctx pred;
    void*           context;
};


// I.    predicate combinator evaluation
bool d_predicate_and_eval(const struct d_predicate_and* _combo, const void* _element);
bool d_predicate_or_eval(const struct d_predicate_or* _combo, const void* _element);
bool d_predicate_not_eval(const struct d_predicate_not* _combo, const void* _element);
bool d_predicate_and_ctx_eval(const struct d_predicate_and_ctx* _combo, const void* _element);
bool d_predicate_or_ctx_eval(const struct d_predicate_or_ctx* _combo, const void* _element);
bool d_predicate_not_ctx_eval(const struct d_predicate_not_ctx* _combo, const void* _element);

// II.   predicate combinator creation
struct d_predicate_and d_predicate_and_create(d_predicate _pred1, d_predicate _pred2);
struct d_predicate_or  d_predicate_or_create(d_predicate _pred1, d_predicate _pred2);
struct d_predicate_not d_predicate_not_create(d_predicate _pred);


///////////////////////////////////////////////////////////////////////////////
///             IV.   FUNCTIONAL COMPOSITION                                ///
///////////////////////////////////////////////////////////////////////////////

// struct d_composed_transformer
//   struct: composition of two transformers (f ∘ g).
struct d_composed_transformer
{
    d_transformer first;     // applied first (g)
    d_transformer second;    // applied second (f)
    size_t        temp_size; // size of intermediate result
    void*         temp_buf;  // temporary buffer for intermediate result
};

// I.    transformer composition
struct d_composed_transformer d_compose_transformers(d_transformer _first, d_transformer _second, size_t _temp_size);
bool                         d_composed_transformer_apply(const struct d_composed_transformer* _composed, const void* _input, void* _output);
void                         d_composed_transformer_free(struct d_composed_transformer* _composed);

// struct d_partial_consumer
//   struct: partially applied consumer with bound context.
struct d_partial_consumer
{
    d_consumer_ctx fn;
    void*          context;
};

// II.   partial application
struct d_partial_consumer d_partial_consumer_create(d_consumer_ctx _fn, void* _context);
void                      d_partial_consumer_apply(const struct d_partial_consumer* _partial, void* _element);


///////////////////////////////////////////////////////////////////////////////
///             V.    HIGHER-ORDER OPERATIONS                               ///
///////////////////////////////////////////////////////////////////////////////

// I.    map operations
bool   d_map(const void* _input, void* _output, size_t _count, size_t _element_size, d_transformer _transform);
bool   d_map_ctx(const void* _input, void* _output, size_t _count, size_t _element_size, d_transformer_ctx _transform, void* _context);

// II.   filter operations
size_t d_filter(const void* _input, void* _output, size_t _count, size_t _element_size, d_predicate _test);
size_t d_filter_ctx(const void* _input, void* _output, size_t _count, size_t _element_size, d_predicate_ctx _test, void* _context);

// III.  fold/reduce operations
bool d_fold_left(const void* _input, size_t _count, size_t _element_size, void* _accumulator, d_accumulator _combine);
bool d_fold_left_ctx(const void* _input, size_t _count, size_t _element_size, void* _accumulator, d_accumulator_ctx _combine, void* _context);
bool d_fold_right(const void* _input, size_t _count, size_t _element_size, void* _accumulator, d_accumulator _combine);
bool d_fold_right_ctx(const void* _input, size_t _count, size_t _element_size, void* _accumulator, d_accumulator_ctx _combine, void* _context);

// IV.   for-each operations
void d_for_each(void* _input, size_t _count, size_t _element_size, d_consumer _apply);
void d_for_each_const(const void* _input, size_t _count, size_t _element_size, d_consumer_const _apply);
void d_for_each_ctx(void* _input, size_t _count, size_t _element_size, d_consumer_ctx _apply, void* _context);

// V.    any/all/none operations
bool d_any(const void* _input, size_t _count, size_t _element_size, d_predicate _test);
bool d_any_ctx(const void* _input, size_t _count, size_t _element_size, d_predicate_ctx _test, void* _context);
bool d_all(const void* _input, size_t _count, size_t _element_size, d_predicate _test);
bool d_all_ctx(const void* _input, size_t _count, size_t _element_size, d_predicate_ctx _test, void* _context);
bool d_none(const void* _input, size_t _count, size_t _element_size, d_predicate _test);
bool d_none_ctx(const void* _input, size_t _count, size_t _element_size, d_predicate_ctx _test, void* _context);

// VI.   counting and searching
size_t d_count_if(const void* _input, size_t _count, size_t _element_size, d_predicate _test);
size_t d_count_if_ctx(const void* _input, size_t _count, size_t _element_size, d_predicate_ctx _test, void* _context);
void*  d_find_if(const void* _input, size_t _count, size_t _element_size, d_predicate _test);
void*  d_find_if_ctx(const void* _input, size_t _count, size_t _element_size, d_predicate_ctx _test, void* _context);


///////////////////////////////////////////////////////////////////////////////
///             VI.   UTILITY FUNCTIONS                                     ///
///////////////////////////////////////////////////////////////////////////////

// I.    identity functions
bool d_identity_transformer(const void* _input, void* _output);
bool d_identity_predicate(const void* _element);

// II.   constant functions
bool d_constant_true(const void* _element);
bool d_constant_false(const void* _element);

// III.  comparison utilities
int  d_compare_int(const void* _a, const void* _b);
int  d_compare_size_t(const void* _a, const void* _b);
int  d_compare_double(const void* _a, const void* _b);
bool d_equal_int(const void* _a, const void* _b);
bool d_equal_size_t(const void* _a, const void* _b);

// IV.   predicate utilities
bool d_is_null(const void* _element);
bool d_is_not_null(const void* _element);


///////////////////////////////////////////////////////////////////////////////
///             VII.  MACRO HELPERS                                         ///
///////////////////////////////////////////////////////////////////////////////

// D_LAMBDA
//   macro: creates a simple inline function-like expression.
// Note: This is a limited "lambda" that works in C89+.
#define D_LAMBDA(RETURN_TYPE, PARAM_TYPE, PARAM_NAME, BODY)  \
    D_FUNCTIONAL_INLINE RETURN_TYPE                          \
    D_CONCAT(d_lambda_, __LINE__)(PARAM_TYPE PARAM_NAME)    \
    BODY

// D_PREDICATE
//   macro: creates a simple predicate function.
#define D_PREDICATE(NAME, PARAM, CONDITION)                  \
    D_FUNCTIONAL_INLINE bool                                 \
    NAME(const void* PARAM)                                  \
    {                                                        \
        return (CONDITION);                                  \
    }

// D_TRANSFORMER
//   macro: creates a simple transformer function.
#define D_TRANSFORMER(NAME, INPUT, OUTPUT, BODY)             \
    D_FUNCTIONAL_INLINE bool                                 \
    NAME(const void* INPUT, void* OUTPUT)                    \
    {                                                        \
        BODY                                                 \
        return true;                                         \
    }

// D_CONSUMER
//   macro: creates a simple consumer function.
#define D_CONSUMER(NAME, PARAM, BODY)                        \
    D_FUNCTIONAL_INLINE void                                 \
    NAME(void* PARAM)                                        \
    {                                                        \
        BODY                                                 \
    }

// D_COMPOSE
//   macro: composes two function calls.
#define D_COMPOSE(F, G, X) F(G(X))


///////////////////////////////////////////////////////////////////////////////
///             VIII. FUNCTION COMPOSITION AND CHAINING                     ///
///////////////////////////////////////////////////////////////////////////////

// struct d_fn_pipeline
//   struct: holds intermediate results for function pipeline.
struct d_fn_pipeline
{
    void*  data;           // current data pointer
    size_t element_size;   // size of each element
    size_t count;          // number of elements
    bool   owns_data;      // whether pipeline owns the data
    int    error_code;     // error status (0 = success)
};

// I.    pipeline creation
struct d_fn_pipeline d_fn_pipeline_begin(void* _data, size_t _count, size_t _element_size);
struct d_fn_pipeline d_fn_pipeline_begin_copy(const void* _data, size_t _count, size_t _element_size);

// II.   pipeline operations (chainable)
struct d_fn_pipeline d_fn_pipeline_map(struct d_fn_pipeline _pipe, d_transformer _transform);
struct d_fn_pipeline d_fn_pipeline_filter(struct d_fn_pipeline _pipe, d_predicate _test);
struct d_fn_pipeline d_fn_pipeline_fold(struct d_fn_pipeline _pipe, void* _initial, d_accumulator _combine);
struct d_fn_pipeline d_fn_pipeline_for_each(struct d_fn_pipeline _pipe, d_consumer _apply);
struct d_fn_pipeline d_fn_pipeline_take(struct d_fn_pipeline _pipe, size_t _n);
struct d_fn_pipeline d_fn_pipeline_skip(struct d_fn_pipeline _pipe, size_t _n);

// III.  pipeline finalization
void* d_fn_pipeline_end(struct d_fn_pipeline _pipe, size_t* _out_count);
void  d_fn_pipeline_free(struct d_fn_pipeline _pipe);

// D_PIPELINE
//   macro: starts a function pipeline.
#define D_PIPELINE(DATA, COUNT, SIZE) \
    d_fn_pipeline_begin((DATA), (COUNT), (SIZE))

// D_PIPE
//   macro: applies an operation in a pipeline (alias for readability).
#define D_PIPE(PIPE, OP) OP(PIPE)


///////////////////////////////////////////////////////////////////////////////
///             IX.   PREDICATE COMBINATORS (INLINE)                        ///
///////////////////////////////////////////////////////////////////////////////

// struct d_pred_combo_and
//   struct: stores two predicates for AND combination.
struct d_pred_combo_and
{
    d_predicate pred1;
    d_predicate pred2;
};

// struct d_pred_combo_or
//   struct: stores two predicates for OR combination.
struct d_pred_combo_or
{
    d_predicate pred1;
    d_predicate pred2;
};

// struct d_pred_combo_not
//   struct: stores predicate for NOT.
struct d_pred_combo_not
{
    d_predicate pred;
};

// I.    predicate combinator evaluators
bool d_pred_combo_and_eval(const struct d_pred_combo_and* _combo, const void* _element);
bool d_pred_combo_or_eval(const struct d_pred_combo_or* _combo, const void* _element);
bool d_pred_combo_not_eval(const struct d_pred_combo_not* _combo, const void* _element);

// D_PRED_AND_CREATE
//   macro: creates an AND combinator.
#define D_PRED_AND_CREATE(P1, P2) \
    ((struct d_pred_combo_and){(P1), (P2)})

// D_PRED_OR_CREATE
//   macro: creates an OR combinator.
#define D_PRED_OR_CREATE(P1, P2) \
    ((struct d_pred_combo_or){(P1), (P2)})

// D_PRED_NOT_CREATE
//   macro: creates a NOT combinator.
#define D_PRED_NOT_CREATE(P) \
    ((struct d_pred_combo_not){(P)})


///////////////////////////////////////////////////////////////////////////////
///             X.    CONVENIENCE COMPOSITION MACROS                        ///
///////////////////////////////////////////////////////////////////////////////

// D_THEN
//   macro: chains two operations sequentially (left-to-right composition).
// Usage: result = D_THEN(D_THEN(f1, f2), f3)(input);
#define D_THEN(F1, F2) d_compose_##F1##_##F2

// D_AND_THEN
//   macro: alias for D_THEN for readability.
#define D_AND_THEN(F1, F2) D_THEN(F1, F2)

// D_COMPOSE_FN
//   macro: creates a composition function from two functions.
// This is a helper for creating named composition functions.
#define D_COMPOSE_FN(NAME, F1, F2, TYPE)                     \
    D_FUNCTIONAL_INLINE bool                                 \
    NAME(const void* _input, void* _output)                  \
    {                                                        \
        TYPE temp;                                           \
        if (!(F1)(_input, &temp))                            \
        {                                                    \
            return false;                                    \
        }                                                    \
        return (F2)(&temp, _output);                         \
    }

// D_MAP_THEN_FILTER
//   macro: convenient composition of map followed by filter.
#define D_MAP_THEN_FILTER(DATA, COUNT, SIZE, TRANSFORM, TEST) \
    d_filter(                                                  \
        d_map((DATA), NULL, (COUNT), (SIZE), (TRANSFORM)),     \
        NULL,                                                  \
        (COUNT),                                               \
        (SIZE),                                                \
        (TEST))


///////////////////////////////////////////////////////////////////////////////
///             XI.   TYPE-GENERIC HELPERS (C11+)                           ///
///////////////////////////////////////////////////////////////////////////////

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)

// D_FN_CALL
//   macro: type-generic function call wrapper.
// Automatically handles casting for common types.
#define D_FN_CALL(FN, ARG)                                   \
    _Generic((ARG),                                          \
        int*:          FN((const void*)(ARG)),               \
        double*:       FN((const void*)(ARG)),               \
        char*:         FN((const void*)(ARG)),               \
        void*:         FN((const void*)(ARG)),               \
        const int*:    FN((const void*)(ARG)),               \
        const double*: FN((const void*)(ARG)),               \
        const char*:   FN((const void*)(ARG)),               \
        const void*:   FN((ARG)),                            \
        default:       FN((const void*)(ARG)))

#endif  // C11+


///////////////////////////////////////////////////////////////////////////////
///             XII.  FLUENT BUILDER PATTERN                                ///
///////////////////////////////////////////////////////////////////////////////

// struct d_fn_builder
//   struct: fluent builder for constructing function chains.
struct d_fn_builder
{
    d_transformer* transforms;      // array of transformers
    d_predicate*   predicates;      // array of predicates
    size_t         transform_count;
    size_t         predicate_count;
    size_t         capacity;
};

// I.    builder creation
struct d_fn_builder* d_fn_builder_new(void);

// II.   fluent operations (return builder for chaining)
struct d_fn_builder* d_fn_builder_map(struct d_fn_builder* _builder, d_transformer _transform);
struct d_fn_builder* d_fn_builder_filter(struct d_fn_builder* _builder, d_predicate _test);
struct d_fn_builder* d_fn_builder_and_then(struct d_fn_builder* _builder, d_transformer _transform);
struct d_fn_builder* d_fn_builder_where(struct d_fn_builder* _builder, d_predicate _test);

// III.  builder execution
bool d_fn_builder_execute(const struct d_fn_builder* _builder, const void* _input, size_t _count, size_t _element_size, void* _output, size_t* _out_count);

// IV.   builder cleanup
void d_fn_builder_free(struct d_fn_builder* _builder);


///////////////////////////////////////////////////////////////////////////////
///             XIII. USER-DEFINED FUNCTION WRAPPERS                        ///
///////////////////////////////////////////////////////////////////////////////

// D_DEFINE_PREDICATE
//   macro: defines a typed predicate wrapper for user types.
// Creates both a typed version and a void* wrapper for composition.
#define D_DEFINE_PREDICATE(NAME, TYPE)                       \
    bool NAME##_typed(const TYPE* _element);                 \
    D_FUNCTIONAL_INLINE bool                                 \
    NAME(const void* _element)                               \
    {                                                        \
        return NAME##_typed((const TYPE*)_element);          \
    }                                                        \
    bool NAME##_typed(const TYPE* _element)

// D_DEFINE_TRANSFORMER
//   macro: defines a typed transformer wrapper for user types.
#define D_DEFINE_TRANSFORMER(NAME, INPUT_TYPE, OUTPUT_TYPE)  \
    bool NAME##_typed(const INPUT_TYPE* _input,              \
                      OUTPUT_TYPE*       _output);           \
    D_FUNCTIONAL_INLINE bool                                 \
    NAME(const void* _input, void* _output)                  \
    {                                                        \
        return NAME##_typed((const INPUT_TYPE*)_input,       \
                           (OUTPUT_TYPE*)_output);           \
    }                                                        \
    bool NAME##_typed(const INPUT_TYPE* _input,              \
                      OUTPUT_TYPE*       _output)

// D_DEFINE_CONSUMER
//   macro: defines a typed consumer wrapper for user types.
#define D_DEFINE_CONSUMER(NAME, TYPE)                        \
    void NAME##_typed(TYPE* _element);                       \
    D_FUNCTIONAL_INLINE void                                 \
    NAME(void* _element)                                     \
    {                                                        \
        NAME##_typed((TYPE*)_element);                       \
    }                                                        \
    void NAME##_typed(TYPE* _element)

// D_DEFINE_COMPARATOR
//   macro: defines a typed comparator wrapper for user types.
#define D_DEFINE_COMPARATOR(NAME, TYPE)                      \
    int NAME##_typed(const TYPE* _a, const TYPE* _b);        \
    D_FUNCTIONAL_INLINE int                                  \
    NAME(const void* _a, const void* _b)                     \
    {                                                        \
        return NAME##_typed((const TYPE*)_a,                 \
                           (const TYPE*)_b);                 \
    }                                                        \
    int NAME##_typed(const TYPE* _a, const TYPE* _b)

// D_DEFINE_ACCUMULATOR
//   macro: defines a typed accumulator wrapper for user types.
#define D_DEFINE_ACCUMULATOR(NAME, ACC_TYPE, ELEM_TYPE)      \
    bool NAME##_typed(ACC_TYPE*       _accumulated,          \
                      const ELEM_TYPE* _element);            \
    D_FUNCTIONAL_INLINE bool                                 \
    NAME(void* _accumulated, const void* _element)           \
    {                                                        \
        return NAME##_typed((ACC_TYPE*)_accumulated,         \
                           (const ELEM_TYPE*)_element);      \
    }                                                        \
    bool NAME##_typed(ACC_TYPE*       _accumulated,          \
                      const ELEM_TYPE* _element)


///////////////////////////////////////////////////////////////////////////////
///             XIV.  COMPOSABLE FUNCTION TEMPLATES                         ///
///////////////////////////////////////////////////////////////////////////////

// D_MAKE_COMPOSABLE
//   macro: wraps a user function to make it composable.
// Usage: D_MAKE_COMPOSABLE(my_func, int) creates d_my_func wrapper.
#define D_MAKE_COMPOSABLE(FUNC, TYPE)                        \
    D_FUNCTIONAL_INLINE bool                                 \
    d_##FUNC##_wrapper(const void* _input, void* _output)    \
    {                                                        \
        *(TYPE*)_output = FUNC(*(const TYPE*)_input);        \
        return true;                                         \
    }

// D_MAKE_PREDICATE_FROM
//   macro: converts a boolean function to d_predicate.
#define D_MAKE_PREDICATE_FROM(FUNC, TYPE)                    \
    D_FUNCTIONAL_INLINE bool                                 \
    d_##FUNC##_predicate(const void* _element)               \
    {                                                        \
        return FUNC(*(const TYPE*)_element);                 \
    }

// D_LIFT
//   macro: lifts a typed function into void* space for composition.
// This is the generic version that works with any signature.
#define D_LIFT(FUNC_NAME, WRAPPER_NAME)                      \
    const void* WRAPPER_NAME = (const void*)FUNC_NAME


///////////////////////////////////////////////////////////////////////////////
///             XV.   EXAMPLE USAGE PATTERNS                                ///
///////////////////////////////////////////////////////////////////////////////

/*
EXAMPLE 1: Function Pipeline
-----------------------------
struct d_fn_pipeline result = 
    D_PIPE(
        D_PIPE(
            D_PIPE(
                D_PIPELINE(elements, count, sizeof(int)),
                d_fn_pipeline_filter, is_positive),
            d_fn_pipeline_map, double_value),
        d_fn_pipeline_take, 10);

int* final_data = d_fn_pipeline_end(result, &final_count);


EXAMPLE 2: Predicate Composition
---------------------------------
struct d_pred_combo_and combo = D_PRED_AND_CREATE(is_positive, is_even);
filtered_count = d_filter(input, output, count, size, 
                          (d_predicate)d_pred_combo_and_eval);


EXAMPLE 3: Fluent Builder
--------------------------
struct d_fn_builder* builder = d_fn_builder_new();
d_fn_builder_filter(builder, is_valid)
    ->d_fn_builder_map(builder, transform_to_string)
    ->d_fn_builder_filter(builder, is_not_empty);

d_fn_builder_execute(builder, input, count, size, output, &out_count);
d_fn_builder_free(builder);


EXAMPLE 4: Composed Transformer
--------------------------------
D_COMPOSE_FN(double_then_increment, double_value, increment, int);
d_map(input, output, count, sizeof(int), double_then_increment);


EXAMPLE 5: Inline Predicate Composition
----------------------------------------
bool result = d_pred_combo_and_eval(
    &D_PRED_AND_CREATE(
        is_positive,
        d_pred_combo_or_eval(
            &D_PRED_OR_CREATE(is_even, is_multiple_of_five))),
    &element);
*/


#endif  // DJINTERP_FUNCTIONAL_
