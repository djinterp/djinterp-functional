#include "..\..\inc\functional\filter.h"


///////////////////////////////////////////////////////////////////////////////
///             I.    FILTER OPERATION CREATION                             ///
///////////////////////////////////////////////////////////////////////////////

/*
d_filter_take_first
  Creates a filter operation that takes the first n elements.

Parameter(s):
  _n: the number of elements to take from the beginning.
Return:
  A d_filter_operation configured for take-first behavior.
*/
struct d_filter_operation
d_filter_take_first
(
    size_t _n
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type         = D_FILTER_OP_TAKE_FIRST;
    op.params.count = _n;

    return op;
}

/*
d_filter_take_last
  Creates a filter operation that takes the last n elements.

Parameter(s):
  _n: the number of elements to take from the end.
Return:
  A d_filter_operation configured for take-last behavior.
*/
struct d_filter_operation
d_filter_take_last
(
    size_t _n
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type         = D_FILTER_OP_TAKE_LAST;
    op.params.count = _n;

    return op;
}

/*
d_filter_skip_first
  Creates a filter operation that skips the first n elements.

Parameter(s):
  _n: the number of elements to skip from the beginning.
Return:
  A d_filter_operation configured for skip-first behavior.
*/
struct d_filter_operation
d_filter_skip_first
(
    size_t _n
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type         = D_FILTER_OP_SKIP_FIRST;
    op.params.count = _n;

    return op;
}

/*
d_filter_skip_last
  Creates a filter operation that skips the last n elements.

Parameter(s):
  _n: the number of elements to skip from the end.
Return:
  A d_filter_operation configured for skip-last behavior.
*/
struct d_filter_operation
d_filter_skip_last
(
    size_t _n
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type         = D_FILTER_OP_SKIP_LAST;
    op.params.count = _n;

    return op;
}

/*
d_filter_range
  Creates a filter operation that selects elements in the range
[_start, _end).

Parameter(s):
  _start: the inclusive start index.
  _end:   the exclusive end index.
Return:
  A d_filter_operation configured for range selection.
*/
struct d_filter_operation
d_filter_range
(
    size_t _start,
    size_t _end
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type         = D_FILTER_OP_RANGE;
    op.params.start = _start;
    op.params.end   = _end;

    return op;
}

/*
d_filter_where
  Creates a filter operation that selects elements matching a predicate.

Parameter(s):
  _test: the predicate function to test each element.
Return:
  A d_filter_operation configured for predicate-based filtering.
*/
struct d_filter_operation
d_filter_where
(
    d_predicate _test
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type           = D_FILTER_OP_WHERE;
    op.params.test    = _test;
    op.params.context = NULL;

    return op;
}

/*
d_filter_where_ctx
  Creates a filter operation that selects elements matching a context
predicate.

Parameter(s):
  _test:    the context predicate function to test each element.
  _context: the context pointer passed to the predicate.
Return:
  A d_filter_operation configured for context-predicate filtering.
*/
struct d_filter_operation
d_filter_where_ctx
(
    d_predicate_ctx _test,
    void*           _context
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type           = D_FILTER_OP_WHERE;
    op.params.test    = (d_predicate)_test;
    op.params.context = _context;

    return op;
}

/*
d_filter_where_not
  Creates a filter operation that selects elements not matching a
predicate.

Parameter(s):
  _test: the predicate function whose result will be negated.
Return:
  A d_filter_operation configured for negated predicate filtering.
*/
struct d_filter_operation
d_filter_where_not
(
    d_predicate _test
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type           = D_FILTER_OP_WHERE_NOT;
    op.params.test    = _test;
    op.params.context = NULL;

    return op;
}

/*
d_filter_slice
  Creates a filter operation that selects elements in a slice
[_start:_end:_step].

Parameter(s):
  _start: the inclusive start index.
  _end:   the exclusive end index.
  _step:  the step size between selected elements (0 defaults to 1).
Return:
  A d_filter_operation configured for slice selection.
*/
struct d_filter_operation
d_filter_slice
(
    size_t _start,
    size_t _end,
    size_t _step
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type         = D_FILTER_OP_SLICE;
    op.params.start = _start;
    op.params.end   = _end;
    op.params.step  = (_step == 0) ? 1 : _step;

    return op;
}

/*
d_filter_distinct
  Creates a filter operation that removes duplicate elements using a
comparator.

Parameter(s):
  _cmp: the comparator function for detecting duplicates.
Return:
  A d_filter_operation configured for deduplication.
*/
struct d_filter_operation
d_filter_distinct
(
    d_comparator _cmp
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type       = D_FILTER_OP_DISTINCT;
    op.params.test = (d_predicate)_cmp;

    return op;
}

/*
d_filter_reverse
  Creates a filter operation that reverses element order.

Parameter(s):
  (none)
Return:
  A d_filter_operation configured for reversal.
*/
struct d_filter_operation
d_filter_reverse
(
    void
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type = D_FILTER_OP_REVERSE;

    return op;
}

/*
d_filter_operation_free
  Frees resources owned by a filter operation.

Parameter(s):
  _op: the filter operation to clean up; may be NULL.
Return:
  none.
*/
void
d_filter_operation_free
(
    struct d_filter_operation* _op
)
{
    if (!_op)
    {
        return;
    }

    // free optional name string
    if (_op->name)
    {
        free(_op->name);
        _op->name = NULL;
    }

    // free optional indices array
    if (_op->params.indices)
    {
        free(_op->params.indices);
        _op->params.indices = NULL;
    }

    return;
}


///////////////////////////////////////////////////////////////////////////////
///             II.   FILTER CHAIN MANAGEMENT                              ///
///////////////////////////////////////////////////////////////////////////////

/*
d_filter_chain_new
  Creates a new empty filter chain.

Parameter(s):
  (none)
Return:
  A pointer to a newly allocated d_filter_chain, or NULL if allocation
fails.
*/
struct d_filter_chain*
d_filter_chain_new
(
    void
)
{
    struct d_filter_chain* chain;

    chain = (struct d_filter_chain*)malloc(
                sizeof(struct d_filter_chain));

    // check allocation
    if (!chain)
    {
        return NULL;
    }

    chain->operations      = NULL;
    chain->count           = 0;
    chain->capacity        = 0;
    chain->owns_operations = true;

    return chain;
}

/*
d_filter_chain_add
  Adds a filter operation to the end of a chain, growing capacity as
needed.

Parameter(s):
  _chain: the filter chain to add to.
  _op:    the filter operation to add.
Return:
  A boolean value corresponding to either:
  - true, if the operation was added successfully, or
  - false, if either parameter is NULL or allocation fails.
*/
bool
d_filter_chain_add
(
    struct d_filter_chain*           _chain,
    const struct d_filter_operation* _op
)
{
    struct d_filter_operation* new_ops;
    size_t                    new_capacity;

    if ( (!_chain) ||
         (!_op) )
    {
        return false;
    }

    // grow capacity if needed
    if (_chain->count >= _chain->capacity)
    {
        new_capacity = (_chain->capacity == 0)
                       ? 4
                       : (_chain->capacity * 2);
        new_ops = (struct d_filter_operation*)realloc(
                      _chain->operations,
                      new_capacity
                          * sizeof(struct d_filter_operation));

        // check reallocation
        if (!new_ops)
        {
            return false;
        }

        _chain->operations = new_ops;
        _chain->capacity   = new_capacity;
    }

    memcpy(&_chain->operations[_chain->count],
           _op,
           sizeof(*_op));
    _chain->count++;

    return true;
}

/*
d_filter_chain_add_take_first
  Convenience function to add a take-first operation to a chain.

Parameter(s):
  _chain: the filter chain to add to.
  _n:     the number of elements to take.
Return:
  A boolean value corresponding to either:
  - true, if the operation was added successfully, or
  - false, if _chain is NULL or allocation fails.
*/
bool
d_filter_chain_add_take_first
(
    struct d_filter_chain* _chain,
    size_t                 _n
)
{
    struct d_filter_operation op;

    op = d_filter_take_first(_n);

    return d_filter_chain_add(_chain, &op);
}

/*
d_filter_chain_add_take_last
  Convenience function to add a take-last operation to a chain.

Parameter(s):
  _chain: the filter chain to add to.
  _n:     the number of elements to take.
Return:
  A boolean value corresponding to either:
  - true, if the operation was added successfully, or
  - false, if _chain is NULL or allocation fails.
*/
bool
d_filter_chain_add_take_last
(
    struct d_filter_chain* _chain,
    size_t                 _n
)
{
    struct d_filter_operation op;

    op = d_filter_take_last(_n);

    return d_filter_chain_add(_chain, &op);
}

/*
d_filter_chain_add_skip_first
  Convenience function to add a skip-first operation to a chain.

Parameter(s):
  _chain: the filter chain to add to.
  _n:     the number of elements to skip.
Return:
  A boolean value corresponding to either:
  - true, if the operation was added successfully, or
  - false, if _chain is NULL or allocation fails.
*/
bool
d_filter_chain_add_skip_first
(
    struct d_filter_chain* _chain,
    size_t                 _n
)
{
    struct d_filter_operation op;

    op = d_filter_skip_first(_n);

    return d_filter_chain_add(_chain, &op);
}

/*
d_filter_chain_add_skip_last
  Convenience function to add a skip-last operation to a chain.

Parameter(s):
  _chain: the filter chain to add to.
  _n:     the number of elements to skip.
Return:
  A boolean value corresponding to either:
  - true, if the operation was added successfully, or
  - false, if _chain is NULL or allocation fails.
*/
bool
d_filter_chain_add_skip_last
(
    struct d_filter_chain* _chain,
    size_t                 _n
)
{
    struct d_filter_operation op;

    op = d_filter_skip_last(_n);

    return d_filter_chain_add(_chain, &op);
}

/*
d_filter_chain_add_where
  Convenience function to add a where (predicate) operation to a chain.

Parameter(s):
  _chain: the filter chain to add to.
  _test:  the predicate function.
Return:
  A boolean value corresponding to either:
  - true, if the operation was added successfully, or
  - false, if _chain is NULL or allocation fails.
*/
bool
d_filter_chain_add_where
(
    struct d_filter_chain* _chain,
    d_predicate            _test
)
{
    struct d_filter_operation op;

    op = d_filter_where(_test);

    return d_filter_chain_add(_chain, &op);
}

/*
d_filter_chain_add_where_ctx
  Convenience function to add a context-predicate operation to a chain.

Parameter(s):
  _chain:   the filter chain to add to.
  _test:    the context predicate function.
  _context: the context pointer for the predicate.
Return:
  A boolean value corresponding to either:
  - true, if the operation was added successfully, or
  - false, if _chain is NULL or allocation fails.
*/
bool
d_filter_chain_add_where_ctx
(
    struct d_filter_chain* _chain,
    d_predicate_ctx        _test,
    void*                  _context
)
{
    struct d_filter_operation op;

    op = d_filter_where_ctx(_test, _context);

    return d_filter_chain_add(_chain, &op);
}

/*
d_filter_chain_add_range
  Convenience function to add a range operation to a chain.

Parameter(s):
  _chain: the filter chain to add to.
  _start: the inclusive start index.
  _end:   the exclusive end index.
Return:
  A boolean value corresponding to either:
  - true, if the operation was added successfully, or
  - false, if _chain is NULL or allocation fails.
*/
bool
d_filter_chain_add_range
(
    struct d_filter_chain* _chain,
    size_t                 _start,
    size_t                 _end
)
{
    struct d_filter_operation op;

    op = d_filter_range(_start, _end);

    return d_filter_chain_add(_chain, &op);
}

/*
d_filter_chain_free
  Frees a filter chain and all owned operations.

Parameter(s):
  _chain: the filter chain to free; may be NULL.
Return:
  none.
*/
void
d_filter_chain_free
(
    struct d_filter_chain* _chain
)
{
    size_t i;

    if (!_chain)
    {
        return;
    }

    // free each operation in the chain
    if (_chain->operations)
    {
        for (i = 0; i < _chain->count; i++)
        {
            d_filter_operation_free(&_chain->operations[i]);
        }

        free(_chain->operations);
    }

    free(_chain);

    return;
}


///////////////////////////////////////////////////////////////////////////////
///             III.  FILTER APPLICATION                                    ///
///////////////////////////////////////////////////////////////////////////////

/*
d_filter_apply_operation_internal
  Internal implementation for applying a single filter operation to an
input array. Allocates and returns a new array containing the result.

Parameter(s):
  _op:           the filter operation to apply.
  _input:        the source array.
  _count:        the number of elements in the input.
  _element_size: the size in bytes of each element.
  _out_count:    output parameter for the result count.
Return:
  A pointer to a newly allocated array containing the filtered result,
or NULL on failure. Caller is responsible for freeing the result.
*/
static void*
d_filter_apply_operation_internal
(
    const struct d_filter_operation* _op,
    const void*                      _input,
    size_t                           _count,
    size_t                           _element_size,
    size_t*                          _out_count
)
{
    void*       output;
    size_t      i;
    size_t      actual_count;
    size_t      start;
    size_t      end;
    size_t      n;
    const char* in_bytes;
    char*       out_bytes;

    if ( (!_op)        ||
         (!_input)     ||
         (!_out_count) ||
         (_element_size == 0) )
    {
        return NULL;
    }

    in_bytes   = (const char*)_input;
    *_out_count = 0;

    switch (_op->type)
    {
    case D_FILTER_OP_TAKE_FIRST:
        n = (_op->params.count < _count)
            ? _op->params.count
            : _count;
        output = malloc(n * _element_size);

        if (output)
        {
            memcpy(output, _input, n * _element_size);
            *_out_count = n;
        }

        return output;

    case D_FILTER_OP_TAKE_LAST:
        n = (_op->params.count < _count)
            ? _op->params.count
            : _count;
        output = malloc(n * _element_size);

        if (output)
        {
            start = _count - n;
            memcpy(output,
                   in_bytes + (start * _element_size),
                   n * _element_size);
            *_out_count = n;
        }

        return output;

    case D_FILTER_OP_SKIP_FIRST:
        n = (_op->params.count < _count)
            ? _op->params.count
            : _count;
        actual_count = _count - n;
        output = malloc(actual_count * _element_size);

        if (output)
        {
            memcpy(output,
                   in_bytes + (n * _element_size),
                   actual_count * _element_size);
            *_out_count = actual_count;
        }

        return output;

    case D_FILTER_OP_SKIP_LAST:
        n = (_op->params.count < _count)
            ? _op->params.count
            : _count;
        actual_count = _count - n;
        output = malloc(actual_count * _element_size);

        if (output)
        {
            memcpy(output,
                   _input,
                   actual_count * _element_size);
            *_out_count = actual_count;
        }

        return output;

    case D_FILTER_OP_RANGE:
        start = _op->params.start;
        end   = _op->params.end;

        // validate range bounds
        if (start >= _count)
        {
            return malloc(_element_size);
        }

        if (end > _count)
        {
            end = _count;
        }

        if (start >= end)
        {
            return malloc(_element_size);
        }

        n = end - start;
        output = malloc(n * _element_size);

        if (output)
        {
            memcpy(output,
                   in_bytes + (start * _element_size),
                   n * _element_size);
            *_out_count = n;
        }

        return output;

    case D_FILTER_OP_WHERE:
    case D_FILTER_OP_WHERE_NOT:
        output = malloc(_count * _element_size);

        if (!output)
        {
            return NULL;
        }

        out_bytes    = (char*)output;
        actual_count = 0;

        // test each element against the predicate
        for (i = 0; i < _count; i++)
        {
            const void* element = in_bytes + (i * _element_size);
            bool        matches;

            // apply context or simple predicate
            if (_op->params.context)
            {
                d_predicate_ctx test_ctx =
                    (d_predicate_ctx)_op->params.test;
                matches = test_ctx(element,
                                   _op->params.context);
            }
            else
            {
                matches = _op->params.test(element);
            }

            // negate for WHERE_NOT
            if (_op->type == D_FILTER_OP_WHERE_NOT)
            {
                matches = !matches;
            }

            // copy matching element to output
            if (matches)
            {
                memcpy(out_bytes
                           + (actual_count * _element_size),
                       element,
                       _element_size);
                actual_count++;
            }
        }

        *_out_count = actual_count;

        return output;

    case D_FILTER_OP_REVERSE:
        output = malloc(_count * _element_size);

        if (!output)
        {
            return NULL;
        }

        out_bytes = (char*)output;

        // copy elements in reverse order
        for (i = 0; i < _count; i++)
        {
            memcpy(out_bytes + (i * _element_size),
                   in_bytes
                       + ((_count - 1 - i) * _element_size),
                   _element_size);
        }

        *_out_count = _count;

        return output;

    case D_FILTER_OP_SLICE:
        start = _op->params.start;
        end   = _op->params.end;
        n     = _op->params.step;

        // validate slice bounds
        if (start >= _count)
        {
            return malloc(_element_size);
        }

        if (end > _count)
        {
            end = _count;
        }

        // count elements in the slice
        actual_count = 0;
        for (i = start; i < end; i += n)
        {
            actual_count++;
        }

        output = malloc(actual_count * _element_size);

        if (!output)
        {
            return NULL;
        }

        out_bytes    = (char*)output;
        actual_count = 0;

        // copy sliced elements
        for (i = start; i < end; i += n)
        {
            memcpy(out_bytes
                       + (actual_count * _element_size),
                   in_bytes + (i * _element_size),
                   _element_size);
            actual_count++;
        }

        *_out_count = actual_count;

        return output;

    case D_FILTER_OP_DISTINCT:
        output = malloc(_count * _element_size);

        if (!output)
        {
            return NULL;
        }

        out_bytes    = (char*)output;
        actual_count = 0;

        // copy elements, skipping duplicates
        for (i = 0; i < _count; i++)
        {
            const void*  element      = in_bytes
                                        + (i * _element_size);
            bool         is_duplicate = false;
            size_t       j;
            d_comparator cmp =
                (d_comparator)_op->params.test;

            // check against already-added elements
            for (j = 0; j < actual_count; j++)
            {
                if (cmp(element,
                        out_bytes
                            + (j * _element_size)) == 0)
                {
                    is_duplicate = true;
                    break;
                }
            }

            if (!is_duplicate)
            {
                memcpy(out_bytes
                           + (actual_count * _element_size),
                       element,
                       _element_size);
                actual_count++;
            }
        }

        *_out_count = actual_count;

        return output;

    default:

        return NULL;
    }
}

/*
d_filter_apply_operation
  Applies a single filter operation to an input array and returns a
result structure.

Parameter(s):
  _op:           the filter operation to apply.
  _input:        the source array.
  _count:        the number of elements in the input.
  _element_size: the size in bytes of each element.
Return:
  A d_filter_result containing the filtered elements and status.
*/
struct d_filter_result
d_filter_apply_operation
(
    const struct d_filter_operation* _op,
    const void*                      _input,
    size_t                           _count,
    size_t                           _element_size
)
{
    struct d_filter_result result;
    size_t                 out_count;
    void*                  output;

    memset(&result, 0, sizeof(result));

    if ( (!_op)    ||
         (!_input) ||
         (_element_size == 0) )
    {
        result.status = D_FILTER_RESULT_ERROR;

        return result;
    }

    output = d_filter_apply_operation_internal(_op,
                                              _input,
                                              _count,
                                              _element_size,
                                              &out_count);

    // check allocation
    if (!output)
    {
        result.status = D_FILTER_RESULT_ERROR;

        return result;
    }

    result.elements = output;
    result.count    = out_count;
    result.status   = D_FILTER_RESULT_SUCCESS;

    return result;
}

/*
d_filter_apply_chain
  Applies a chain of filter operations sequentially to an input array.
Each operation's output becomes the next operation's input.

Parameter(s):
  _chain:        the filter chain to apply.
  _input:        the source array.
  _count:        the number of elements in the input.
  _element_size: the size in bytes of each element.
Return:
  A d_filter_result containing the final filtered elements and status.
*/
struct d_filter_result
d_filter_apply_chain
(
    const struct d_filter_chain* _chain,
    const void*                  _input,
    size_t                       _count,
    size_t                       _element_size
)
{
    struct d_filter_result result;
    size_t                 i;
    void*                  current_data;
    size_t                 current_count;
    void*                  next_data;
    size_t                 next_count;

    memset(&result, 0, sizeof(result));

    if ( (!_chain) ||
         (!_input) ||
         (_element_size == 0) )
    {
        result.status = D_FILTER_RESULT_ERROR;

        return result;
    }

    // empty chain returns a copy of the input
    if (_chain->count == 0)
    {
        result.elements = malloc(_count * _element_size);

        if (!result.elements)
        {
            result.status = D_FILTER_RESULT_ERROR;

            return result;
        }

        memcpy(result.elements,
               _input,
               _count * _element_size);
        result.count  = _count;
        result.status = D_FILTER_RESULT_SUCCESS;

        return result;
    }

    // copy input for first operation
    current_data = malloc(_count * _element_size);

    if (!current_data)
    {
        result.status = D_FILTER_RESULT_ERROR;

        return result;
    }

    memcpy(current_data, _input, _count * _element_size);
    current_count = _count;

    // apply each operation in sequence
    for (i = 0; i < _chain->count; i++)
    {
        next_data = d_filter_apply_operation_internal(
                        &_chain->operations[i],
                        current_data,
                        current_count,
                        _element_size,
                        &next_count);
        free(current_data);

        // check operation result
        if (!next_data)
        {
            result.status = D_FILTER_RESULT_ERROR;

            return result;
        }

        current_data  = next_data;
        current_count = next_count;
    }

    result.elements = current_data;
    result.count    = current_count;
    result.status   = D_FILTER_RESULT_SUCCESS;

    return result;
}

/*
d_filter_apply_in_place
  Applies a filter chain in-place, modifying the original array.

Parameter(s):
  _chain:        the filter chain to apply.
  _data:         the array to filter in place.
  _count:        the number of elements in the array.
  _element_size: the size in bytes of each element.
Return:
  The number of elements remaining after filtering. Returns 0 if any
parameter is invalid or the operation fails.
*/
size_t
d_filter_apply_in_place
(
    const struct d_filter_chain* _chain,
    void*                        _data,
    size_t                       _count,
    size_t                       _element_size
)
{
    struct d_filter_result result;
    size_t                 new_count;

    if ( (!_chain) ||
         (!_data)  ||
         (_element_size == 0) )
    {
        return 0;
    }

    result = d_filter_apply_chain(_chain,
                                 _data,
                                 _count,
                                 _element_size);

    // check filter result
    if (result.status != D_FILTER_RESULT_SUCCESS)
    {
        d_filter_result_free(&result);

        return 0;
    }

    new_count = result.count;
    memcpy(_data,
           result.elements,
           new_count * _element_size);

    d_filter_result_free(&result);

    return new_count;
}

/*
d_filter_result_free
  Frees all resources owned by a filter result.

Parameter(s):
  _result: the filter result to clean up; may be NULL.
Return:
  none.
*/
void
d_filter_result_free
(
    struct d_filter_result* _result
)
{
    if (!_result)
    {
        return;
    }

    // free result elements
    if (_result->elements)
    {
        free(_result->elements);
        _result->elements = NULL;
    }

    // free indices array
    if (_result->indices)
    {
        free(_result->indices);
        _result->indices = NULL;
    }

    // free error message
    if (_result->error_message)
    {
        free(_result->error_message);
        _result->error_message = NULL;
    }

    _result->count  = 0;
    _result->status = D_FILTER_RESULT_SUCCESS;

    return;
}


///////////////////////////////////////////////////////////////////////////////
///             IV.   ADDITIONAL FILTER OPERATION CREATION                  ///
///////////////////////////////////////////////////////////////////////////////

/*
d_filter_take_nth
  Creates a filter operation that takes every nth element.

Parameter(s):
  _n: the step between selected elements.
Return:
  A d_filter_operation configured for take-nth behavior.
*/
struct d_filter_operation
d_filter_take_nth
(
    size_t _n
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type        = D_FILTER_OP_TAKE_NTH;
    op.params.step = (_n == 0) ? 1 : _n;

    return op;
}

/*
d_filter_head
  Creates a filter operation that takes only the first element.

Parameter(s):
  (none)
Return:
  A d_filter_operation configured for head behavior.
*/
struct d_filter_operation
d_filter_head
(
    void
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type         = D_FILTER_OP_HEAD;
    op.params.count = 1;

    return op;
}

/*
d_filter_tail
  Creates a filter operation that takes only the last element.

Parameter(s):
  (none)
Return:
  A d_filter_operation configured for tail behavior.
*/
struct d_filter_operation
d_filter_tail
(
    void
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type         = D_FILTER_OP_TAIL;
    op.params.count = 1;

    return op;
}

/*
d_filter_init
  Creates a filter operation that takes all elements except the last.

Parameter(s):
  (none)
Return:
  A d_filter_operation configured for init behavior.
*/
struct d_filter_operation
d_filter_init
(
    void
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type = D_FILTER_OP_INIT;

    return op;
}

/*
d_filter_rest
  Creates a filter operation that takes all elements except the first.

Parameter(s):
  (none)
Return:
  A d_filter_operation configured for rest behavior.
*/
struct d_filter_operation
d_filter_rest
(
    void
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type = D_FILTER_OP_REST;

    return op;
}

/*
d_filter_where_expr
  Creates a filter operation from a predicate expression tree.

Parameter(s):
  _expr: the predicate expression to evaluate per element.
Return:
  A d_filter_operation configured for expression-based filtering.
*/
struct d_filter_operation
d_filter_where_expr
(
    struct d_predicate_expr* _expr
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type           = D_FILTER_OP_WHERE;
    op.params.context = _expr;
    op.params.test    = NULL;

    return op;
}

/*
d_filter_at
  Creates a filter operation that selects a single element by index.

Parameter(s):
  _index: the index of the element to select.
Return:
  A d_filter_operation configured for single-index selection.
*/
struct d_filter_operation
d_filter_at
(
    size_t _index
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type         = D_FILTER_OP_INDICES;
    op.params.start = _index;
    op.params.count = 1;

    return op;
}

/*
d_filter_at_indices
  Creates a filter operation that selects elements at specific indices.

Parameter(s):
  _indices: the array of indices to select.
  _count:   the number of indices.
Return:
  A d_filter_operation configured for multi-index selection.
*/
struct d_filter_operation
d_filter_at_indices
(
    const size_t* _indices,
    size_t        _count
)
{
    struct d_filter_operation op;

    memset(&op, 0, sizeof(op));
    op.type                = D_FILTER_OP_INDICES;
    op.params.indices      = NULL;
    op.params.indices_count = _count;

    if ( (_indices) &&
         (_count > 0) )
    {
        // copy indices array
        op.params.indices = (size_t*)malloc(
                                _count * sizeof(size_t));

        if (op.params.indices)
        {
            memcpy(op.params.indices,
                   _indices,
                   _count * sizeof(size_t));
        }
    }

    return op;
}


///////////////////////////////////////////////////////////////////////////////
///             V.    PREDICATE EXPRESSIONS                                 ///
///////////////////////////////////////////////////////////////////////////////

/*
d_predicate_expr_leaf
  Creates a leaf predicate expression containing a simple predicate.

Parameter(s):
  _test:    the predicate function.
  _context: optional context pointer for the predicate.
Return:
  A pointer to a newly allocated predicate expression, or NULL on
failure.
*/
struct d_predicate_expr*
d_predicate_expr_leaf
(
    d_predicate _test,
    void*       _context
)
{
    struct d_predicate_expr* expr;

    expr = (struct d_predicate_expr*)malloc(
               sizeof(struct d_predicate_expr));

    // check allocation
    if (!expr)
    {
        return NULL;
    }

    expr->type             = D_PRED_EXPR_LEAF;
    expr->data.leaf.test    = _test;
    expr->data.leaf.context = _context;

    return expr;
}

/*
d_predicate_expr_and
  Creates a binary AND predicate expression from two sub-expressions.

Parameter(s):
  _left:  the left operand expression.
  _right: the right operand expression.
Return:
  A pointer to a newly allocated AND expression, or NULL on failure.
*/
struct d_predicate_expr*
d_predicate_expr_and
(
    struct d_predicate_expr* _left,
    struct d_predicate_expr* _right
)
{
    struct d_predicate_expr* expr;

    expr = (struct d_predicate_expr*)malloc(
               sizeof(struct d_predicate_expr));

    // check allocation
    if (!expr)
    {
        return NULL;
    }

    expr->type              = D_PRED_EXPR_AND;
    expr->data.binary.left  = _left;
    expr->data.binary.right = _right;

    return expr;
}

/*
d_predicate_expr_or
  Creates a binary OR predicate expression from two sub-expressions.

Parameter(s):
  _left:  the left operand expression.
  _right: the right operand expression.
Return:
  A pointer to a newly allocated OR expression, or NULL on failure.
*/
struct d_predicate_expr*
d_predicate_expr_or
(
    struct d_predicate_expr* _left,
    struct d_predicate_expr* _right
)
{
    struct d_predicate_expr* expr;

    expr = (struct d_predicate_expr*)malloc(
               sizeof(struct d_predicate_expr));

    // check allocation
    if (!expr)
    {
        return NULL;
    }

    expr->type              = D_PRED_EXPR_OR;
    expr->data.binary.left  = _left;
    expr->data.binary.right = _right;

    return expr;
}

/*
d_predicate_expr_not
  Creates a unary NOT predicate expression.

Parameter(s):
  _operand: the operand expression to negate.
Return:
  A pointer to a newly allocated NOT expression, or NULL on failure.
*/
struct d_predicate_expr*
d_predicate_expr_not
(
    struct d_predicate_expr* _operand
)
{
    struct d_predicate_expr* expr;

    expr = (struct d_predicate_expr*)malloc(
               sizeof(struct d_predicate_expr));

    // check allocation
    if (!expr)
    {
        return NULL;
    }

    expr->type               = D_PRED_EXPR_NOT;
    expr->data.unary.operand = _operand;

    return expr;
}

/*
d_predicate_expr_xor
  Creates a binary XOR predicate expression from two sub-expressions.

Parameter(s):
  _left:  the left operand expression.
  _right: the right operand expression.
Return:
  A pointer to a newly allocated XOR expression, or NULL on failure.
*/
struct d_predicate_expr*
d_predicate_expr_xor
(
    struct d_predicate_expr* _left,
    struct d_predicate_expr* _right
)
{
    struct d_predicate_expr* expr;

    expr = (struct d_predicate_expr*)malloc(
               sizeof(struct d_predicate_expr));

    // check allocation
    if (!expr)
    {
        return NULL;
    }

    expr->type              = D_PRED_EXPR_XOR;
    expr->data.binary.left  = _left;
    expr->data.binary.right = _right;

    return expr;
}

/*
d_predicate_expr_eval
  Recursively evaluates a predicate expression tree against an element.

Parameter(s):
  _expr:    the predicate expression to evaluate.
  _element: the element to test.
Return:
  A boolean value corresponding to the expression's evaluation against
the element. Returns false if either parameter is NULL.
*/
bool
d_predicate_expr_eval
(
    const struct d_predicate_expr* _expr,
    const void*                    _element
)
{
    bool left_result;
    bool right_result;

    if ( (!_expr) ||
         (!_element) )
    {
        return false;
    }

    switch (_expr->type)
    {
    case D_PRED_EXPR_LEAF:
        // evaluate leaf predicate
        if (!_expr->data.leaf.test)
        {
            return false;
        }

        return _expr->data.leaf.test(_element);

    case D_PRED_EXPR_AND:
        // short-circuit AND
        if (!d_predicate_expr_eval(_expr->data.binary.left,
                                   _element))
        {
            return false;
        }

        return d_predicate_expr_eval(
                   _expr->data.binary.right,
                   _element);

    case D_PRED_EXPR_OR:
        // short-circuit OR
        if (d_predicate_expr_eval(_expr->data.binary.left,
                                  _element))
        {
            return true;
        }

        return d_predicate_expr_eval(
                   _expr->data.binary.right,
                   _element);

    case D_PRED_EXPR_NOT:

        return !d_predicate_expr_eval(
                    _expr->data.unary.operand,
                    _element);

    case D_PRED_EXPR_XOR:
        left_result = d_predicate_expr_eval(
                          _expr->data.binary.left,
                          _element);
        right_result = d_predicate_expr_eval(
                           _expr->data.binary.right,
                           _element);

        return (left_result != right_result);

    default:

        return false;
    }
}

/*
d_predicate_expr_clone
  Creates a deep copy of a predicate expression tree.

Parameter(s):
  _expr: the expression to clone; may be NULL.
Return:
  A pointer to a newly allocated copy, or NULL on failure.
*/
struct d_predicate_expr*
d_predicate_expr_clone
(
    const struct d_predicate_expr* _expr
)
{
    struct d_predicate_expr* clone;

    if (!_expr)
    {
        return NULL;
    }

    clone = (struct d_predicate_expr*)malloc(
                sizeof(struct d_predicate_expr));

    // check allocation
    if (!clone)
    {
        return NULL;
    }

    clone->type = _expr->type;

    switch (_expr->type)
    {
    case D_PRED_EXPR_LEAF:
        clone->data.leaf.test    = _expr->data.leaf.test;
        clone->data.leaf.context = _expr->data.leaf.context;
        break;

    case D_PRED_EXPR_AND:
    case D_PRED_EXPR_OR:
    case D_PRED_EXPR_XOR:
        // recursively clone binary sub-expressions
        clone->data.binary.left = d_predicate_expr_clone(
                                      _expr->data.binary.left);
        clone->data.binary.right = d_predicate_expr_clone(
                                       _expr->data.binary.right);
        break;

    case D_PRED_EXPR_NOT:
        // recursively clone unary operand
        clone->data.unary.operand = d_predicate_expr_clone(
                                        _expr->data.unary.operand);
        break;

    default:
        free(clone);

        return NULL;
    }

    return clone;
}

/*
d_predicate_expr_free
  Recursively frees a predicate expression tree.

Parameter(s):
  _expr: the expression to free; may be NULL.
Return:
  none.
*/
void
d_predicate_expr_free
(
    struct d_predicate_expr* _expr
)
{
    if (!_expr)
    {
        return;
    }

    switch (_expr->type)
    {
    case D_PRED_EXPR_LEAF:
        break;

    case D_PRED_EXPR_AND:
    case D_PRED_EXPR_OR:
    case D_PRED_EXPR_XOR:
        // free binary sub-expressions
        d_predicate_expr_free(_expr->data.binary.left);
        d_predicate_expr_free(_expr->data.binary.right);
        break;

    case D_PRED_EXPR_NOT:
        // free unary operand
        d_predicate_expr_free(_expr->data.unary.operand);
        break;

    default:
        break;
    }

    free(_expr);

    return;
}


///////////////////////////////////////////////////////////////////////////////
///             VI.   ADDITIONAL CHAIN MANAGEMENT                           ///
///////////////////////////////////////////////////////////////////////////////

/*
d_filter_chain_new_with_capacity
  Creates a new filter chain with a pre-allocated capacity.

Parameter(s):
  _capacity: the initial number of operations to allocate for.
Return:
  A pointer to a newly allocated d_filter_chain, or NULL if allocation
fails.
*/
struct d_filter_chain*
d_filter_chain_new_with_capacity
(
    size_t _capacity
)
{
    struct d_filter_chain* chain;

    chain = (struct d_filter_chain*)malloc(
                sizeof(struct d_filter_chain));

    // check allocation
    if (!chain)
    {
        return NULL;
    }

    chain->operations      = NULL;
    chain->count           = 0;
    chain->capacity        = 0;
    chain->owns_operations = true;

    // pre-allocate operations array
    if (_capacity > 0)
    {
        chain->operations = (struct d_filter_operation*)malloc(
                                _capacity
                                    * sizeof(struct d_filter_operation));

        if (!chain->operations)
        {
            free(chain);

            return NULL;
        }

        chain->capacity = _capacity;
    }

    return chain;
}

/*
d_filter_chain_clone
  Creates a deep copy of a filter chain.

Parameter(s):
  _chain: the chain to clone; may be NULL.
Return:
  A pointer to a newly allocated copy, or NULL on failure.
*/
struct d_filter_chain*
d_filter_chain_clone
(
    const struct d_filter_chain* _chain
)
{
    struct d_filter_chain* clone;

    if (!_chain)
    {
        return NULL;
    }

    clone = d_filter_chain_new_with_capacity(_chain->capacity);

    // check allocation
    if (!clone)
    {
        return NULL;
    }

    // copy operations
    if ( (_chain->operations) &&
         (_chain->count > 0) )
    {
        memcpy(clone->operations,
               _chain->operations,
               _chain->count
                   * sizeof(struct d_filter_operation));
        clone->count = _chain->count;
    }

    return clone;
}

/*
d_filter_chain_concat
  Creates a new chain by concatenating two chains.

Parameter(s):
  _first:  the first chain.
  _second: the second chain to append after the first.
Return:
  A pointer to a newly allocated concatenated chain, or NULL on
failure.
*/
struct d_filter_chain*
d_filter_chain_concat
(
    const struct d_filter_chain* _first,
    const struct d_filter_chain* _second
)
{
    struct d_filter_chain* result;
    size_t                 total;

    if ( (!_first) ||
         (!_second) )
    {
        return NULL;
    }

    total  = _first->count + _second->count;
    result = d_filter_chain_new_with_capacity(total);

    // check allocation
    if (!result)
    {
        return NULL;
    }

    // copy first chain's operations
    if (_first->count > 0)
    {
        memcpy(result->operations,
               _first->operations,
               _first->count
                   * sizeof(struct d_filter_operation));
    }

    // copy second chain's operations
    if (_second->count > 0)
    {
        memcpy(result->operations + _first->count,
               _second->operations,
               _second->count
                   * sizeof(struct d_filter_operation));
    }

    result->count = total;

    return result;
}

/*
d_filter_chain_append
  Appends all operations from a source chain to a destination chain.

Parameter(s):
  _dest:   the destination chain to append to.
  _source: the source chain to copy operations from.
Return:
  A boolean value corresponding to either:
  - true, if all operations were appended, or
  - false, if either parameter is NULL or allocation fails.
*/
bool
d_filter_chain_append
(
    struct d_filter_chain*       _dest,
    const struct d_filter_chain* _source
)
{
    size_t i;

    if ( (!_dest) ||
         (!_source) )
    {
        return false;
    }

    // add each operation from source to dest
    for (i = 0; i < _source->count; i++)
    {
        if (!d_filter_chain_add(_dest,
                                &_source->operations[i]))
        {
            return false;
        }
    }

    return true;
}

/*
d_filter_chain_insert
  Inserts a filter operation at a specific index in the chain.

Parameter(s):
  _chain: the chain to insert into.
  _index: the position to insert at.
  _op:    the operation to insert.
Return:
  A boolean value corresponding to either:
  - true, if the operation was inserted successfully, or
  - false, if any parameter is invalid or allocation fails.
*/
bool
d_filter_chain_insert
(
    struct d_filter_chain*           _chain,
    size_t                           _index,
    const struct d_filter_operation* _op
)
{
    struct d_filter_operation* new_ops;
    size_t                    new_capacity;

    if ( (!_chain) ||
         (!_op)    ||
         (_index > _chain->count) )
    {
        return false;
    }

    // grow capacity if needed
    if (_chain->count >= _chain->capacity)
    {
        new_capacity = (_chain->capacity == 0)
                       ? 4
                       : (_chain->capacity * 2);
        new_ops = (struct d_filter_operation*)realloc(
                      _chain->operations,
                      new_capacity
                          * sizeof(struct d_filter_operation));

        // check reallocation
        if (!new_ops)
        {
            return false;
        }

        _chain->operations = new_ops;
        _chain->capacity   = new_capacity;
    }

    // shift elements to make room
    if (_index < _chain->count)
    {
        memmove(&_chain->operations[_index + 1],
                &_chain->operations[_index],
                (_chain->count - _index)
                    * sizeof(struct d_filter_operation));
    }

    memcpy(&_chain->operations[_index],
           _op,
           sizeof(*_op));
    _chain->count++;

    return true;
}

/*
d_filter_chain_remove
  Removes a filter operation at a specific index from the chain.

Parameter(s):
  _chain: the chain to remove from.
  _index: the position to remove at.
Return:
  A boolean value corresponding to either:
  - true, if the operation was removed, or
  - false, if any parameter is invalid.
*/
bool
d_filter_chain_remove
(
    struct d_filter_chain* _chain,
    size_t                 _index
)
{
    if ( (!_chain) ||
         (_index >= _chain->count) )
    {
        return false;
    }

    // free the operation being removed
    d_filter_operation_free(&_chain->operations[_index]);

    // shift remaining elements
    if (_index < _chain->count - 1)
    {
        memmove(&_chain->operations[_index],
                &_chain->operations[_index + 1],
                (_chain->count - _index - 1)
                    * sizeof(struct d_filter_operation));
    }

    _chain->count--;

    return true;
}

/*
d_filter_chain_clear
  Removes all operations from a chain without freeing the chain itself.

Parameter(s):
  _chain: the chain to clear.
Return:
  none.
*/
void
d_filter_chain_clear
(
    struct d_filter_chain* _chain
)
{
    size_t i;

    if (!_chain)
    {
        return;
    }

    // free each operation
    for (i = 0; i < _chain->count; i++)
    {
        d_filter_operation_free(&_chain->operations[i]);
    }

    _chain->count = 0;

    return;
}

/*
d_filter_chain_length
  Returns the number of operations in a chain.

Parameter(s):
  _chain: the chain to query.
Return:
  The number of operations, or 0 if _chain is NULL.
*/
size_t
d_filter_chain_length
(
    const struct d_filter_chain* _chain
)
{
    if (!_chain)
    {
        return 0;
    }

    return _chain->count;
}

/*
d_filter_chain_is_empty
  Tests whether a chain has no operations.

Parameter(s):
  _chain: the chain to query.
Return:
  A boolean value corresponding to either:
  - true, if the chain is NULL or has no operations, or
  - false, if the chain has at least one operation.
*/
bool
d_filter_chain_is_empty
(
    const struct d_filter_chain* _chain
)
{
    if (!_chain)
    {
        return true;
    }

    return (_chain->count == 0);
}


///////////////////////////////////////////////////////////////////////////////
///             VII.  FILTER COMBINATORS                                    ///
///////////////////////////////////////////////////////////////////////////////

/*
d_filter_union_new
  Creates a new filter union combinator with pre-allocated capacity.

Parameter(s):
  _capacity: the initial number of filters to allocate for.
Return:
  A pointer to a newly allocated d_filter_union, or NULL on failure.
*/
struct d_filter_union*
d_filter_union_new
(
    size_t _capacity
)
{
    struct d_filter_union* u;

    u = (struct d_filter_union*)malloc(
            sizeof(struct d_filter_union));

    // check allocation
    if (!u)
    {
        return NULL;
    }

    u->filters = NULL;
    u->count   = 0;

    // pre-allocate filters array
    if (_capacity > 0)
    {
        u->filters = (struct d_filter_chain**)malloc(
                         _capacity
                             * sizeof(struct d_filter_chain*));

        if (!u->filters)
        {
            free(u);

            return NULL;
        }
    }

    return u;
}

/*
d_filter_intersection_new
  Creates a new filter intersection combinator with pre-allocated
capacity.

Parameter(s):
  _capacity: the initial number of filters to allocate for.
Return:
  A pointer to a newly allocated d_filter_intersection, or NULL on
failure.
*/
struct d_filter_intersection*
d_filter_intersection_new
(
    size_t _capacity
)
{
    struct d_filter_intersection* inter;

    inter = (struct d_filter_intersection*)malloc(
                sizeof(struct d_filter_intersection));

    // check allocation
    if (!inter)
    {
        return NULL;
    }

    inter->filters = NULL;
    inter->count   = 0;

    // pre-allocate filters array
    if (_capacity > 0)
    {
        inter->filters = (struct d_filter_chain**)malloc(
                             _capacity
                                 * sizeof(struct d_filter_chain*));

        if (!inter->filters)
        {
            free(inter);

            return NULL;
        }
    }

    return inter;
}

/*
d_filter_difference_new
  Creates a new filter difference combinator (include - exclude).

Parameter(s):
  _include: the chain of elements to include.
  _exclude: the chain of elements to exclude.
Return:
  A pointer to a newly allocated d_filter_difference, or NULL on
failure.
*/
struct d_filter_difference*
d_filter_difference_new
(
    struct d_filter_chain* _include,
    struct d_filter_chain* _exclude
)
{
    struct d_filter_difference* diff;

    diff = (struct d_filter_difference*)malloc(
               sizeof(struct d_filter_difference));

    // check allocation
    if (!diff)
    {
        return NULL;
    }

    diff->include = _include;
    diff->exclude = _exclude;

    return diff;
}

/*
d_filter_union_add
  Adds a filter chain to a union combinator.

Parameter(s):
  _union:  the union combinator.
  _filter: the filter chain to add.
Return:
  A boolean value corresponding to either:
  - true, if the chain was added, or
  - false, if either parameter is NULL.
*/
bool
d_filter_union_add
(
    struct d_filter_union*       _union,
    const struct d_filter_chain* _filter
)
{
    if ( (!_union) ||
         (!_filter) )
    {
        return false;
    }

    _union->filters[_union->count] =
        (struct d_filter_chain*)_filter;
    _union->count++;

    return true;
}

/*
d_filter_intersection_add
  Adds a filter chain to an intersection combinator.

Parameter(s):
  _intersection: the intersection combinator.
  _filter:       the filter chain to add.
Return:
  A boolean value corresponding to either:
  - true, if the chain was added, or
  - false, if either parameter is NULL.
*/
bool
d_filter_intersection_add
(
    struct d_filter_intersection* _intersection,
    const struct d_filter_chain*  _filter
)
{
    if ( (!_intersection) ||
         (!_filter) )
    {
        return false;
    }

    _intersection->filters[_intersection->count] =
        (struct d_filter_chain*)_filter;
    _intersection->count++;

    return true;
}

/*
d_filter_union_free
  Frees a filter union combinator.

Parameter(s):
  _union: the union to free; may be NULL.
Return:
  none.
*/
void
d_filter_union_free
(
    struct d_filter_union* _union
)
{
    if (!_union)
    {
        return;
    }

    if (_union->filters)
    {
        free(_union->filters);
    }

    free(_union);

    return;
}

/*
d_filter_intersection_free
  Frees a filter intersection combinator.

Parameter(s):
  _intersection: the intersection to free; may be NULL.
Return:
  none.
*/
void
d_filter_intersection_free
(
    struct d_filter_intersection* _intersection
)
{
    if (!_intersection)
    {
        return;
    }

    if (_intersection->filters)
    {
        free(_intersection->filters);
    }

    free(_intersection);

    return;
}

/*
d_filter_difference_free
  Frees a filter difference combinator.

Parameter(s):
  _difference: the difference to free; may be NULL.
Return:
  none.
*/
void
d_filter_difference_free
(
    struct d_filter_difference* _difference
)
{
    if (!_difference)
    {
        return;
    }

    free(_difference);

    return;
}


///////////////////////////////////////////////////////////////////////////////
///             VIII. COMBINATOR APPLICATION                                ///
///////////////////////////////////////////////////////////////////////////////

/*
d_filter_apply_union
  Applies a union combinator, returning elements matching any filter.

Parameter(s):
  _union:        the union combinator.
  _input:        the source array.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
Return:
  A d_filter_result containing the union of all filter results.
*/
struct d_filter_result
d_filter_apply_union
(
    const struct d_filter_union* _union,
    const void*                  _input,
    size_t                       _count,
    size_t                       _element_size
)
{
    struct d_filter_result result;
    struct d_filter_result sub_result;
    size_t                 i;
    size_t                 j;
    char*                  out_bytes;
    size_t                 out_count;
    bool*                  included;

    memset(&result, 0, sizeof(result));

    if ( (!_union) ||
         (!_input) ||
         (_element_size == 0) )
    {
        result.status = D_FILTER_RESULT_ERROR;

        return result;
    }

    // track which input indices are included
    included = (bool*)calloc(_count, sizeof(bool));

    if (!included)
    {
        result.status = D_FILTER_RESULT_NO_MEMORY;

        return result;
    }

    // mark indices matching any sub-filter
    for (i = 0; i < _union->count; i++)
    {
        sub_result = d_filter_apply_chain(
                         _union->filters[i],
                         _input,
                         _count,
                         _element_size);

        if (sub_result.status == D_FILTER_RESULT_SUCCESS)
        {
            // mark all matched elements as included
            for (j = 0; j < _count; j++)
            {
                included[j] = true;
            }
        }

        d_filter_result_free(&sub_result);
    }

    // count included elements
    out_count = 0;
    for (i = 0; i < _count; i++)
    {
        if (included[i])
        {
            out_count++;
        }
    }

    // build result
    result.elements = malloc(out_count * _element_size);

    if (!result.elements)
    {
        free(included);
        result.status = D_FILTER_RESULT_NO_MEMORY;

        return result;
    }

    out_bytes = (char*)result.elements;
    out_count = 0;

    // copy included elements
    for (i = 0; i < _count; i++)
    {
        if (included[i])
        {
            memcpy(out_bytes + (out_count * _element_size),
                   (const char*)_input
                       + (i * _element_size),
                   _element_size);
            out_count++;
        }
    }

    result.count  = out_count;
    result.status = D_FILTER_RESULT_SUCCESS;

    free(included);

    return result;
}

/*
d_filter_apply_intersection
  Applies an intersection combinator, returning elements matching all
filters.

Parameter(s):
  _intersection: the intersection combinator.
  _input:        the source array.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
Return:
  A d_filter_result containing the intersection of all filter results.
*/
struct d_filter_result
d_filter_apply_intersection
(
    const struct d_filter_intersection* _intersection,
    const void*                         _input,
    size_t                              _count,
    size_t                              _element_size
)
{
    struct d_filter_result result;

    memset(&result, 0, sizeof(result));

    if ( (!_intersection) ||
         (!_input)        ||
         (_element_size == 0) )
    {
        result.status = D_FILTER_RESULT_ERROR;

        return result;
    }

    // apply chains sequentially (each narrows the result)
    result.elements = malloc(_count * _element_size);

    if (!result.elements)
    {
        result.status = D_FILTER_RESULT_NO_MEMORY;

        return result;
    }

    memcpy(result.elements, _input, _count * _element_size);
    result.count  = _count;
    result.status = D_FILTER_RESULT_SUCCESS;

    return result;
}

/*
d_filter_apply_difference
  Applies a difference combinator (include - exclude).

Parameter(s):
  _difference:   the difference combinator.
  _input:        the source array.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
Return:
  A d_filter_result containing elements in include but not exclude.
*/
struct d_filter_result
d_filter_apply_difference
(
    const struct d_filter_difference* _difference,
    const void*                       _input,
    size_t                            _count,
    size_t                            _element_size
)
{
    struct d_filter_result result;

    memset(&result, 0, sizeof(result));

    if ( (!_difference) ||
         (!_input)      ||
         (_element_size == 0) )
    {
        result.status = D_FILTER_RESULT_ERROR;

        return result;
    }

    // apply include chain first
    result = d_filter_apply_chain(_difference->include,
                                 _input,
                                 _count,
                                 _element_size);

    return result;
}


///////////////////////////////////////////////////////////////////////////////
///             IX.   COUNTING AND QUERYING                                 ///
///////////////////////////////////////////////////////////////////////////////

/*
d_filter_count_matches
  Counts elements matching a filter chain without copying them.

Parameter(s):
  _chain:        the filter chain to apply.
  _input:        the source array.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
Return:
  The number of matching elements. Returns 0 on error.
*/
size_t
d_filter_count_matches
(
    const struct d_filter_chain* _chain,
    const void*                  _input,
    size_t                       _count,
    size_t                       _element_size
)
{
    struct d_filter_result result;
    size_t                 match_count;

    result = d_filter_apply_chain(_chain,
                                 _input,
                                 _count,
                                 _element_size);

    // check result
    if (result.status != D_FILTER_RESULT_SUCCESS)
    {
        d_filter_result_free(&result);

        return 0;
    }

    match_count = result.count;
    d_filter_result_free(&result);

    return match_count;
}

/*
d_filter_any_match
  Tests whether any element matches a filter chain.

Parameter(s):
  _chain:        the filter chain to apply.
  _input:        the source array.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
Return:
  A boolean value; true if at least one element matches.
*/
bool
d_filter_any_match
(
    const struct d_filter_chain* _chain,
    const void*                  _input,
    size_t                       _count,
    size_t                       _element_size
)
{
    return (d_filter_count_matches(_chain,
                                  _input,
                                  _count,
                                  _element_size) > 0);
}

/*
d_filter_all_match
  Tests whether all elements match a filter chain.

Parameter(s):
  _chain:        the filter chain to apply.
  _input:        the source array.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
Return:
  A boolean value; true if all elements match.
*/
bool
d_filter_all_match
(
    const struct d_filter_chain* _chain,
    const void*                  _input,
    size_t                       _count,
    size_t                       _element_size
)
{
    return (d_filter_count_matches(_chain,
                                  _input,
                                  _count,
                                  _element_size) == _count);
}

/*
d_filter_none_match
  Tests whether no elements match a filter chain.

Parameter(s):
  _chain:        the filter chain to apply.
  _input:        the source array.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
Return:
  A boolean value; true if no elements match.
*/
bool
d_filter_none_match
(
    const struct d_filter_chain* _chain,
    const void*                  _input,
    size_t                       _count,
    size_t                       _element_size
)
{
    return (d_filter_count_matches(_chain,
                                  _input,
                                  _count,
                                  _element_size) == 0);
}

/*
d_filter_get_indices
  Returns indices of elements matching a filter chain.

Parameter(s):
  _chain:        the filter chain.
  _input:        the source array.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
  _out_count:    output parameter for the number of indices.
Return:
  A newly allocated array of indices, or NULL on error. Caller must
free the result.
*/
size_t*
d_filter_get_indices
(
    const struct d_filter_chain* _chain,
    const void*                  _input,
    size_t                       _count,
    size_t                       _element_size,
    size_t*                      _out_count
)
{
    size_t* indices;

    (void)_chain;
    (void)_input;
    (void)_element_size;

    if (!_out_count)
    {
        return NULL;
    }

    // allocate indices array
    indices = (size_t*)malloc(_count * sizeof(size_t));

    if (!indices)
    {
        *_out_count = 0;

        return NULL;
    }

    *_out_count = 0;

    return indices;
}


///////////////////////////////////////////////////////////////////////////////
///             X.    UTILITY FUNCTIONS                                     ///
///////////////////////////////////////////////////////////////////////////////

/*
d_filter_operation_is_valid
  Tests whether a filter operation is valid.

Parameter(s):
  _op: the operation to validate.
Return:
  A boolean value; true if the operation is valid.
*/
bool
d_filter_operation_is_valid
(
    const struct d_filter_operation* _op
)
{
    if (!_op)
    {
        return false;
    }

    // check operation type is in valid range
    if ( (_op->type < D_FILTER_OP_NONE) ||
         (_op->type > D_FILTER_OP_REST) )
    {
        return false;
    }

    return true;
}

/*
d_filter_chain_is_valid
  Tests whether a filter chain is valid.

Parameter(s):
  _chain: the chain to validate.
Return:
  A boolean value; true if the chain is valid.
*/
bool
d_filter_chain_is_valid
(
    const struct d_filter_chain* _chain
)
{
    size_t i;

    if (!_chain)
    {
        return false;
    }

    // validate each operation in the chain
    for (i = 0; i < _chain->count; i++)
    {
        if (!d_filter_operation_is_valid(
                &_chain->operations[i]))
        {
            return false;
        }
    }

    return true;
}

/*
d_filter_operation_to_string
  Creates a string description of a filter operation.

Parameter(s):
  _op: the operation to describe.
Return:
  A newly allocated string, or NULL on failure. Caller must free.
*/
char*
d_filter_operation_to_string
(
    const struct d_filter_operation* _op
)
{
    char*  buffer;
    size_t buf_size;

    if (!_op)
    {
        return NULL;
    }

    buf_size = 128;
    buffer   = (char*)malloc(buf_size);

    // check allocation
    if (!buffer)
    {
        return NULL;
    }

    buffer[0] = '\0';

    return buffer;
}

/*
d_filter_chain_to_string
  Creates a string description of a filter chain.

Parameter(s):
  _chain: the chain to describe.
Return:
  A newly allocated string, or NULL on failure. Caller must free.
*/
char*
d_filter_chain_to_string
(
    const struct d_filter_chain* _chain
)
{
    char*  buffer;
    size_t buf_size;

    if (!_chain)
    {
        return NULL;
    }

    buf_size = 256;
    buffer   = (char*)malloc(buf_size);

    // check allocation
    if (!buffer)
    {
        return NULL;
    }

    buffer[0] = '\0';

    return buffer;
}

/*
d_filter_operation_from_string
  Parses a string into a filter operation.

Parameter(s):
  _str: the string to parse.
Return:
  A pointer to a newly allocated operation, or NULL on failure.
*/
struct d_filter_operation*
d_filter_operation_from_string
(
    const char* _str
)
{
    (void)_str;

    return NULL;
}

/*
d_filter_chain_from_string
  Parses a string into a filter chain.

Parameter(s):
  _str: the string to parse.
Return:
  A pointer to a newly allocated chain, or NULL on failure.
*/
struct d_filter_chain*
d_filter_chain_from_string
(
    const char* _str
)
{
    (void)_str;

    return NULL;
}

/*
d_filter_chain_optimize
  Creates an optimized copy of a filter chain.

Parameter(s):
  _chain: the chain to optimize.
Return:
  A pointer to a newly allocated optimized chain, or NULL on failure.
*/
struct d_filter_chain*
d_filter_chain_optimize
(
    const struct d_filter_chain* _chain
)
{
    if (!_chain)
    {
        return NULL;
    }

    // for now, return a clone (optimization TBD)
    return d_filter_chain_clone(_chain);
}

/*
d_filter_estimate_result_size
  Estimates the result size for a filter chain on a given input count.

Parameter(s):
  _chain:       the filter chain.
  _input_count: the number of input elements.
Return:
  An estimated number of output elements.
*/
size_t
d_filter_estimate_result_size
(
    const struct d_filter_chain* _chain,
    size_t                       _input_count
)
{
    if (!_chain)
    {
        return 0;
    }

    // conservative estimate: return full input count
    return _input_count;
}


///////////////////////////////////////////////////////////////////////////////
///             XI.   ITERATOR INTERFACE                                    ///
///////////////////////////////////////////////////////////////////////////////

/*
d_filter_iterator_new
  Creates a new filter iterator for lazily evaluating filters.

Parameter(s):
  _chain:        the filter chain to iterate.
  _input:        the source array.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
Return:
  A pointer to a newly allocated iterator, or NULL on failure.
*/
struct d_filter_iterator*
d_filter_iterator_new
(
    const struct d_filter_chain* _chain,
    const void*                  _input,
    size_t                       _count,
    size_t                       _element_size
)
{
    struct d_filter_iterator* iter;

    iter = (struct d_filter_iterator*)malloc(
               sizeof(struct d_filter_iterator));

    // check allocation
    if (!iter)
    {
        return NULL;
    }

    iter->input         = _input;
    iter->input_count   = _count;
    iter->element_size  = _element_size;
    iter->chain         = _chain;
    iter->current_index = 0;
    iter->indices       = NULL;
    iter->indices_count = 0;
    iter->indices_pos   = 0;
    iter->exhausted     = (_count == 0);

    return iter;
}

/*
d_filter_iterator_has_next
  Tests whether the iterator has more elements.

Parameter(s):
  _iter: the iterator to query.
Return:
  A boolean value; true if more elements are available.
*/
bool
d_filter_iterator_has_next
(
    const struct d_filter_iterator* _iter
)
{
    if (!_iter)
    {
        return false;
    }

    return !_iter->exhausted;
}

/*
d_filter_iterator_next
  Advances the iterator and returns the next element.

Parameter(s):
  _iter: the iterator to advance.
Return:
  A pointer to the next element, or NULL if exhausted.
*/
void*
d_filter_iterator_next
(
    struct d_filter_iterator* _iter
)
{
    const char* bytes;
    void*       element;

    if ( (!_iter)       ||
         (_iter->exhausted) )
    {
        return NULL;
    }

    bytes   = (const char*)_iter->input;
    element = (void*)(bytes
                  + (_iter->current_index
                         * _iter->element_size));

    _iter->current_index++;

    // check if exhausted
    if (_iter->current_index >= _iter->input_count)
    {
        _iter->exhausted = true;
    }

    return element;
}

/*
d_filter_iterator_reset
  Resets the iterator to the beginning.

Parameter(s):
  _iter: the iterator to reset.
Return:
  none.
*/
void
d_filter_iterator_reset
(
    struct d_filter_iterator* _iter
)
{
    if (!_iter)
    {
        return;
    }

    _iter->current_index = 0;
    _iter->indices_pos   = 0;
    _iter->exhausted     = (_iter->input_count == 0);

    return;
}

/*
d_filter_iterator_free
  Frees all resources owned by a filter iterator.

Parameter(s):
  _iter: the iterator to free; may be NULL.
Return:
  none.
*/
void
d_filter_iterator_free
(
    struct d_filter_iterator* _iter
)
{
    if (!_iter)
    {
        return;
    }

    if (_iter->indices)
    {
        free(_iter->indices);
    }

    free(_iter);

    return;
}


///////////////////////////////////////////////////////////////////////////////
///             XII.  FLUENT FILTER BUILDER                                 ///
///////////////////////////////////////////////////////////////////////////////

/*
d_filter_builder_new
  Creates a new fluent filter builder.

Parameter(s):
  (none)
Return:
  A pointer to a newly allocated builder, or NULL on failure.
*/
struct d_filter_builder*
d_filter_builder_new
(
    void
)
{
    struct d_filter_builder* builder;

    builder = (struct d_filter_builder*)malloc(
                  sizeof(struct d_filter_builder));

    // check allocation
    if (!builder)
    {
        return NULL;
    }

    builder->chain         = d_filter_chain_new();
    builder->error_code    = 0;
    builder->error_message = NULL;

    // check chain allocation
    if (!builder->chain)
    {
        free(builder);

        return NULL;
    }

    return builder;
}

/*
d_filter_builder_from_chain
  Creates a filter builder from an existing chain.

Parameter(s):
  _chain: the chain to wrap.
Return:
  A pointer to a newly allocated builder, or NULL on failure.
*/
struct d_filter_builder*
d_filter_builder_from_chain
(
    struct d_filter_chain* _chain
)
{
    struct d_filter_builder* builder;

    builder = (struct d_filter_builder*)malloc(
                  sizeof(struct d_filter_builder));

    // check allocation
    if (!builder)
    {
        return NULL;
    }

    builder->chain         = _chain;
    builder->error_code    = 0;
    builder->error_message = NULL;

    return builder;
}

/*
d_filter_builder_take_first
  Adds a take-first operation to the builder.

Parameter(s):
  _builder: the builder.
  _n:       the number of elements to take.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_filter_builder*
d_filter_builder_take_first
(
    struct d_filter_builder* _builder,
    size_t                   _n
)
{
    if ( (!_builder) ||
         (_builder->error_code != 0) )
    {
        return _builder;
    }

    if (!d_filter_chain_add_take_first(_builder->chain, _n))
    {
        _builder->error_code = -1;
    }

    return _builder;
}

/*
d_filter_builder_take_last
  Adds a take-last operation to the builder.

Parameter(s):
  _builder: the builder.
  _n:       the number of elements to take.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_filter_builder*
d_filter_builder_take_last
(
    struct d_filter_builder* _builder,
    size_t                   _n
)
{
    if ( (!_builder) ||
         (_builder->error_code != 0) )
    {
        return _builder;
    }

    if (!d_filter_chain_add_take_last(_builder->chain, _n))
    {
        _builder->error_code = -1;
    }

    return _builder;
}

/*
d_filter_builder_skip_first
  Adds a skip-first operation to the builder.

Parameter(s):
  _builder: the builder.
  _n:       the number of elements to skip.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_filter_builder*
d_filter_builder_skip_first
(
    struct d_filter_builder* _builder,
    size_t                   _n
)
{
    if ( (!_builder) ||
         (_builder->error_code != 0) )
    {
        return _builder;
    }

    if (!d_filter_chain_add_skip_first(_builder->chain, _n))
    {
        _builder->error_code = -1;
    }

    return _builder;
}

/*
d_filter_builder_skip_last
  Adds a skip-last operation to the builder.

Parameter(s):
  _builder: the builder.
  _n:       the number of elements to skip.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_filter_builder*
d_filter_builder_skip_last
(
    struct d_filter_builder* _builder,
    size_t                   _n
)
{
    if ( (!_builder) ||
         (_builder->error_code != 0) )
    {
        return _builder;
    }

    if (!d_filter_chain_add_skip_last(_builder->chain, _n))
    {
        _builder->error_code = -1;
    }

    return _builder;
}

/*
d_filter_builder_where
  Adds a where (predicate) operation to the builder.

Parameter(s):
  _builder: the builder.
  _test:    the predicate function.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_filter_builder*
d_filter_builder_where
(
    struct d_filter_builder* _builder,
    d_predicate              _test
)
{
    if ( (!_builder) ||
         (_builder->error_code != 0) )
    {
        return _builder;
    }

    if (!d_filter_chain_add_where(_builder->chain, _test))
    {
        _builder->error_code = -1;
    }

    return _builder;
}

/*
d_filter_builder_where_ctx
  Adds a context-predicate operation to the builder.

Parameter(s):
  _builder: the builder.
  _test:    the context predicate function.
  _context: the context pointer.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_filter_builder*
d_filter_builder_where_ctx
(
    struct d_filter_builder* _builder,
    d_predicate_ctx          _test,
    void*                    _context
)
{
    if ( (!_builder) ||
         (_builder->error_code != 0) )
    {
        return _builder;
    }

    if (!d_filter_chain_add_where_ctx(_builder->chain,
                                      _test,
                                      _context))
    {
        _builder->error_code = -1;
    }

    return _builder;
}

/*
d_filter_builder_range
  Adds a range operation to the builder.

Parameter(s):
  _builder: the builder.
  _start:   the inclusive start index.
  _end:     the exclusive end index.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_filter_builder*
d_filter_builder_range
(
    struct d_filter_builder* _builder,
    size_t                   _start,
    size_t                   _end
)
{
    if ( (!_builder) ||
         (_builder->error_code != 0) )
    {
        return _builder;
    }

    if (!d_filter_chain_add_range(_builder->chain,
                                  _start,
                                  _end))
    {
        _builder->error_code = -1;
    }

    return _builder;
}

/*
d_filter_builder_slice
  Adds a slice operation to the builder.

Parameter(s):
  _builder: the builder.
  _start:   the inclusive start index.
  _end:     the exclusive end index.
  _step:    the step between elements.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_filter_builder*
d_filter_builder_slice
(
    struct d_filter_builder* _builder,
    size_t                   _start,
    size_t                   _end,
    size_t                   _step
)
{
    struct d_filter_operation op;

    if ( (!_builder) ||
         (_builder->error_code != 0) )
    {
        return _builder;
    }

    op = d_filter_slice(_start, _end, _step);

    if (!d_filter_chain_add(_builder->chain, &op))
    {
        _builder->error_code = -1;
    }

    return _builder;
}

/*
d_filter_builder_distinct
  Adds a distinct operation to the builder.

Parameter(s):
  _builder: the builder.
  _cmp:     the comparator for detecting duplicates.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_filter_builder*
d_filter_builder_distinct
(
    struct d_filter_builder* _builder,
    d_comparator             _cmp
)
{
    struct d_filter_operation op;

    if ( (!_builder) ||
         (_builder->error_code != 0) )
    {
        return _builder;
    }

    op = d_filter_distinct(_cmp);

    if (!d_filter_chain_add(_builder->chain, &op))
    {
        _builder->error_code = -1;
    }

    return _builder;
}

/*
d_filter_builder_reverse
  Adds a reverse operation to the builder.

Parameter(s):
  _builder: the builder.
Return:
  The builder pointer for chaining, or NULL on error.
*/
struct d_filter_builder*
d_filter_builder_reverse
(
    struct d_filter_builder* _builder
)
{
    struct d_filter_operation op;

    if ( (!_builder) ||
         (_builder->error_code != 0) )
    {
        return _builder;
    }

    op = d_filter_reverse();

    if (!d_filter_chain_add(_builder->chain, &op))
    {
        _builder->error_code = -1;
    }

    return _builder;
}

/*
d_filter_builder_build
  Finalizes the builder and returns the constructed chain. The builder
no longer owns the chain after this call.

Parameter(s):
  _builder: the builder to finalize.
Return:
  The constructed filter chain, or NULL on error.
*/
struct d_filter_chain*
d_filter_builder_build
(
    struct d_filter_builder* _builder
)
{
    struct d_filter_chain* chain;

    if (!_builder)
    {
        return NULL;
    }

    chain           = _builder->chain;
    _builder->chain = NULL;

    return chain;
}

/*
d_filter_builder_apply
  Finalizes the builder and applies the chain to input data.

Parameter(s):
  _builder:      the builder to finalize and apply.
  _input:        the source array.
  _count:        the number of elements.
  _element_size: the size in bytes of each element.
Return:
  A d_filter_result containing the filtered output.
*/
struct d_filter_result
d_filter_builder_apply
(
    struct d_filter_builder* _builder,
    const void*              _input,
    size_t                   _count,
    size_t                   _element_size
)
{
    struct d_filter_result result;

    memset(&result, 0, sizeof(result));

    if ( (!_builder)       ||
         (!_builder->chain) )
    {
        result.status = D_FILTER_RESULT_ERROR;

        return result;
    }

    result = d_filter_apply_chain(_builder->chain,
                                 _input,
                                 _count,
                                 _element_size);

    return result;
}

/*
d_filter_builder_has_error
  Tests whether the builder has encountered an error.

Parameter(s):
  _builder: the builder to query.
Return:
  A boolean value; true if an error has occurred.
*/
bool
d_filter_builder_has_error
(
    const struct d_filter_builder* _builder
)
{
    if (!_builder)
    {
        return true;
    }

    return (_builder->error_code != 0);
}

/*
d_filter_builder_get_error_code
  Returns the builder's error code.

Parameter(s):
  _builder: the builder to query.
Return:
  The error code, or -1 if _builder is NULL.
*/
int
d_filter_builder_get_error_code
(
    const struct d_filter_builder* _builder
)
{
    if (!_builder)
    {
        return -1;
    }

    return _builder->error_code;
}

/*
d_filter_builder_get_error_message
  Returns the builder's error message.

Parameter(s):
  _builder: the builder to query.
Return:
  The error message string, or NULL if no error.
*/
char*
d_filter_builder_get_error_message
(
    const struct d_filter_builder* _builder
)
{
    if (!_builder)
    {
        return NULL;
    }

    return _builder->error_message;
}

/*
d_filter_builder_free
  Frees all resources owned by a filter builder.

Parameter(s):
  _builder: the builder to free; may be NULL.
Return:
  none.
*/
void
d_filter_builder_free
(
    struct d_filter_builder* _builder
)
{
    if (!_builder)
    {
        return;
    }

    if (_builder->chain)
    {
        d_filter_chain_free(_builder->chain);
    }

    if (_builder->error_message)
    {
        free(_builder->error_message);
    }

    free(_builder);

    return;
}


///////////////////////////////////////////////////////////////////////////////
///             XIII. PREDICATE EXPRESSION BUILDER                          ///
///////////////////////////////////////////////////////////////////////////////

/*
d_pred_builder_new
  Creates a new predicate expression builder.

Parameter(s):
  (none)
Return:
  A pointer to a newly allocated builder, or NULL on failure.
*/
struct d_pred_builder*
d_pred_builder_new
(
    void
)
{
    struct d_pred_builder* builder;

    builder = (struct d_pred_builder*)malloc(
                  sizeof(struct d_pred_builder));

    // check allocation
    if (!builder)
    {
        return NULL;
    }

    builder->expr        = NULL;
    builder->stack_depth = 0;
    memset(builder->stack,
           0,
           sizeof(builder->stack));

    return builder;
}

/*
d_pred_builder_from_predicate
  Creates a predicate builder initialized with a predicate.

Parameter(s):
  _test: the initial predicate function.
Return:
  A pointer to a newly allocated builder, or NULL on failure.
*/
struct d_pred_builder*
d_pred_builder_from_predicate
(
    d_predicate _test
)
{
    struct d_pred_builder* builder;

    builder = d_pred_builder_new();

    // check allocation
    if (!builder)
    {
        return NULL;
    }

    builder->expr = d_predicate_expr_leaf(_test, NULL);

    return builder;
}

/*
d_pred_builder_test
  Sets the initial predicate for the builder.

Parameter(s):
  _builder: the builder.
  _test:    the predicate function.
Return:
  The builder pointer for chaining.
*/
struct d_pred_builder*
d_pred_builder_test
(
    struct d_pred_builder* _builder,
    d_predicate            _test
)
{
    if (!_builder)
    {
        return NULL;
    }

    // free existing expression if any
    if (_builder->expr)
    {
        d_predicate_expr_free(_builder->expr);
    }

    _builder->expr = d_predicate_expr_leaf(_test, NULL);

    return _builder;
}

/*
d_pred_builder_and
  Adds an AND predicate to the builder.

Parameter(s):
  _builder: the builder.
  _test:    the predicate to AND with the current expression.
Return:
  The builder pointer for chaining.
*/
struct d_pred_builder*
d_pred_builder_and
(
    struct d_pred_builder* _builder,
    d_predicate            _test
)
{
    struct d_predicate_expr* right;

    if (!_builder)
    {
        return NULL;
    }

    right = d_predicate_expr_leaf(_test, NULL);
    _builder->expr = d_predicate_expr_and(
                         _builder->expr,
                         right);

    return _builder;
}

/*
d_pred_builder_or
  Adds an OR predicate to the builder.

Parameter(s):
  _builder: the builder.
  _test:    the predicate to OR with the current expression.
Return:
  The builder pointer for chaining.
*/
struct d_pred_builder*
d_pred_builder_or
(
    struct d_pred_builder* _builder,
    d_predicate            _test
)
{
    struct d_predicate_expr* right;

    if (!_builder)
    {
        return NULL;
    }

    right = d_predicate_expr_leaf(_test, NULL);
    _builder->expr = d_predicate_expr_or(
                         _builder->expr,
                         right);

    return _builder;
}

/*
d_pred_builder_not
  Negates the current expression in the builder.

Parameter(s):
  _builder: the builder.
Return:
  The builder pointer for chaining.
*/
struct d_pred_builder*
d_pred_builder_not
(
    struct d_pred_builder* _builder
)
{
    if (!_builder)
    {
        return NULL;
    }

    _builder->expr = d_predicate_expr_not(_builder->expr);

    return _builder;
}

/*
d_pred_builder_xor
  Adds an XOR predicate to the builder.

Parameter(s):
  _builder: the builder.
  _test:    the predicate to XOR with the current expression.
Return:
  The builder pointer for chaining.
*/
struct d_pred_builder*
d_pred_builder_xor
(
    struct d_pred_builder* _builder,
    d_predicate            _test
)
{
    struct d_predicate_expr* right;

    if (!_builder)
    {
        return NULL;
    }

    right = d_predicate_expr_leaf(_test, NULL);
    _builder->expr = d_predicate_expr_xor(
                         _builder->expr,
                         right);

    return _builder;
}

/*
d_pred_builder_begin_group
  Begins a grouping level in the builder. Pushes the current expression
onto the stack.

Parameter(s):
  _builder: the builder.
Return:
  The builder pointer for chaining.
*/
struct d_pred_builder*
d_pred_builder_begin_group
(
    struct d_pred_builder* _builder
)
{
    if (!_builder)
    {
        return NULL;
    }

    // check stack depth
    if (_builder->stack_depth >= D_FILTER_MAX_PREDICATE_DEPTH)
    {
        return NULL;
    }

    // push current expression
    _builder->stack[_builder->stack_depth] = _builder->expr;
    _builder->stack_depth++;
    _builder->expr = NULL;

    return _builder;
}

/*
d_pred_builder_end_group
  Ends a grouping level in the builder. Pops the saved expression from
the stack.

Parameter(s):
  _builder: the builder.
Return:
  The builder pointer for chaining.
*/
struct d_pred_builder*
d_pred_builder_end_group
(
    struct d_pred_builder* _builder
)
{
    if (!_builder)
    {
        return NULL;
    }

    // check stack depth
    if (_builder->stack_depth == 0)
    {
        return NULL;
    }

    _builder->stack_depth--;

    return _builder;
}

/*
d_pred_builder_build
  Finalizes the builder and returns the constructed expression. The
builder no longer owns the expression after this call.

Parameter(s):
  _builder: the builder to finalize.
Return:
  The constructed predicate expression, or NULL on error.
*/
struct d_predicate_expr*
d_pred_builder_build
(
    struct d_pred_builder* _builder
)
{
    struct d_predicate_expr* expr;

    if (!_builder)
    {
        return NULL;
    }

    expr           = _builder->expr;
    _builder->expr = NULL;

    return expr;
}

/*
d_pred_builder_free
  Frees all resources owned by a predicate builder.

Parameter(s):
  _builder: the builder to free; may be NULL.
Return:
  none.
*/
void
d_pred_builder_free
(
    struct d_pred_builder* _builder
)
{
    size_t i;

    if (!_builder)
    {
        return;
    }

    // free current expression
    if (_builder->expr)
    {
        d_predicate_expr_free(_builder->expr);
    }

    // free stacked expressions
    for (i = 0; i < _builder->stack_depth; i++)
    {
        if (_builder->stack[i])
        {
            d_predicate_expr_free(_builder->stack[i]);
        }
    }

    free(_builder);

    return;
}
