iff_stack_pop(struct iff_stack *stack)
{
    stack->index--;
    return stack->stack[stack->index];
}