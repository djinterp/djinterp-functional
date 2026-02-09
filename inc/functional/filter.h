/******************************************************************************
* djinterp [core]                                                    filter.h
*
* Powerful collection filtering system with expression-based selection.
*   This header provides a comprehensive filtering framework for collections
* that supports complex boolean expressions, sequential operations, and
* chainable/combinable filters. Users can describe filtering expressions like
* "first n", "last n", "first n after skipping m", or match against multiple
* conditions with arbitrary boolean logic.
*   Designed to be independent and compartmentalized to facilitate future
* CLI and interpreter integration.
*
* DESIGN PHILOSOPHY:
*   - Operations are composable (can be chained sequentially)
*   - Operations are combinable (can be aggregated with AND/OR/NOT)
*   - Each operation is independent and self-contained
*   - Expression-based API allows declarative filtering
*   - Zero-cost abstractions where possible
*
* TYPICAL USAGE PATTERNS:
*   - Take first N elements
*   - Skip M elements, then take N
*   - Filter by predicate(s) with complex boolean logic
*   - Combine multiple filters (union, intersection, difference)
*   - Chain operations (skip → filter → take)
*   - Range-based selection (elements [start, end))
*
* FUTURE EXTENSIBILITY:
*   - CLI: "filter --take 10 --skip 5 --where 'size > 100'"
*   - Interpreter: parse filter expressions into operation chains
*   - Serialization: convert filter chains to/from strings
*
* path:      \inc\filter.h
* link:      TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.02.06
******************************************************************************/

/*
TABLE OF CONTENTS
=================
I.    PLATFORM DETECTION AND CONFIGURATION
      -------------------------------------
      1.  Feature detection
      2.  Inline support
      3.  Compatibility macros

II.   CORE FILTER TYPES
      ------------------
      1.  Filter operation enumeration
      2.  Filter expression structure
      3.  Filter chain structure
      4.  Filter result structure

III.  FILTER OPERATIONS
      ------------------
      1.  Take operations (first, last, nth)
      2.  Skip operations
      3.  Range operations
      4.  Predicate-based filtering
      5.  Index-based selection

IV.   PREDICATE EXPRESSIONS
      ----------------------
      1.  Simple predicates
      2.  Compound predicates (AND, OR, NOT)
      3.  Chained predicates
      4.  Contextual predicates

V.    FILTER CHAIN CONSTRUCTION
      --------------------------
      1.  Chain creation and initialization
      2.  Adding operations to chain
      3.  Combining chains
      4.  Chain execution
      5.  Chain cleanup

VI.   FILTER COMBINATORS
      -------------------
      1.  Union (OR) of filters
      2.  Intersection (AND) of filters
      3.  Difference (NOT) of filters
      4.  Symmetric difference (XOR)

VII.  EXECUTION AND APPLICATION
      --------------------------
      1.  Apply filter to array
      2.  Apply filter chain
      3.  Count matching elements
      4.  Get indices of matches
      5.  In-place filtering

VIII. UTILITY FUNCTIONS
      ------------------
      1.  Filter validation
      2.  Filter description/serialization
      3.  Clone and copy filters
      4.  Filter optimization

IX.   ITERATOR-BASED INTERFACE
      -------------------------
      1.  Filter iterator creation
      2.  Iterator advancement
      3.  Iterator state management
*/

#ifndef DJINTERP_FILTER_
#define DJINTERP_FILTER_ 1


///////////////////////////////////////////////////////////////////////////////
///             I.    PLATFORM DETECTION AND CONFIGURATION                  ///
///////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "..\djinterp.h"
#include ".\functional.h"


// D_FILTER_INLINE
//   macro: expands to inline keyword if available.
#ifndef D_FILTER_INLINE
    #define D_FILTER_INLINE D_FUNCTIONAL_INLINE
#endif

// D_FILTER_MAX_CHAIN_LENGTH
//   constant: maximum number of operations in a filter chain.
#ifndef D_FILTER_MAX_CHAIN_LENGTH
    #define D_FILTER_MAX_CHAIN_LENGTH 32
#endif

// D_FILTER_MAX_PREDICATE_DEPTH
//   constant: maximum nesting depth for predicate expressions.
#ifndef D_FILTER_MAX_PREDICATE_DEPTH
    #define D_FILTER_MAX_PREDICATE_DEPTH 16
#endif


///////////////////////////////////////////////////////////////////////////////
///             II.   CORE FILTER TYPES                                     ///
///////////////////////////////////////////////////////////////////////////////

// d_filter_op_type
//   enum: types of filter operations.
enum d_filter_op_type
{
    D_FILTER_OP_NONE        = 0x00,  // no operation
    D_FILTER_OP_TAKE_FIRST  = 0x01,  // take first n elements
    D_FILTER_OP_TAKE_LAST   = 0x02,  // take last n elements
    D_FILTER_OP_SKIP_FIRST  = 0x03,  // skip first n elements
    D_FILTER_OP_SKIP_LAST   = 0x04,  // skip last n elements
    D_FILTER_OP_TAKE_NTH    = 0x05,  // take every nth element
    D_FILTER_OP_RANGE       = 0x06,  // take elements in range [start, end)
    D_FILTER_OP_WHERE       = 0x07,  // filter by predicate
    D_FILTER_OP_WHERE_NOT   = 0x08,  // filter by negated predicate
    D_FILTER_OP_INDICES     = 0x09,  // take elements at specific indices
    D_FILTER_OP_DISTINCT    = 0x0A,  // remove duplicate elements
    D_FILTER_OP_REVERSE     = 0x0B,  // reverse element order
    D_FILTER_OP_SLICE       = 0x0C,  // take slice [start:end:step]
    D_FILTER_OP_HEAD        = 0x0D,  // alias for take_first(1)
    D_FILTER_OP_TAIL        = 0x0E,  // alias for take_last(1)
    D_FILTER_OP_INIT        = 0x0F,  // all except last
    D_FILTER_OP_REST        = 0x10   // all except first
};

// d_filter_result_type
//   enum: result type of filter operation.
enum d_filter_result_type
{
    D_FILTER_RESULT_SUCCESS    = 0,   // operation succeeded
    D_FILTER_RESULT_EMPTY      = 1,   // no elements matched
    D_FILTER_RESULT_ERROR      = -1,  // operation failed
    D_FILTER_RESULT_INVALID    = -2,  // invalid parameters
    D_FILTER_RESULT_NO_MEMORY  = -3   // allocation failed
};

// struct d_filter_op_params
//   struct: parameters for a filter operation.
struct d_filter_op_params
{
    size_t      count;           // count parameter (for take/skip)
    size_t      start;           // start index (for range/slice)
    size_t      end;             // end index (for range/slice)
    size_t      step;            // step size (for slice/nth)
    size_t*     indices;         // array of indices (for index-based ops)
    size_t      indices_count;   // number of indices
    d_predicate test;            // predicate function
    void*       context;         // context for predicate
};

// struct d_filter_operation
//   struct: single filter operation in a chain.
struct d_filter_operation
{
    enum d_filter_op_type     type;    // operation type
    struct d_filter_op_params params;  // operation parameters
    char*                     name;    // optional name/description
};

// struct d_filter_chain
//   struct: chain of sequential filter operations.
struct d_filter_chain
{
    struct d_filter_operation* operations;      // array of operations
    size_t                     count;           // number of operations
    size_t                     capacity;        // allocated capacity
    bool                       owns_operations; // whether chain owns ops
};

// struct d_filter_result
//   struct: result of applying a filter.
struct d_filter_result
{
    void*                     elements;      // resulting elements
    size_t                    count;         // number of results
    size_t*                   indices;       // indices of matched elements
    enum d_filter_result_type status;        // operation status
    char*                     error_message; // error description if failed
};

// struct d_predicate_expr
//   struct: compound predicate expression tree.
enum d_predicate_expr_type
{
    D_PRED_EXPR_LEAF,    // simple predicate
    D_PRED_EXPR_AND,     // logical AND
    D_PRED_EXPR_OR,      // logical OR
    D_PRED_EXPR_NOT,     // logical NOT
    D_PRED_EXPR_XOR      // logical XOR
};

struct d_predicate_expr
{
    enum d_predicate_expr_type type;
    union
    {
        struct
        {
            d_predicate test;
            void*       context;
        } leaf;
        struct
        {
            struct d_predicate_expr* left;
            struct d_predicate_expr* right;
        } binary;
        struct
        {
            struct d_predicate_expr* operand;
        } unary;
    } data;
};


///////////////////////////////////////////////////////////////////////////////
///             III.  FILTER OPERATIONS                                     ///
///////////////////////////////////////////////////////////////////////////////

// I.    take operations
struct d_filter_operation d_filter_take_first(size_t _n);
struct d_filter_operation d_filter_take_last(size_t _n);
struct d_filter_operation d_filter_take_nth(size_t _n);
struct d_filter_operation d_filter_head(void);
struct d_filter_operation d_filter_tail(void);

// II.   skip operations
struct d_filter_operation d_filter_skip_first(size_t _n);
struct d_filter_operation d_filter_skip_last(size_t _n);
struct d_filter_operation d_filter_init(void);
struct d_filter_operation d_filter_rest(void);

// III.  range operations
struct d_filter_operation d_filter_range(size_t _start, size_t _end);
struct d_filter_operation d_filter_slice(size_t _start, size_t _end, size_t _step);

// IV.   predicate-based operations
struct d_filter_operation d_filter_where(d_predicate _test);
struct d_filter_operation d_filter_where_ctx(d_predicate_ctx _test, void* _context);
struct d_filter_operation d_filter_where_not(d_predicate _test);
struct d_filter_operation d_filter_where_expr(struct d_predicate_expr* _expr);

// V.    index-based operations
struct d_filter_operation d_filter_at(size_t _index);
struct d_filter_operation d_filter_at_indices(const size_t* _indices, size_t _count);

// VI.   transformation operations
struct d_filter_operation d_filter_distinct(d_comparator _cmp);
struct d_filter_operation d_filter_reverse(void);

// VII.  operation cleanup
void d_filter_operation_free(struct d_filter_operation* _op);


///////////////////////////////////////////////////////////////////////////////
///             IV.   PREDICATE EXPRESSIONS                                 ///
///////////////////////////////////////////////////////////////////////////////

// I.    predicate expression creation
struct d_predicate_expr* d_predicate_expr_leaf(d_predicate _test, void* _context);
struct d_predicate_expr* d_predicate_expr_and(struct d_predicate_expr* _left, struct d_predicate_expr* _right);
struct d_predicate_expr* d_predicate_expr_or(struct d_predicate_expr* _left, struct d_predicate_expr* _right);
struct d_predicate_expr* d_predicate_expr_not(struct d_predicate_expr* _operand);
struct d_predicate_expr* d_predicate_expr_xor(struct d_predicate_expr* _left, struct d_predicate_expr* _right);

// II.   predicate expression evaluation
bool d_predicate_expr_eval(const struct d_predicate_expr* _expr, const void* _element);

// III.  predicate expression management
struct d_predicate_expr* d_predicate_expr_clone(const struct d_predicate_expr* _expr);
void                     d_predicate_expr_free(struct d_predicate_expr* _expr);

// helper macros for predicate expressions
#define D_PRED_AND(LEFT, RIGHT) d_predicate_expr_and((LEFT), (RIGHT))
#define D_PRED_OR(LEFT, RIGHT)  d_predicate_expr_or((LEFT), (RIGHT))
#define D_PRED_NOT(EXPR)        d_predicate_expr_not((EXPR))
#define D_PRED_XOR(LEFT, RIGHT) d_predicate_expr_xor((LEFT), (RIGHT))


///////////////////////////////////////////////////////////////////////////////
///             V.    FILTER CHAIN CONSTRUCTION                             ///
///////////////////////////////////////////////////////////////////////////////

// I.    chain creation
struct d_filter_chain* d_filter_chain_new(void);
struct d_filter_chain* d_filter_chain_new_with_capacity(size_t _capacity);
struct d_filter_chain* d_filter_chain_clone(const struct d_filter_chain* _chain);

// II.   adding operations
bool d_filter_chain_add(struct d_filter_chain* _chain, const struct d_filter_operation* _op);
bool d_filter_chain_add_take_first(struct d_filter_chain* _chain, size_t _n);
bool d_filter_chain_add_take_last(struct d_filter_chain* _chain, size_t _n);
bool d_filter_chain_add_skip_first(struct d_filter_chain* _chain, size_t _n);
bool d_filter_chain_add_skip_last(struct d_filter_chain* _chain, size_t _n);
bool d_filter_chain_add_range(struct d_filter_chain* _chain, size_t _start, size_t _end);
bool d_filter_chain_add_where(struct d_filter_chain* _chain, d_predicate _test);
bool d_filter_chain_add_where_ctx(struct d_filter_chain* _chain, d_predicate_ctx _test, void* _context);

// III.  combining chains
struct d_filter_chain* d_filter_chain_concat(const struct d_filter_chain* _first, const struct d_filter_chain* _second);
bool                   d_filter_chain_append(struct d_filter_chain* _dest, const struct d_filter_chain* _source);

// IV.   chain manipulation
bool d_filter_chain_insert(struct d_filter_chain* _chain, size_t _index, const struct d_filter_operation* _op);
bool d_filter_chain_remove(struct d_filter_chain* _chain, size_t _index);
void d_filter_chain_clear(struct d_filter_chain* _chain);

// V.    chain properties
size_t d_filter_chain_length(const struct d_filter_chain* _chain);
bool   d_filter_chain_is_empty(const struct d_filter_chain* _chain);

// VI.   chain cleanup
void d_filter_chain_free(struct d_filter_chain* _chain);


///////////////////////////////////////////////////////////////////////////////
///             VI.   FILTER COMBINATORS                                    ///
///////////////////////////////////////////////////////////////////////////////

// struct d_filter_union
//   struct: union of multiple filters (OR semantics).
struct d_filter_union
{
    struct d_filter_chain** filters;
    size_t                  count;
};

// struct d_filter_intersection
//   struct: intersection of multiple filters (AND semantics).
struct d_filter_intersection
{
    struct d_filter_chain** filters;
    size_t                  count;
};

// struct d_filter_difference
//   struct: difference of two filters (A - B).
struct d_filter_difference
{
    struct d_filter_chain* include;
    struct d_filter_chain* exclude;
};

// I.    combinator creation
struct d_filter_union*        d_filter_union_new(size_t _capacity);
struct d_filter_intersection* d_filter_intersection_new(size_t _capacity);
struct d_filter_difference*   d_filter_difference_new(struct d_filter_chain* _include, struct d_filter_chain* _exclude);

// II.   adding filters to combinators
bool d_filter_union_add(struct d_filter_union* _union, const struct d_filter_chain* _filter);
bool d_filter_intersection_add(struct d_filter_intersection* _intersection, const struct d_filter_chain* _filter);

// III.  combinator cleanup
void d_filter_union_free(struct d_filter_union* _union);
void d_filter_intersection_free(struct d_filter_intersection* _intersection);
void d_filter_difference_free(struct d_filter_difference* _difference);


///////////////////////////////////////////////////////////////////////////////
///             VII.  EXECUTION AND APPLICATION                             ///
///////////////////////////////////////////////////////////////////////////////

// I.    apply single operation
struct d_filter_result d_filter_apply_operation(const struct d_filter_operation* _op, const void* _input, size_t _count, size_t _element_size);

// II.   apply filter chain
struct d_filter_result d_filter_apply_chain(const struct d_filter_chain* _chain, const void* _input, size_t _count, size_t _element_size);

// III.  apply combinators
struct d_filter_result d_filter_apply_union(const struct d_filter_union* _union, const void* _input, size_t _count, size_t _element_size);
struct d_filter_result d_filter_apply_intersection(const struct d_filter_intersection* _intersection, const void* _input, size_t _count, size_t _element_size);
struct d_filter_result d_filter_apply_difference(const struct d_filter_difference* _difference, const void* _input, size_t _count, size_t _element_size);

// IV.   counting and querying
size_t d_filter_count_matches(const struct d_filter_chain* _chain, const void* _input, size_t _count, size_t _element_size);
bool   d_filter_any_match(const struct d_filter_chain* _chain, const void* _input, size_t _count, size_t _element_size);
bool   d_filter_all_match(const struct d_filter_chain* _chain, const void* _input, size_t _count, size_t _element_size);
bool   d_filter_none_match(const struct d_filter_chain* _chain, const void* _input, size_t _count, size_t _element_size);

// V.    get indices only (no element copying)
size_t* d_filter_get_indices(const struct d_filter_chain* _chain, const void* _input, size_t _count, size_t _element_size, size_t* _out_count);

// VI.   in-place filtering (modifies original array)
size_t d_filter_apply_in_place(const struct d_filter_chain* _chain, void* _array, size_t _count, size_t _element_size);

// VII.  result management
void d_filter_result_free(struct d_filter_result* _result);


///////////////////////////////////////////////////////////////////////////////
///             VIII. UTILITY FUNCTIONS                                     ///
///////////////////////////////////////////////////////////////////////////////

// I.    validation
bool d_filter_operation_is_valid(const struct d_filter_operation* _op);
bool d_filter_chain_is_valid(const struct d_filter_chain* _chain);

// II.   description and serialization
char* d_filter_operation_to_string(const struct d_filter_operation* _op);
char* d_filter_chain_to_string(const struct d_filter_chain* _chain);

// III.  parsing (for future CLI/interpreter)
struct d_filter_operation* d_filter_operation_from_string(const char* _str);
struct d_filter_chain*     d_filter_chain_from_string(const char* _str);

// IV.   optimization
struct d_filter_chain* d_filter_chain_optimize(const struct d_filter_chain* _chain);

// V.    statistics
size_t d_filter_estimate_result_size(const struct d_filter_chain* _chain, size_t _input_count);


///////////////////////////////////////////////////////////////////////////////
///             IX.   ITERATOR-BASED INTERFACE                              ///
///////////////////////////////////////////////////////////////////////////////

// struct d_filter_iterator
//   struct: iterator for lazily evaluating filters.
struct d_filter_iterator
{
    const void*                  input;          // input array
    size_t                       input_count;    // input element count
    size_t                       element_size;   // size of each element
    const struct d_filter_chain* chain;          // filter chain
    size_t                       current_index;  // current position
    size_t*                      indices;        // precomputed indices
    size_t                       indices_count;  // number of indices
    size_t                       indices_pos;    // current index position
    bool                         exhausted;      // no more elements
};

// I.    iterator creation
struct d_filter_iterator* d_filter_iterator_new(const struct d_filter_chain* _chain, const void* _input, size_t _count, size_t _element_size);

// II.   iterator operations
bool  d_filter_iterator_has_next(const struct d_filter_iterator* _iter);
void* d_filter_iterator_next(struct d_filter_iterator* _iter);
void  d_filter_iterator_reset(struct d_filter_iterator* _iter);

// III.  iterator cleanup
void d_filter_iterator_free(struct d_filter_iterator* _iter);


///////////////////////////////////////////////////////////////////////////////
///             X.    CONVENIENCE MACROS                                    ///
///////////////////////////////////////////////////////////////////////////////

// D_FILTER
//   macro: creates and applies a simple filter in one expression.
#define D_FILTER(INPUT, COUNT, SIZE, OP) \
    d_filter_apply_operation(&(OP), (INPUT), (COUNT), (SIZE))

// D_FILTER_CHAIN
//   macro: creates a filter chain from multiple operations.
#define D_FILTER_CHAIN_BEGIN() d_filter_chain_new()
#define D_FILTER_CHAIN_ADD(CHAIN, OP) d_filter_chain_add((CHAIN), &(OP))
#define D_FILTER_CHAIN_END(CHAIN) (CHAIN)

// D_FILTER_FIRST_N
//   macro: shorthand for taking first n elements.
#define D_FILTER_FIRST_N(INPUT, COUNT, SIZE, N) \
    d_filter_apply_operation(                   \
        &d_filter_take_first(N),                \
        (INPUT),                                \
        (COUNT),                                \
        (SIZE))

// D_FILTER_LAST_N
//   macro: shorthand for taking last n elements.
#define D_FILTER_LAST_N(INPUT, COUNT, SIZE, N) \
    d_filter_apply_operation(                  \
        &d_filter_take_last(N),                \
        (INPUT),                               \
        (COUNT),                               \
        (SIZE))

// D_FILTER_WHERE
//   macro: shorthand for predicate-based filtering.
#define D_FILTER_WHERE(INPUT, COUNT, SIZE, PRED) \
    d_filter_apply_operation(                    \
        &d_filter_where(PRED),                   \
        (INPUT),                                 \
        (COUNT),                                 \
        (SIZE))


///////////////////////////////////////////////////////////////////////////////
///             XI.   FLUENT FILTER CHAIN BUILDER                           ///
///////////////////////////////////////////////////////////////////////////////

// struct d_filter_builder
//   struct: fluent builder for constructing filter chains.
// Provides method-chaining style API where each operation returns the builder.
struct d_filter_builder
{
    struct d_filter_chain* chain;
    int                    error_code;
    char*                  error_message;
};

// I.    builder creation
struct d_filter_builder* d_filter_builder_new(void);
struct d_filter_builder* d_filter_builder_from_chain(struct d_filter_chain* _chain);

// II.   fluent operations (return builder for chaining)
struct d_filter_builder* d_filter_builder_take_first(struct d_filter_builder* _builder, size_t _n);
struct d_filter_builder* d_filter_builder_take_last(struct d_filter_builder* _builder, size_t _n);
struct d_filter_builder* d_filter_builder_skip_first(struct d_filter_builder* _builder, size_t _n);
struct d_filter_builder* d_filter_builder_skip_last(struct d_filter_builder* _builder, size_t _n);
struct d_filter_builder* d_filter_builder_where(struct d_filter_builder* _builder, d_predicate _test);
struct d_filter_builder* d_filter_builder_where_ctx(struct d_filter_builder* _builder, d_predicate_ctx _test, void* _context);
struct d_filter_builder* d_filter_builder_range(struct d_filter_builder* _builder, size_t _start, size_t _end);
struct d_filter_builder* d_filter_builder_slice(struct d_filter_builder* _builder, size_t _start, size_t _end, size_t _step);
struct d_filter_builder* d_filter_builder_distinct(struct d_filter_builder* _builder, d_comparator _cmp);
struct d_filter_builder* d_filter_builder_reverse(struct d_filter_builder* _builder);

// III.  builder finalization
struct d_filter_chain* d_filter_builder_build(struct d_filter_builder* _builder);
struct d_filter_result d_filter_builder_apply(struct d_filter_builder* _builder, const void* _input, size_t _count, size_t _element_size);

// IV.   builder error handling
bool  d_filter_builder_has_error(const struct d_filter_builder* _builder);
int   d_filter_builder_get_error_code(const struct d_filter_builder* _builder);
char* d_filter_builder_get_error_message(const struct d_filter_builder* _builder);

// V.    builder cleanup
void d_filter_builder_free(struct d_filter_builder* _builder);


///////////////////////////////////////////////////////////////////////////////
///             XII.  PREDICATE EXPRESSION BUILDER                          ///
///////////////////////////////////////////////////////////////////////////////

// struct d_pred_builder
//   struct: fluent builder for constructing predicate expressions.
struct d_pred_builder
{
    struct d_predicate_expr* expr;
    struct d_predicate_expr* stack[D_FILTER_MAX_PREDICATE_DEPTH];
    size_t                   stack_depth;
};

// I.    predicate builder creation
struct d_pred_builder* d_pred_builder_new(void);
struct d_pred_builder* d_pred_builder_from_predicate(d_predicate _test);

// II.   fluent predicate operations
struct d_pred_builder* d_pred_builder_test(struct d_pred_builder* _builder, d_predicate _test);
struct d_pred_builder* d_pred_builder_and(struct d_pred_builder* _builder, d_predicate _test);
struct d_pred_builder* d_pred_builder_or(struct d_pred_builder* _builder, d_predicate _test);
struct d_pred_builder* d_pred_builder_not(struct d_pred_builder* _builder);
struct d_pred_builder* d_pred_builder_xor(struct d_pred_builder* _builder, d_predicate _test);

// III.  grouping operations
struct d_pred_builder* d_pred_builder_begin_group(struct d_pred_builder* _builder);
struct d_pred_builder* d_pred_builder_end_group(struct d_pred_builder* _builder);

// IV.   builder finalization
struct d_predicate_expr* d_pred_builder_build(struct d_pred_builder* _builder);

// V.    builder cleanup
void d_pred_builder_free(struct d_pred_builder* _builder);


///////////////////////////////////////////////////////////////////////////////
///             XIII. EXPRESSION COMPOSITION HELPERS                        ///
///////////////////////////////////////////////////////////////////////////////

// D_EXPR_AND
//   macro: creates AND expression from two predicates.
#define D_EXPR_AND(P1, P2) \
    d_predicate_expr_and(  \
        d_predicate_expr_leaf((P1), NULL), \
        d_predicate_expr_leaf((P2), NULL))

// D_EXPR_OR
//   macro: creates OR expression from two predicates.
#define D_EXPR_OR(P1, P2) \
    d_predicate_expr_or(  \
        d_predicate_expr_leaf((P1), NULL), \
        d_predicate_expr_leaf((P2), NULL))

// D_EXPR_NOT
//   macro: creates NOT expression from predicate.
#define D_EXPR_NOT(P) \
    d_predicate_expr_not(d_predicate_expr_leaf((P), NULL))

// D_EXPR_COMPLEX
//   macro: helper for building complex expressions inline.
// Usage: D_EXPR_COMPLEX((p1 AND p2) OR (p3 AND NOT p4))
#define D_EXPR_COMPLEX(...) __VA_ARGS__


///////////////////////////////////////////////////////////////////////////////
///             XIV.  CHAINED FILTER SYNTAX                                 ///
///////////////////////////////////////////////////////////////////////////////

// D_FILTER_BEGIN
//   macro: starts a fluent filter chain.
#define D_FILTER_BEGIN() d_filter_builder_new()

// D_THEN_TAKE_FIRST
//   macro: chains take_first operation.
#define D_THEN_TAKE_FIRST(BUILDER, N) \
    d_filter_builder_take_first((BUILDER), (N))

// D_THEN_TAKE_LAST
//   macro: chains take_last operation.
#define D_THEN_TAKE_LAST(BUILDER, N) \
    d_filter_builder_take_last((BUILDER), (N))

// D_THEN_SKIP_FIRST
//   macro: chains skip_first operation.
#define D_THEN_SKIP_FIRST(BUILDER, N) \
    d_filter_builder_skip_first((BUILDER), (N))

// D_THEN_SKIP_LAST
//   macro: chains skip_last operation.
#define D_THEN_SKIP_LAST(BUILDER, N) \
    d_filter_builder_skip_last((BUILDER), (N))

// D_THEN_WHERE
//   macro: chains where (filter) operation.
#define D_THEN_WHERE(BUILDER, PRED) \
    d_filter_builder_where((BUILDER), (PRED))

// D_THEN_RANGE
//   macro: chains range operation.
#define D_THEN_RANGE(BUILDER, START, END) \
    d_filter_builder_range((BUILDER), (START), (END))

// D_THEN_DISTINCT
//   macro: chains distinct operation.
#define D_THEN_DISTINCT(BUILDER, CMP) \
    d_filter_builder_distinct((BUILDER), (CMP))

// D_THEN_REVERSE
//   macro: chains reverse operation.
#define D_THEN_REVERSE(BUILDER) \
    d_filter_builder_reverse((BUILDER))

// D_FILTER_END
//   macro: finalizes and executes filter chain.
#define D_FILTER_END(BUILDER, INPUT, COUNT, SIZE) \
    d_filter_builder_apply((BUILDER), (INPUT), (COUNT), (SIZE))


///////////////////////////////////////////////////////////////////////////////
///             XV.   TYPED FILTER WRAPPERS (C11+)                          ///
///////////////////////////////////////////////////////////////////////////////

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)

// D_FILTER_TYPED
//   macro: type-safe filter application using _Generic.
#define D_FILTER_TYPED(INPUT, COUNT, OP)                     \
    _Generic((INPUT),                                        \
        int*:    d_filter_apply_operation(                   \
                     &(OP), (INPUT), (COUNT), sizeof(int)),  \
        double*: d_filter_apply_operation(                   \
                     &(OP), (INPUT), (COUNT), sizeof(double)),\
        char*:   d_filter_apply_operation(                   \
                     &(OP), (INPUT), (COUNT), sizeof(char)), \
        default: d_filter_apply_operation(                   \
                     &(OP), (INPUT), (COUNT), sizeof(*INPUT)))

#endif  // C11+


///////////////////////////////////////////////////////////////////////////////
///             XVI.  EXAMPLE USAGE PATTERNS                                ///
///////////////////////////////////////////////////////////////////////////////

/*
EXAMPLE 1: Fluent Filter Chain
-------------------------------
struct d_filter_builder* builder = D_FILTER_BEGIN();
struct d_filter_result result =
    D_FILTER_END(
        D_THEN_DISTINCT(
            D_THEN_WHERE(
                D_THEN_SKIP_FIRST(
                    D_THEN_TAKE_FIRST(builder, 100),
                    10),
                is_valid),
            compare_elements),
        input_data,
        input_count,
        sizeof(element_type));


EXAMPLE 2: Complex Predicate Expression
----------------------------------------
struct d_pred_builder* pred_builder = d_pred_builder_new();
d_pred_builder_test(pred_builder, is_positive)
    ->d_pred_builder_and(pred_builder, is_even)
    ->d_pred_builder_or(pred_builder, is_multiple_of_five);

struct d_predicate_expr* expr = d_pred_builder_build(pred_builder);
struct d_filter_operation where_op = d_filter_where_expr(expr);


EXAMPLE 3: Inline Composition
------------------------------
struct d_filter_result result = d_filter_apply_chain(
    d_filter_chain_concat(
        d_filter_chain_skip_first(5),
        d_filter_chain_where_and_take(is_valid, 10)),
    input,
    count,
    size);


EXAMPLE 4: Predicate Expression Tree
-------------------------------------
// (is_positive AND is_even) OR (is_negative AND is_odd)
struct d_predicate_expr* expr = D_PRED_OR(
    D_PRED_AND(
        d_predicate_expr_leaf(is_positive, NULL),
        d_predicate_expr_leaf(is_even, NULL)),
    D_PRED_AND(
        d_predicate_expr_leaf(is_negative, NULL),
        d_predicate_expr_leaf(is_odd, NULL)));

struct d_filter_operation op = d_filter_where_expr(expr);


EXAMPLE 5: Method-Style Chaining
---------------------------------
struct d_filter_builder* filter = d_filter_builder_new();
filter = d_filter_builder_skip_first(filter, 5);
filter = d_filter_builder_where(filter, is_valid);
filter = d_filter_builder_take_first(filter, 10);
filter = d_filter_builder_distinct(filter, compare_fn);

struct d_filter_result result = d_filter_builder_apply(
    filter, data, count, elem_size);


EXAMPLE 6: Builder Pattern with Error Handling
-----------------------------------------------
struct d_filter_builder* builder = d_filter_builder_new();
d_filter_builder_where(builder, predicate1);
d_filter_builder_and_where(builder, predicate2);
d_filter_builder_take_first(builder, 50);

if (d_filter_builder_has_error(builder))
{
    fprintf(stderr, "Filter error: %s\n",
            d_filter_builder_get_error_message(builder));
    d_filter_builder_free(builder);
    return;
}

struct d_filter_chain* chain = d_filter_builder_build(builder);
struct d_filter_result result = d_filter_apply_chain(
    chain, data, count, size);


EXAMPLE 7: Combining Functional and Filter APIs
------------------------------------------------
// fn1(f2(f3(elements) && f4(elements)))
struct d_fn_pipeline pipe = D_PIPELINE(elements, count, size);
pipe = d_fn_pipeline_filter(pipe, f3);
pipe = d_fn_pipeline_filter(pipe, f4);
pipe = d_fn_pipeline_map(pipe, f2);
pipe = d_fn_pipeline_map(pipe, fn1);

void* result = d_fn_pipeline_end(pipe, &result_count);
*/


#endif  // DJINTERP_FILTER_
