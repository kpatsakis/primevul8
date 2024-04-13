iff_stack_clean(struct iff_stack *stack)
{
    stack->size = 0;
    free(stack->stack);
}