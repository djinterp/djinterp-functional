#include "..\..\inc\functional\functional.h"


///////////////////////////////////////////////////////////////////////////////
///             PREDICATE COMBINATORS                                       ///
///////////////////////////////////////////////////////////////////////////////

/*
d_predicate_and_eval
  Evaluates a logical AND combination of two predicates against an element.

Parameter(s):
  _combo:   the AND predicate combinator containing two predicates.
  _element: the element to test against both predicates.
Return:
  A boolean value corresponding to either:
  - true, if both predicates return true for the element, or
  - false, if either parameter is NULL or either predicate returns false.
*/
bool
d_predicate_and_eval
(
    const struct d_predicate_and* _combo,
    const void*                   _element
)
{
    if ( (!_combo) ||
         (!_element) )
    {
        return false;
    }

    // check first predicate (short-circuit)
    if (!_combo->pred1(_element))
    {
        return false;
    }

    return _combo->pred2(_element);
}

/*
d_predicate_or_eval
  Evaluates a logical OR combination of two predicates against an element.

Parameter(s):
  _combo:   the OR predicate combinator containing two predicates.
  _element: the element to test against both predicates.
Return:
  A boolean value corresponding to either:
  - true, if either predicate returns true for the element, or
  - false, if either parameter is NULL or both predicates return false.
*/
bool
d_predicate_or_eval
(
    const struct d_predicate_or* _combo,
    const void*                  _element
)
{
    if ( (!_combo) ||
         (!_element) )
    {
        return false;
    }

    // check first predicate (short-circuit)
    if (_combo->pred1(_element))
    {
        return true;
    }

    return _combo->pred2(_element);
}

/*
d_predicate_not_eval
  Evaluates the logical negation of a predicate against an element.

Parameter(s):
  _combo:   the NOT predicate combinator containing one predicate.
  _element: the element to test against the predicate.
Return:
  A boolean value corresponding to either:
  - true, if the predicate returns false for the element, or
  - false, if either parameter is NULL or the predicate returns true.
*/
bool
d_predicate_not_eval
(
    const struct d_predicate_not* _combo,
    const void*                   _element
)
{
    if ( (!_combo) ||
         (!_element) )
    {
        return false;
    }

    return !(_combo->pred(_element));
}

/*
d_predicate_and_create
  Creates a logical AND predicate combinator from two predicates.

Parameter(s):
  _pred1: the first predicate function.
  _pred2: the second predicate function.
Return:
  A d_predicate_and struct containing both predicates.
*/
struct d_predicate_and
d_predicate_and_create
(
    d_predicate _pred1,
    d_predicate _pred2
)
{
    struct d_predicate_and result;

    result.pred1 = _pred1;
    result.pred2 = _pred2;

    return result;
}

/*
d_predicate_or_create
  Creates a logical OR predicate combinator from two predicates.

Parameter(s):
  _pred1: the first predicate function.
  _pred2: the second predicate function.
Return:
  A d_predicate_or struct containing both predicates.
*/
struct d_predicate_or
d_predicate_or_create
(
    d_predicate _pred1,
    d_predicate _pred2
)
{
    struct d_predicate_or result;

    result.pred1 = _pred1;
    result.pred2 = _pred2;

    return result;
}

/*
d_predicate_not_create
  Creates a logical NOT predicate combinator from a predicate.

Parameter(s):
  _pred: the predicate function to negate.
Return:
  A d_predicate_not struct containing the predicate.
*/
struct d_predicate_not
d_predicate_not_create
(
    d_predicate _pred
)
{
    struct d_predicate_not result;

    result.pred = _pred;

    return result;
}


///////////////////////////////////////////////////////////////////////////////
///             HIGHER-ORDER OPERATIONS                                     ///
///////////////////////////////////////////////////////////////////////////////

/*
d_map
  Applies a transformer to each element in an input array, writing results
to a corresponding output array.

Parameter(s):
  _input:        the source array of elements to transform.
  _output:       the destination array for transformed elements.
  _count:        the number of elements to transform.
  _element_size: the size in bytes of each element.
  _transform:    the transformer function to apply.
Return:
  A boolean value corresponding to either:
  - true, if all elements were transformed successfully, or
  - false, if any parameter is invalid or a transformation fails.
*/
bool
d_map
(
    const void*   _input,
    void*         _output,
    size_t        _count,
    size_t        _element_size,
    d_transformer _transform
)
{
    size_t      i;
    const char* in_bytes;
    char*       out_bytes;

    if ( (!_input)    ||
         (!_output)   ||
         (!_transform) ||
         (_element_size == 0) )
    {
        return false;
    }

    in_bytes  = (const char*)_input;
    out_bytes = (char*)_output;

    // apply transformer to each element
    for (i = 0; i < _count; i++)
    {
        if (!_transform(in_bytes + (i * _element_size),
                        out_bytes + (i * _element_size)))
        {
            return false;
        }
    }

    return true;
}

/*
d_filter
  Filters elements from an input array into an output array based on a
predicate function. Only elements for which the predicate returns true
are copied to the output.

Parameter(s):
  _input:        the source array of elements to filter.
  _output:       the destination array for matching elements.
  _count:        the number of elements in the input array.
  _element_size: the size in bytes of each element.
  _test:         the predicate function to test each element.
Return:
  The number of elements that passed the filter and were copied to the
output array. Returns 0 if any parameter is invalid.
*/
size_t
d_filter
(
    const void* _input,
    void*       _output,
    size_t      _count,
    size_t      _element_size,
    d_predicate _test
)
{
    size_t      i;
    size_t      out_count;
    const char* in_bytes;
    char*       out_bytes;
    const void* element;

    if ( (!_input)  ||
         (!_output) ||
         (!_test)   ||
         (_element_size == 0) )
    {
        return 0;
    }

    in_bytes  = (const char*)_input;
    out_bytes = (char*)_output;
    out_count = 0;

    // copy each matching element to output
    for (i = 0; i < _count; i++)
    {
        element = in_bytes + (i * _element_size);

        if (_test(element))
        {
            memcpy(out_bytes + (out_count * _element_size),
                   element,
                   _element_size);
            out_count++;
        }
    }

    return out_count;
}

/*
d_fold_left
  Left fold (reduce) over an array, accumulating from left to right:
f(f(f(init, a0), a1), a2)...

Parameter(s):
  _input:        the source array of elements.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _accumulator:  the initial accumulator value (modified in place).
  _combine:      the accumulator function combining state with elements.
Return:
  A boolean value corresponding to either:
  - true, if all elements were accumulated successfully, or
  - false, if any parameter is invalid or the combiner fails.
*/
bool
d_fold_left
(
    const void*   _input,
    size_t        _count,
    size_t        _element_size,
    void*         _accumulator,
    d_accumulator _combine
)
{
    size_t      i;
    const char* in_bytes;

    if ( (!_input)       ||
         (!_accumulator) ||
         (!_combine)     ||
         (_element_size == 0) )
    {
        return false;
    }

    in_bytes = (const char*)_input;

    // accumulate from left to right
    for (i = 0; i < _count; i++)
    {
        if (!_combine(_accumulator,
                      in_bytes + (i * _element_size)))
        {
            return false;
        }
    }

    return true;
}

/*
d_fold_right
  Right fold over an array, accumulating from right to left:
f(a0, f(a1, f(a2, init)))...

Parameter(s):
  _input:        the source array of elements.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _accumulator:  the initial accumulator value (modified in place).
  _combine:      the accumulator function combining state with elements.
Return:
  A boolean value corresponding to either:
  - true, if all elements were accumulated successfully, or
  - false, if any parameter is invalid or the combiner fails.
*/
bool
d_fold_right
(
    const void*   _input,
    size_t        _count,
    size_t        _element_size,
    void*         _accumulator,
    d_accumulator _combine
)
{
    ssize_t     i;
    const char* in_bytes;

    if ( (!_input)       ||
         (!_accumulator) ||
         (!_combine)     ||
         (_element_size == 0) )
    {
        return false;
    }

    in_bytes = (const char*)_input;

    // accumulate from right to left
    for (i = (ssize_t)_count - 1; i >= 0; i--)
    {
        if (!_combine(_accumulator,
                      in_bytes + (i * _element_size)))
        {
            return false;
        }
    }

    return true;
}

/*
d_for_each
  Applies a consumer function to each element in a mutable array.

Parameter(s):
  _input:        the array of elements to process.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _apply:        the consumer function to apply to each element.
Return:
  none.
*/
void
d_for_each
(
    void*      _input,
    size_t     _count,
    size_t     _element_size,
    d_consumer _apply
)
{
    size_t i;
    char*  bytes;

    if ( (!_input) ||
         (!_apply) ||
         (_element_size == 0) )
    {
        return;
    }

    bytes = (char*)_input;

    // apply consumer to each element
    for (i = 0; i < _count; i++)
    {
        _apply(bytes + (i * _element_size));
    }

    return;
}

/*
d_for_each_const
  Applies a const consumer function to each element in a read-only array.

Parameter(s):
  _input:        the array of elements to process.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _apply:        the const consumer function to apply to each element.
Return:
  none.
*/
void
d_for_each_const
(
    const void*      _input,
    size_t           _count,
    size_t           _element_size,
    d_consumer_const _apply
)
{
    size_t      i;
    const char* bytes;

    if ( (!_input) ||
         (!_apply) ||
         (_element_size == 0) )
    {
        return;
    }

    bytes = (const char*)_input;

    // apply const consumer to each element
    for (i = 0; i < _count; i++)
    {
        _apply(bytes + (i * _element_size));
    }

    return;
}

/*
d_any
  Tests whether any element in the array satisfies a predicate.

Parameter(s):
  _input:        the array of elements to test.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _test:         the predicate function to evaluate.
Return:
  A boolean value corresponding to either:
  - true, if at least one element satisfies the predicate, or
  - false, if no elements match or any parameter is invalid.
*/
bool
d_any
(
    const void* _input,
    size_t      _count,
    size_t      _element_size,
    d_predicate _test
)
{
    size_t      i;
    const char* bytes;

    if ( (!_input) ||
         (!_test)  ||
         (_element_size == 0) )
    {
        return false;
    }

    bytes = (const char*)_input;

    // return true on first match
    for (i = 0; i < _count; i++)
    {
        if (_test(bytes + (i * _element_size)))
        {
            return true;
        }
    }

    return false;
}

/*
d_all
  Tests whether all elements in the array satisfy a predicate.

Parameter(s):
  _input:        the array of elements to test.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _test:         the predicate function to evaluate.
Return:
  A boolean value corresponding to either:
  - true, if every element satisfies the predicate, or
  - false, if any element fails or any parameter is invalid.
*/
bool
d_all
(
    const void* _input,
    size_t      _count,
    size_t      _element_size,
    d_predicate _test
)
{
    size_t      i;
    const char* bytes;

    if ( (!_input) ||
         (!_test)  ||
         (_element_size == 0) )
    {
        return false;
    }

    bytes = (const char*)_input;

    // return false on first non-match
    for (i = 0; i < _count; i++)
    {
        if (!_test(bytes + (i * _element_size)))
        {
            return false;
        }
    }

    return true;
}

/*
d_none
  Tests whether no elements in the array satisfy a predicate.

Parameter(s):
  _input:        the array of elements to test.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _test:         the predicate function to evaluate.
Return:
  A boolean value corresponding to either:
  - true, if no elements satisfy the predicate, or
  - false, if any element matches or any parameter is invalid.
*/
bool
d_none
(
    const void* _input,
    size_t      _count,
    size_t      _element_size,
    d_predicate _test
)
{
    return !d_any(_input,
                  _count,
                  _element_size,
                  _test);
}

/*
d_count_if
  Counts the number of elements in the array satisfying a predicate.

Parameter(s):
  _input:        the array of elements to count.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _test:         the predicate function to evaluate.
Return:
  The number of elements matching the predicate. Returns 0 if any
parameter is invalid.
*/
size_t
d_count_if
(
    const void* _input,
    size_t      _count,
    size_t      _element_size,
    d_predicate _test
)
{
    size_t      i;
    size_t      match_count;
    const char* bytes;

    if ( (!_input) ||
         (!_test)  ||
         (_element_size == 0) )
    {
        return 0;
    }

    bytes       = (const char*)_input;
    match_count = 0;

    // count matching elements
    for (i = 0; i < _count; i++)
    {
        if (_test(bytes + (i * _element_size)))
        {
            match_count++;
        }
    }

    return match_count;
}

/*
d_find_if
  Finds the first element in the array satisfying a predicate.

Parameter(s):
  _input:        the array of elements to search.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _test:         the predicate function to evaluate.
Return:
  A pointer to the first matching element, or NULL if no element matches
or any parameter is invalid.
*/
void*
d_find_if
(
    const void* _input,
    size_t      _count,
    size_t      _element_size,
    d_predicate _test
)
{
    size_t      i;
    const char* bytes;
    const void* element;

    if ( (!_input) ||
         (!_test)  ||
         (_element_size == 0) )
    {
        return NULL;
    }

    bytes = (const char*)_input;

    // return first matching element
    for (i = 0; i < _count; i++)
    {
        element = bytes + (i * _element_size);

        if (_test(element))
        {
            return (void*)element;
        }
    }

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
///             UTILITY FUNCTIONS                                           ///
///////////////////////////////////////////////////////////////////////////////

/*
d_identity_transformer
  Identity transformer that performs no transformation.

Parameter(s):
  _input:  the input element.
  _output: the output element (unchanged).
Return:
  A boolean value corresponding to either:
  - true, if both parameters are non-NULL, or
  - false, if either parameter is NULL.
*/
bool
d_identity_transformer
(
    const void* _input,
    void*       _output
)
{
    return ((_input) && (_output));
}

/*
d_identity_predicate
  Identity predicate that tests whether an element is non-NULL.

Parameter(s):
  _element: the element to test.
Return:
  A boolean value corresponding to either:
  - true, if the element is non-NULL, or
  - false, if the element is NULL.
*/
bool
d_identity_predicate
(
    const void* _element
)
{
    return (_element != NULL);
}

/*
d_constant_true
  Predicate that always returns true regardless of input.

Parameter(s):
  _element: the element (unused).
Return:
  true, always.
*/
bool
d_constant_true
(
    const void* _element
)
{
    (void)_element;

    return true;
}

/*
d_constant_false
  Predicate that always returns false regardless of input.

Parameter(s):
  _element: the element (unused).
Return:
  false, always.
*/
bool
d_constant_false
(
    const void* _element
)
{
    (void)_element;

    return false;
}

/*
d_compare_int
  Three-way comparison for int values.

Parameter(s):
  _a: pointer to the first int value.
  _b: pointer to the second int value.
Return:
  -1 if *_a < *_b, 0 if equal, 1 if *_a > *_b. Returns 0 if either
parameter is NULL.
*/
int
d_compare_int
(
    const void* _a,
    const void* _b
)
{
    int a;
    int b;

    if ( (!_a) ||
         (!_b) )
    {
        return 0;
    }

    a = *(const int*)_a;
    b = *(const int*)_b;

    return (a < b) ? -1 : ((a > b) ? 1 : 0);
}

/*
d_compare_size_t
  Three-way comparison for size_t values.

Parameter(s):
  _a: pointer to the first size_t value.
  _b: pointer to the second size_t value.
Return:
  -1 if *_a < *_b, 0 if equal, 1 if *_a > *_b. Returns 0 if either
parameter is NULL.
*/
int
d_compare_size_t
(
    const void* _a,
    const void* _b
)
{
    size_t a;
    size_t b;

    if ( (!_a) ||
         (!_b) )
    {
        return 0;
    }

    a = *(const size_t*)_a;
    b = *(const size_t*)_b;

    return (a < b) ? -1 : ((a > b) ? 1 : 0);
}

/*
d_compare_double
  Three-way comparison for double values.

Parameter(s):
  _a: pointer to the first double value.
  _b: pointer to the second double value.
Return:
  -1 if *_a < *_b, 0 if equal, 1 if *_a > *_b. Returns 0 if either
parameter is NULL.
*/
int
d_compare_double
(
    const void* _a,
    const void* _b
)
{
    double a;
    double b;

    if ( (!_a) ||
         (!_b) )
    {
        return 0;
    }

    a = *(const double*)_a;
    b = *(const double*)_b;

    return (a < b) ? -1 : ((a > b) ? 1 : 0);
}

/*
d_equal_int
  Tests equality of two int values.

Parameter(s):
  _a: pointer to the first int value.
  _b: pointer to the second int value.
Return:
  A boolean value corresponding to either:
  - true, if both values are equal, or
  - false, if the values differ or either parameter is NULL.
*/
bool
d_equal_int
(
    const void* _a,
    const void* _b
)
{
    if ( (!_a) ||
         (!_b) )
    {
        return false;
    }

    return (*(const int*)_a == *(const int*)_b);
}

/*
d_equal_size_t
  Tests equality of two size_t values.

Parameter(s):
  _a: pointer to the first size_t value.
  _b: pointer to the second size_t value.
Return:
  A boolean value corresponding to either:
  - true, if both values are equal, or
  - false, if the values differ or either parameter is NULL.
*/
bool
d_equal_size_t
(
    const void* _a,
    const void* _b
)
{
    if ( (!_a) ||
         (!_b) )
    {
        return false;
    }

    return (*(const size_t*)_a == *(const size_t*)_b);
}

/*
d_is_null
  Tests whether an element pointer is NULL.

Parameter(s):
  _element: the element to test.
Return:
  A boolean value corresponding to either:
  - true, if the element is NULL, or
  - false, if the element is non-NULL.
*/
bool
d_is_null
(
    const void* _element
)
{
    return (_element == NULL);
}

/*
d_is_not_null
  Tests whether an element pointer is non-NULL.

Parameter(s):
  _element: the element to test.
Return:
  A boolean value corresponding to either:
  - true, if the element is non-NULL, or
  - false, if the element is NULL.
*/
bool
d_is_not_null
(
    const void* _element
)
{
    return (_element != NULL);
}

/*
d_pred_combo_and_eval
  Evaluates an inline AND combination of two predicates against an
element.

Parameter(s):
  _combo:   the AND predicate combo structure.
  _element: the element to test against both predicates.
Return:
  A boolean value corresponding to either:
  - true, if both predicates return true for the element, or
  - false, if either parameter is NULL or either predicate returns false.
*/
bool
d_pred_combo_and_eval
(
    const struct d_pred_combo_and* _combo,
    const void*                    _element
)
{
    if ( (!_combo) ||
         (!_element) )
    {
        return false;
    }

    // check first predicate (short-circuit)
    if (!_combo->pred1(_element))
    {
        return false;
    }

    return _combo->pred2(_element);
}

/*
d_pred_combo_or_eval
  Evaluates an inline OR combination of two predicates against an
element.

Parameter(s):
  _combo:   the OR predicate combo structure.
  _element: the element to test against both predicates.
Return:
  A boolean value corresponding to either:
  - true, if either predicate returns true for the element, or
  - false, if either parameter is NULL or both predicates return false.
*/
bool
d_pred_combo_or_eval
(
    const struct d_pred_combo_or* _combo,
    const void*                   _element
)
{
    if ( (!_combo) ||
         (!_element) )
    {
        return false;
    }

    // check first predicate (short-circuit)
    if (_combo->pred1(_element))
    {
        return true;
    }

    return _combo->pred2(_element);
}

/*
d_pred_combo_not_eval
  Evaluates an inline NOT of a predicate against an element.

Parameter(s):
  _combo:   the NOT predicate combo structure.
  _element: the element to test against the predicate.
Return:
  A boolean value corresponding to either:
  - true, if the predicate returns false for the element, or
  - false, if either parameter is NULL or the predicate returns true.
*/
bool
d_pred_combo_not_eval
(
    const struct d_pred_combo_not* _combo,
    const void*                    _element
)
{
    if ( (!_combo) ||
         (!_element) )
    {
        return false;
    }

    return !_combo->pred(_element);
}


///////////////////////////////////////////////////////////////////////////////
///             CONTEXT PREDICATE COMBINATORS                               ///
///////////////////////////////////////////////////////////////////////////////

/*
d_predicate_and_ctx_eval
  Evaluates a logical AND combination of two context predicates against
an element.

Parameter(s):
  _combo:   the AND context predicate combinator.
  _element: the element to test against both predicates.
Return:
  A boolean value corresponding to either:
  - true, if both predicates return true for the element, or
  - false, if either parameter is NULL or either predicate returns false.
*/
bool
d_predicate_and_ctx_eval
(
    const struct d_predicate_and_ctx* _combo,
    const void*                       _element
)
{
    if ( (!_combo) ||
         (!_element) )
    {
        return false;
    }

    // check first predicate (short-circuit)
    if (!_combo->pred1(_element, _combo->context1))
    {
        return false;
    }

    return _combo->pred2(_element, _combo->context2);
}

/*
d_predicate_or_ctx_eval
  Evaluates a logical OR combination of two context predicates against
an element.

Parameter(s):
  _combo:   the OR context predicate combinator.
  _element: the element to test against both predicates.
Return:
  A boolean value corresponding to either:
  - true, if either predicate returns true for the element, or
  - false, if either parameter is NULL or both predicates return false.
*/
bool
d_predicate_or_ctx_eval
(
    const struct d_predicate_or_ctx* _combo,
    const void*                      _element
)
{
    if ( (!_combo) ||
         (!_element) )
    {
        return false;
    }

    // check first predicate (short-circuit)
    if (_combo->pred1(_element, _combo->context1))
    {
        return true;
    }

    return _combo->pred2(_element, _combo->context2);
}

/*
d_predicate_not_ctx_eval
  Evaluates the logical negation of a context predicate against an
element.

Parameter(s):
  _combo:   the NOT context predicate combinator.
  _element: the element to test against the predicate.
Return:
  A boolean value corresponding to either:
  - true, if the predicate returns false for the element, or
  - false, if either parameter is NULL or the predicate returns true.
*/
bool
d_predicate_not_ctx_eval
(
    const struct d_predicate_not_ctx* _combo,
    const void*                       _element
)
{
    if ( (!_combo) ||
         (!_element) )
    {
        return false;
    }

    return !_combo->pred(_element, _combo->context);
}


///////////////////////////////////////////////////////////////////////////////
///             FUNCTIONAL COMPOSITION                                      ///
///////////////////////////////////////////////////////////////////////////////

/*
d_compose_transformers
  Creates a composition of two transformers. The result represents
(second . first), i.e., first is applied, then second is applied to
the intermediate result.

Parameter(s):
  _first:     the transformer applied first.
  _second:    the transformer applied second.
  _temp_size: the size of the intermediate result buffer.
Return:
  A d_composed_transformer struct configured for composition.
*/
struct d_composed_transformer
d_compose_transformers
(
    d_transformer _first,
    d_transformer _second,
    size_t        _temp_size
)
{
    struct d_composed_transformer composed;

    composed.first     = _first;
    composed.second    = _second;
    composed.temp_size = _temp_size;
    composed.temp_buf  = NULL;

    // allocate intermediate buffer
    if (_temp_size > 0)
    {
        composed.temp_buf = malloc(_temp_size);
    }

    return composed;
}

/*
d_composed_transformer_apply
  Applies a composed transformer to an input, producing an output.

Parameter(s):
  _composed: the composed transformer to apply.
  _input:    the input element.
  _output:   the output element.
Return:
  A boolean value corresponding to either:
  - true, if both transformations succeeded, or
  - false, if any parameter is invalid or a transformation fails.
*/
bool
d_composed_transformer_apply
(
    const struct d_composed_transformer* _composed,
    const void*                          _input,
    void*                                _output
)
{
    if ( (!_composed)          ||
         (!_input)             ||
         (!_output)            ||
         (!_composed->temp_buf) )
    {
        return false;
    }

    // apply first transformer to intermediate buffer
    if (!_composed->first(_input, _composed->temp_buf))
    {
        return false;
    }

    // apply second transformer from intermediate to output
    return _composed->second(_composed->temp_buf, _output);
}

/*
d_composed_transformer_free
  Frees resources owned by a composed transformer.

Parameter(s):
  _composed: the composed transformer to clean up; may be NULL.
Return:
  none.
*/
void
d_composed_transformer_free
(
    struct d_composed_transformer* _composed
)
{
    if (!_composed)
    {
        return;
    }

    if (_composed->temp_buf)
    {
        free(_composed->temp_buf);
        _composed->temp_buf = NULL;
    }

    return;
}

/*
d_partial_consumer_create
  Creates a partial consumer by binding a context to a consumer
function.

Parameter(s):
  _fn:      the context consumer function.
  _context: the context to bind.
Return:
  A d_partial_consumer struct with the bound context.
*/
struct d_partial_consumer
d_partial_consumer_create
(
    d_consumer_ctx _fn,
    void*          _context
)
{
    struct d_partial_consumer partial;

    partial.fn      = _fn;
    partial.context = _context;

    return partial;
}

/*
d_partial_consumer_apply
  Applies a partial consumer to an element using the bound context.

Parameter(s):
  _partial: the partial consumer to apply.
  _element: the element to consume.
Return:
  none.
*/
void
d_partial_consumer_apply
(
    const struct d_partial_consumer* _partial,
    void*                            _element
)
{
    if ( (!_partial)    ||
         (!_partial->fn) )
    {
        return;
    }

    _partial->fn(_element, _partial->context);

    return;
}


///////////////////////////////////////////////////////////////////////////////
///             CONTEXT HIGHER-ORDER OPERATIONS                             ///
///////////////////////////////////////////////////////////////////////////////

/*
d_map_ctx
  Applies a context transformer to each element in an input array,
writing results to a corresponding output array.

Parameter(s):
  _input:        the source array of elements to transform.
  _output:       the destination array for transformed elements.
  _count:        the number of elements to transform.
  _element_size: the size in bytes of each element.
  _transform:    the context transformer function to apply.
  _context:      the context pointer passed to the transformer.
Return:
  A boolean value corresponding to either:
  - true, if all elements were transformed successfully, or
  - false, if any parameter is invalid or a transformation fails.
*/
bool
d_map_ctx
(
    const void*       _input,
    void*             _output,
    size_t            _count,
    size_t            _element_size,
    d_transformer_ctx _transform,
    void*             _context
)
{
    size_t      i;
    const char* in_bytes;
    char*       out_bytes;

    if ( (!_input)     ||
         (!_output)    ||
         (!_transform) ||
         (_element_size == 0) )
    {
        return false;
    }

    in_bytes  = (const char*)_input;
    out_bytes = (char*)_output;

    // apply context transformer to each element
    for (i = 0; i < _count; i++)
    {
        if (!_transform(in_bytes + (i * _element_size),
                        out_bytes + (i * _element_size),
                        _context))
        {
            return false;
        }
    }

    return true;
}

/*
d_filter_ctx
  Filters elements from an input array into an output array based on
a context predicate. Only elements for which the predicate returns true
are copied to the output.

Parameter(s):
  _input:        the source array of elements to filter.
  _output:       the destination array for matching elements.
  _count:        the number of elements in the input array.
  _element_size: the size in bytes of each element.
  _test:         the context predicate function to test each element.
  _context:      the context pointer passed to the predicate.
Return:
  The number of elements that passed the filter and were copied to the
output array. Returns 0 if any parameter is invalid.
*/
size_t
d_filter_ctx
(
    const void*     _input,
    void*           _output,
    size_t          _count,
    size_t          _element_size,
    d_predicate_ctx _test,
    void*           _context
)
{
    size_t      i;
    size_t      out_count;
    const char* in_bytes;
    char*       out_bytes;
    const void* element;

    if ( (!_input)  ||
         (!_output) ||
         (!_test)   ||
         (_element_size == 0) )
    {
        return 0;
    }

    in_bytes  = (const char*)_input;
    out_bytes = (char*)_output;
    out_count = 0;

    // copy each matching element to output
    for (i = 0; i < _count; i++)
    {
        element = in_bytes + (i * _element_size);

        if (_test(element, _context))
        {
            memcpy(out_bytes + (out_count * _element_size),
                   element,
                   _element_size);
            out_count++;
        }
    }

    return out_count;
}

/*
d_fold_left_ctx
  Left fold (reduce) with context over an array, accumulating from
left to right.

Parameter(s):
  _input:        the source array of elements.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _accumulator:  the initial accumulator value (modified in place).
  _combine:      the context accumulator function.
  _context:      the context pointer passed to the combiner.
Return:
  A boolean value corresponding to either:
  - true, if all elements were accumulated successfully, or
  - false, if any parameter is invalid or the combiner fails.
*/
bool
d_fold_left_ctx
(
    const void*       _input,
    size_t            _count,
    size_t            _element_size,
    void*             _accumulator,
    d_accumulator_ctx _combine,
    void*             _context
)
{
    size_t      i;
    const char* in_bytes;

    if ( (!_input)       ||
         (!_accumulator) ||
         (!_combine)     ||
         (_element_size == 0) )
    {
        return false;
    }

    in_bytes = (const char*)_input;

    // accumulate from left to right with context
    for (i = 0; i < _count; i++)
    {
        if (!_combine(_accumulator,
                      in_bytes + (i * _element_size),
                      _context))
        {
            return false;
        }
    }

    return true;
}

/*
d_fold_right_ctx
  Right fold with context over an array, accumulating from right to
left.

Parameter(s):
  _input:        the source array of elements.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _accumulator:  the initial accumulator value (modified in place).
  _combine:      the context accumulator function.
  _context:      the context pointer passed to the combiner.
Return:
  A boolean value corresponding to either:
  - true, if all elements were accumulated successfully, or
  - false, if any parameter is invalid or the combiner fails.
*/
bool
d_fold_right_ctx
(
    const void*       _input,
    size_t            _count,
    size_t            _element_size,
    void*             _accumulator,
    d_accumulator_ctx _combine,
    void*             _context
)
{
    ssize_t     i;
    const char* in_bytes;

    if ( (!_input)       ||
         (!_accumulator) ||
         (!_combine)     ||
         (_element_size == 0) )
    {
        return false;
    }

    in_bytes = (const char*)_input;

    // accumulate from right to left with context
    for (i = (ssize_t)_count - 1; i >= 0; i--)
    {
        if (!_combine(_accumulator,
                      in_bytes + (i * _element_size),
                      _context))
        {
            return false;
        }
    }

    return true;
}

/*
d_for_each_ctx
  Applies a context consumer function to each element in a mutable
array.

Parameter(s):
  _input:        the array of elements to process.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _apply:        the context consumer function to apply.
  _context:      the context pointer passed to the consumer.
Return:
  none.
*/
void
d_for_each_ctx
(
    void*          _input,
    size_t         _count,
    size_t         _element_size,
    d_consumer_ctx _apply,
    void*          _context
)
{
    size_t i;
    char*  bytes;

    if ( (!_input) ||
         (!_apply) ||
         (_element_size == 0) )
    {
        return;
    }

    bytes = (char*)_input;

    // apply context consumer to each element
    for (i = 0; i < _count; i++)
    {
        _apply(bytes + (i * _element_size), _context);
    }

    return;
}

/*
d_any_ctx
  Tests whether any element in the array satisfies a context predicate.

Parameter(s):
  _input:        the array of elements to test.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _test:         the context predicate function to evaluate.
  _context:      the context pointer passed to the predicate.
Return:
  A boolean value corresponding to either:
  - true, if at least one element satisfies the predicate, or
  - false, if no elements match or any parameter is invalid.
*/
bool
d_any_ctx
(
    const void*     _input,
    size_t          _count,
    size_t          _element_size,
    d_predicate_ctx _test,
    void*           _context
)
{
    size_t      i;
    const char* bytes;

    if ( (!_input) ||
         (!_test)  ||
         (_element_size == 0) )
    {
        return false;
    }

    bytes = (const char*)_input;

    // return true on first match
    for (i = 0; i < _count; i++)
    {
        if (_test(bytes + (i * _element_size), _context))
        {
            return true;
        }
    }

    return false;
}

/*
d_all_ctx
  Tests whether all elements in the array satisfy a context predicate.

Parameter(s):
  _input:        the array of elements to test.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _test:         the context predicate function to evaluate.
  _context:      the context pointer passed to the predicate.
Return:
  A boolean value corresponding to either:
  - true, if every element satisfies the predicate, or
  - false, if any element fails or any parameter is invalid.
*/
bool
d_all_ctx
(
    const void*     _input,
    size_t          _count,
    size_t          _element_size,
    d_predicate_ctx _test,
    void*           _context
)
{
    size_t      i;
    const char* bytes;

    if ( (!_input) ||
         (!_test)  ||
         (_element_size == 0) )
    {
        return false;
    }

    bytes = (const char*)_input;

    // return false on first non-match
    for (i = 0; i < _count; i++)
    {
        if (!_test(bytes + (i * _element_size), _context))
        {
            return false;
        }
    }

    return true;
}

/*
d_none_ctx
  Tests whether no elements in the array satisfy a context predicate.

Parameter(s):
  _input:        the array of elements to test.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _test:         the context predicate function to evaluate.
  _context:      the context pointer passed to the predicate.
Return:
  A boolean value corresponding to either:
  - true, if no elements satisfy the predicate, or
  - false, if any element matches or any parameter is invalid.
*/
bool
d_none_ctx
(
    const void*     _input,
    size_t          _count,
    size_t          _element_size,
    d_predicate_ctx _test,
    void*           _context
)
{
    return !d_any_ctx(_input,
                      _count,
                      _element_size,
                      _test,
                      _context);
}

/*
d_count_if_ctx
  Counts the number of elements satisfying a context predicate.

Parameter(s):
  _input:        the array of elements to count.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _test:         the context predicate function to evaluate.
  _context:      the context pointer passed to the predicate.
Return:
  The number of elements matching the predicate. Returns 0 if any
parameter is invalid.
*/
size_t
d_count_if_ctx
(
    const void*     _input,
    size_t          _count,
    size_t          _element_size,
    d_predicate_ctx _test,
    void*           _context
)
{
    size_t      i;
    size_t      match_count;
    const char* bytes;

    if ( (!_input) ||
         (!_test)  ||
         (_element_size == 0) )
    {
        return 0;
    }

    bytes       = (const char*)_input;
    match_count = 0;

    // count matching elements
    for (i = 0; i < _count; i++)
    {
        if (_test(bytes + (i * _element_size), _context))
        {
            match_count++;
        }
    }

    return match_count;
}

/*
d_find_if_ctx
  Finds the first element satisfying a context predicate.

Parameter(s):
  _input:        the array of elements to search.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
  _test:         the context predicate function to evaluate.
  _context:      the context pointer passed to the predicate.
Return:
  A pointer to the first matching element, or NULL if no element matches
or any parameter is invalid.
*/
void*
d_find_if_ctx
(
    const void*     _input,
    size_t          _count,
    size_t          _element_size,
    d_predicate_ctx _test,
    void*           _context
)
{
    size_t      i;
    const char* bytes;
    const void* element;

    if ( (!_input) ||
         (!_test)  ||
         (_element_size == 0) )
    {
        return NULL;
    }

    bytes = (const char*)_input;

    // return first matching element
    for (i = 0; i < _count; i++)
    {
        element = bytes + (i * _element_size);

        if (_test(element, _context))
        {
            return (void*)element;
        }
    }

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
///             FUNCTION PIPELINE                                           ///
///////////////////////////////////////////////////////////////////////////////

/*
d_fn_pipeline_begin
  Creates a function pipeline from existing mutable data. The pipeline
does not own the data.

Parameter(s):
  _data:         the data array to operate on.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
Return:
  A d_fn_pipeline struct initialized with the given data.
*/
struct d_fn_pipeline
d_fn_pipeline_begin
(
    void*  _data,
    size_t _count,
    size_t _element_size
)
{
    struct d_fn_pipeline pipe;

    pipe.data         = _data;
    pipe.count        = _count;
    pipe.element_size = _element_size;
    pipe.owns_data    = false;
    pipe.error_code   = 0;

    return pipe;
}

/*
d_fn_pipeline_begin_copy
  Creates a function pipeline by copying data. The pipeline owns the
copied data.

Parameter(s):
  _data:         the source data array to copy from.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
Return:
  A d_fn_pipeline struct initialized with a copy of the data.
*/
struct d_fn_pipeline
d_fn_pipeline_begin_copy
(
    const void* _data,
    size_t      _count,
    size_t      _element_size
)
{
    struct d_fn_pipeline pipe;

    pipe.data         = NULL;
    pipe.count        = _count;
    pipe.element_size = _element_size;
    pipe.owns_data    = true;
    pipe.error_code   = 0;

    if ( (!_data) ||
         (_element_size == 0) )
    {
        pipe.error_code = -1;

        return pipe;
    }

    // allocate and copy data
    pipe.data = malloc(_count * _element_size);

    if (!pipe.data)
    {
        pipe.error_code = -1;

        return pipe;
    }

    memcpy(pipe.data, _data, _count * _element_size);

    return pipe;
}

/*
d_fn_pipeline_map
  Applies a transformer to each element in the pipeline.

Parameter(s):
  _pipe:      the pipeline to operate on.
  _transform: the transformer function to apply.
Return:
  The updated pipeline with transformed data.
*/
struct d_fn_pipeline
d_fn_pipeline_map
(
    struct d_fn_pipeline _pipe,
    d_transformer        _transform
)
{
    void* output;

    // propagate errors
    if (_pipe.error_code != 0)
    {
        return _pipe;
    }

    if ( (!_pipe.data) ||
         (!_transform) )
    {
        _pipe.error_code = -1;

        return _pipe;
    }

    // allocate output buffer
    output = malloc(_pipe.count * _pipe.element_size);

    if (!output)
    {
        _pipe.error_code = -1;

        return _pipe;
    }

    // apply transformation
    if (!d_map(_pipe.data,
               output,
               _pipe.count,
               _pipe.element_size,
               _transform))
    {
        free(output);
        _pipe.error_code = -1;

        return _pipe;
    }

    // replace pipeline data
    if (_pipe.owns_data)
    {
        free(_pipe.data);
    }

    _pipe.data      = output;
    _pipe.owns_data = true;

    return _pipe;
}

/*
d_fn_pipeline_filter
  Filters elements in the pipeline using a predicate.

Parameter(s):
  _pipe: the pipeline to operate on.
  _test: the predicate function to test each element.
Return:
  The updated pipeline with only matching elements.
*/
struct d_fn_pipeline
d_fn_pipeline_filter
(
    struct d_fn_pipeline _pipe,
    d_predicate          _test
)
{
    void*  output;
    size_t out_count;

    // propagate errors
    if (_pipe.error_code != 0)
    {
        return _pipe;
    }

    if ( (!_pipe.data) ||
         (!_test) )
    {
        _pipe.error_code = -1;

        return _pipe;
    }

    // allocate output buffer (worst case: all elements pass)
    output = malloc(_pipe.count * _pipe.element_size);

    if (!output)
    {
        _pipe.error_code = -1;

        return _pipe;
    }

    // apply filter
    out_count = d_filter(_pipe.data,
                         output,
                         _pipe.count,
                         _pipe.element_size,
                         _test);

    // replace pipeline data
    if (_pipe.owns_data)
    {
        free(_pipe.data);
    }

    _pipe.data      = output;
    _pipe.count     = out_count;
    _pipe.owns_data = true;

    return _pipe;
}

/*
d_fn_pipeline_fold
  Folds the pipeline elements into an accumulator.

Parameter(s):
  _pipe:    the pipeline to operate on.
  _initial: the initial accumulator value.
  _combine: the accumulator function.
Return:
  The pipeline after folding (count becomes 0).
*/
struct d_fn_pipeline
d_fn_pipeline_fold
(
    struct d_fn_pipeline _pipe,
    void*                _initial,
    d_accumulator        _combine
)
{
    // propagate errors
    if (_pipe.error_code != 0)
    {
        return _pipe;
    }

    if ( (!_pipe.data) ||
         (!_initial)   ||
         (!_combine) )
    {
        _pipe.error_code = -1;

        return _pipe;
    }

    // apply fold
    if (!d_fold_left(_pipe.data,
                     _pipe.count,
                     _pipe.element_size,
                     _initial,
                     _combine))
    {
        _pipe.error_code = -1;
    }

    return _pipe;
}

/*
d_fn_pipeline_for_each
  Applies a consumer to each element in the pipeline.

Parameter(s):
  _pipe:  the pipeline to operate on.
  _apply: the consumer function to apply.
Return:
  The unchanged pipeline.
*/
struct d_fn_pipeline
d_fn_pipeline_for_each
(
    struct d_fn_pipeline _pipe,
    d_consumer           _apply
)
{
    // propagate errors
    if (_pipe.error_code != 0)
    {
        return _pipe;
    }

    if ( (!_pipe.data) ||
         (!_apply) )
    {
        _pipe.error_code = -1;

        return _pipe;
    }

    d_for_each(_pipe.data,
               _pipe.count,
               _pipe.element_size,
               _apply);

    return _pipe;
}

/*
d_fn_pipeline_take
  Takes the first n elements from the pipeline.

Parameter(s):
  _pipe: the pipeline to operate on.
  _n:    the number of elements to take.
Return:
  The updated pipeline with at most n elements.
*/
struct d_fn_pipeline
d_fn_pipeline_take
(
    struct d_fn_pipeline _pipe,
    size_t               _n
)
{
    // propagate errors
    if (_pipe.error_code != 0)
    {
        return _pipe;
    }

    // clamp to available count
    if (_n < _pipe.count)
    {
        _pipe.count = _n;
    }

    return _pipe;
}

/*
d_fn_pipeline_skip
  Skips the first n elements in the pipeline.

Parameter(s):
  _pipe: the pipeline to operate on.
  _n:    the number of elements to skip.
Return:
  The updated pipeline with the first n elements removed.
*/
struct d_fn_pipeline
d_fn_pipeline_skip
(
    struct d_fn_pipeline _pipe,
    size_t               _n
)
{
    size_t skip;
    void*  new_data;
    size_t new_count;

    // propagate errors
    if (_pipe.error_code != 0)
    {
        return _pipe;
    }

    // clamp skip amount
    skip = (_n < _pipe.count) ? _n : _pipe.count;
    new_count = _pipe.count - skip;

    if (new_count == 0)
    {
        if (_pipe.owns_data)
        {
            free(_pipe.data);
        }

        _pipe.data  = NULL;
        _pipe.count = 0;

        return _pipe;
    }

    // allocate new buffer for remaining elements
    new_data = malloc(new_count * _pipe.element_size);

    if (!new_data)
    {
        _pipe.error_code = -1;

        return _pipe;
    }

    memcpy(new_data,
           (const char*)_pipe.data
               + (skip * _pipe.element_size),
           new_count * _pipe.element_size);

    // replace pipeline data
    if (_pipe.owns_data)
    {
        free(_pipe.data);
    }

    _pipe.data      = new_data;
    _pipe.count     = new_count;
    _pipe.owns_data = true;

    return _pipe;
}

/*
d_fn_pipeline_end
  Finalizes the pipeline and returns the resulting data.

Parameter(s):
  _pipe:      the pipeline to finalize.
  _out_count: output parameter for the element count.
Return:
  A pointer to the pipeline data. The caller takes ownership.
*/
void*
d_fn_pipeline_end
(
    struct d_fn_pipeline _pipe,
    size_t*              _out_count
)
{
    if (_out_count)
    {
        *_out_count = _pipe.count;
    }

    return _pipe.data;
}

/*
d_fn_pipeline_free
  Frees resources owned by a pipeline without returning the data.

Parameter(s):
  _pipe: the pipeline to free.
Return:
  none.
*/
void
d_fn_pipeline_free
(
    struct d_fn_pipeline _pipe
)
{
    if ( (_pipe.owns_data) &&
         (_pipe.data) )
    {
        free(_pipe.data);
    }

    return;
}


///////////////////////////////////////////////////////////////////////////////
///             FLUENT BUILDER                                              ///
///////////////////////////////////////////////////////////////////////////////

/*
d_fn_builder_new
  Creates a new empty function builder.

Parameter(s):
  (none)
Return:
  A pointer to a newly allocated d_fn_builder, or NULL if allocation
fails.
*/
struct d_fn_builder*
d_fn_builder_new
(
    void
)
{
    struct d_fn_builder* builder;
    size_t               initial_capacity;

    builder = (struct d_fn_builder*)malloc(
                  sizeof(struct d_fn_builder));

    // check allocation
    if (!builder)
    {
        return NULL;
    }

    initial_capacity = 8;

    builder->transforms = (d_transformer*)malloc(
                              initial_capacity
                                  * sizeof(d_transformer));
    builder->predicates = (d_predicate*)malloc(
                              initial_capacity
                                  * sizeof(d_predicate));

    // check allocations
    if ( (!builder->transforms) ||
         (!builder->predicates) )
    {
        free(builder->transforms);
        free(builder->predicates);
        free(builder);

        return NULL;
    }

    builder->transform_count = 0;
    builder->predicate_count = 0;
    builder->capacity        = initial_capacity;

    return builder;
}

/*
d_fn_builder_map
  Adds a map (transform) operation to the builder.

Parameter(s):
  _builder:   the builder to add the operation to.
  _transform: the transformer function.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_fn_builder*
d_fn_builder_map
(
    struct d_fn_builder* _builder,
    d_transformer        _transform
)
{
    if ( (!_builder) ||
         (!_transform) )
    {
        return NULL;
    }

    // grow capacity if needed
    if (_builder->transform_count >= _builder->capacity)
    {
        return NULL;
    }

    _builder->transforms[_builder->transform_count] =
        _transform;
    _builder->transform_count++;

    return _builder;
}

/*
d_fn_builder_filter
  Adds a filter (predicate) operation to the builder.

Parameter(s):
  _builder: the builder to add the operation to.
  _test:    the predicate function.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_fn_builder*
d_fn_builder_filter
(
    struct d_fn_builder* _builder,
    d_predicate          _test
)
{
    if ( (!_builder) ||
         (!_test) )
    {
        return NULL;
    }

    // grow capacity if needed
    if (_builder->predicate_count >= _builder->capacity)
    {
        return NULL;
    }

    _builder->predicates[_builder->predicate_count] = _test;
    _builder->predicate_count++;

    return _builder;
}

/*
d_fn_builder_and_then
  Adds a chained transform operation to the builder.

Parameter(s):
  _builder:   the builder to add the operation to.
  _transform: the transformer function.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_fn_builder*
d_fn_builder_and_then
(
    struct d_fn_builder* _builder,
    d_transformer        _transform
)
{
    return d_fn_builder_map(_builder, _transform);
}

/*
d_fn_builder_where
  Adds a where (predicate) operation to the builder.

Parameter(s):
  _builder: the builder to add the operation to.
  _test:    the predicate function.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_fn_builder*
d_fn_builder_where
(
    struct d_fn_builder* _builder,
    d_predicate          _test
)
{
    return d_fn_builder_filter(_builder, _test);
}

/*
d_fn_builder_execute
  Executes the builder's operations on input data.

Parameter(s):
  _builder:      the builder containing the operations.
  _input:        the source array.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
  _output:       the destination array.
  _out_count:    output parameter for result count.
Return:
  A boolean value corresponding to either:
  - true, if all operations completed successfully, or
  - false, if any parameter is invalid or an operation fails.
*/
bool
d_fn_builder_execute
(
    const struct d_fn_builder* _builder,
    const void*                _input,
    size_t                     _count,
    size_t                     _element_size,
    void*                      _output,
    size_t*                    _out_count
)
{
    size_t i;
    void*  temp1;
    void*  temp2;
    size_t current_count;

    if ( (!_builder) ||
         (!_input)   ||
         (!_output)  ||
         (_element_size == 0) )
    {
        return false;
    }

    // allocate temporary buffers
    temp1 = malloc(_count * _element_size);
    temp2 = malloc(_count * _element_size);

    if ( (!temp1) ||
         (!temp2) )
    {
        free(temp1);
        free(temp2);

        return false;
    }

    memcpy(temp1, _input, _count * _element_size);
    current_count = _count;

    // apply all predicate filters
    for (i = 0; i < _builder->predicate_count; i++)
    {
        current_count = d_filter(temp1,
                                 temp2,
                                 current_count,
                                 _element_size,
                                 _builder->predicates[i]);
        memcpy(temp1, temp2, current_count * _element_size);
    }

    // apply all transformers
    for (i = 0; i < _builder->transform_count; i++)
    {
        if (!d_map(temp1,
                   temp2,
                   current_count,
                   _element_size,
                   _builder->transforms[i]))
        {
            free(temp1);
            free(temp2);

            return false;
        }

        memcpy(temp1, temp2, current_count * _element_size);
    }

    memcpy(_output, temp1, current_count * _element_size);

    if (_out_count)
    {
        *_out_count = current_count;
    }

    free(temp1);
    free(temp2);

    return true;
}

/*
d_fn_builder_free
  Frees all resources owned by a function builder.

Parameter(s):
  _builder: the builder to free; may be NULL.
Return:
  none.
*/
void
d_fn_builder_free
(
    struct d_fn_builder* _builder
)
{
    if (!_builder)
    {
        return;
    }

    if (_builder->transforms)
    {
        free(_builder->transforms);
    }

    if (_builder->predicates)
    {
        free(_builder->predicates);
    }

    free(_builder);

    return;
}
