iff_stack_push(struct iff_stack *stack, uint8_t value)
{
    if (stack->index == stack->size) {
        stack->size += 4;
        stack->stack = ly_realloc(stack->stack, stack->size * sizeof *stack->stack);
        LY_CHECK_ERR_RETURN(!stack->stack, LOGMEM(NULL); stack->size = 0, EXIT_FAILURE);
    }

    stack->stack[stack->index++] = value;
    return EXIT_SUCCESS;
}