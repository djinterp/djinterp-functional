/******************************************************************************
* djinterp [functional]                                            predicate.h
*
* Predicate combinator types and operations for the functional module.
*   Provides structs and functions for combining predicates with logical AND,
* OR, and NOT operations. Each combinator stores d_predicate function pointers
* with nullable context fields.
*
* 
* path:      \inc\functional\predicate.h
* link(s):   TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2025.02.09
******************************************************************************/

#ifndef DJINTERP_C_FUNCTIONAL_PREDICATE_
#define DJINTERP_C_FUNCTIONAL_PREDICATE_ 1

#include <stdlib.h>
#include "..\djinterp.h"
#include ".\functional_common.h"


// d_predicate_and
//   struct: combines two predicates with logical AND.
// context1 and context2 may be NULL if the underlying predicates do not
// require context.
struct d_predicate_and
{
    d_predicate predicate1;
    d_predicate predicate2;
    void*       context1;
    void*       context2;
};

// d_predicate_or
//   struct: combines two predicates with logical OR.
struct d_predicate_or
{
    d_predicate predicate1;
    d_predicate predicate2;
    void*       context1;
    void*       context2;
};

// d_predicate_xor
//   struct: combines two predicates with logical XOR.
struct d_predicate_xor
{
    d_predicate predicate1;
    d_predicate predicate2;
    void*       context1;
    void*       context2;
};

// d_predicate_not
//   struct: negates a predicate.
struct d_predicate_not
{
    d_predicate predicate;
    void*       context;
};

// D_PRED_AND
//   macro: creates an AND combinator from two `d_predicate` with contexts.
#define D_PREDICATE_AND(predicate1, context1, predicate2, context2)   \
    ( (struct d_predicate_and){ (predicate1),                         \
                                (predicate2),                         \
                                (context1),                           \
                                (context2) } )

// D_PREDICATE_OR
//   macro: creates an OR combinator from two `d_predicate` with contexts.
#define D_PREDICATE_OR(predicate1, context1, predicate2, context2)    \
    ( (struct d_predicate_or){ (predicate1),                          \
                               (predicate2),                          \
                               (context1),                            \
                               (context2) } )

// D_PREDICATE_XOR
//   macro: creates an XOR combinator from two `d_predicate` with contexts.
#define D_PREDICATE_XOR(predicate1, context1, predicate2, context2)   \
    ( (struct d_predicate_xor){ (predicate1),                         \
                                (predicate2),                         \
                                (context1),                           \
                                (context2) } )

// D_PREDICATE_NOT
//   macro: creates a NOT combinator from a `d_predicate` with context.
#define D_PREDICATE_NOT(predicate, context)                           \
    ( (struct d_predicate_not){ (predicate),                          \
                                (context) } )

// D_PREDICATE_AND_SIMPLE
//   macro: creates an AND combinator from two plain `d_predicate` (no 
// context).
#define D_PREDICATE_AND_SIMPLE(predicate1, predicate2)                \
    ( (struct d_predicate_and){ (predicate1),                         \
                                (predicate2),                         \
                                NULL,                                 \
                                NULL } )

// D_PREDICATE_OR_SIMPLE
//   macro: creates an OR combinator from two plain `d_predicate` (no 
// context).
#define D_PREDICATE_OR_SIMPLE(predicate1, predicate2)                 \
    ( (struct d_predicate_or){ (predicate1),                          \
                               (predicate2),                          \
                               NULL,                                  \
                               NULL } )

// D_PREDICATE_XOR_SIMPLE
//   macro: creates an XOR combinator from two plain `d_predicate` (no 
// context).
#define D_PREDICATE_XOR_SIMPLE(predicate1, predicate2)                \
    ( (struct d_predicate_xor){ (predicate1),                         \
                                (predicate2),                         \
                                NULL,                                 \
                                NULL } )

// D_PREDICATE_NOT_SIMPLE
//   macro: creates a NOT combinator from a plain `d_predicate` (no context).
#define D_PREDICATE_NOT_SIMPLE(predicate)                             \
    ( (struct d_predicate_not){ (predicate),                          \
                                NULL } )



struct d_predicate_and* d_predicate_and_new(d_predicate _predicate1, void* _context1, d_predicate _predicate2, void* _context2);
struct d_predicate_or*  d_predicate_or_new(d_predicate _predicate1, void* _context1, d_predicate _predicate2, void* _context2);
struct d_predicate_xor* d_predicate_xor_new(d_predicate _predicate1, void* _context1, d_predicate _predicate2, void* _context2);
struct d_predicate_not* d_predicate_not_new(d_predicate _predicate, void* _context);

bool d_predicate_and_eval(const struct d_predicate_and* _combo, const void* _element);
bool d_predicate_or_eval(const struct d_predicate_or* _combo, const void* _element);
bool d_predicate_xor_eval(const struct d_predicate_xor* _combo, const void* _element);
bool d_predicate_not_eval(const struct d_predicate_not* _combo, const void* _element);


#endif  // DJINTERP_C_FUNCTIONAL_PREDICATE_