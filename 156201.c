stack_double(int is_alloca, char** arg_alloc_base,
             StackType** arg_stk_base, StackType** arg_stk_end, StackType** arg_stk,
             MatchArg* msa)
{
  unsigned int n;
  int used;
  size_t size;
  size_t new_size;
  char* alloc_base;
  char* new_alloc_base;
  StackType *stk_base, *stk_end, *stk;

  alloc_base = *arg_alloc_base;
  stk_base = *arg_stk_base;
  stk_end  = *arg_stk_end;
  stk      = *arg_stk;

  n = (unsigned int )(stk_end - stk_base);
  size = sizeof(StackIndex) * msa->ptr_num + sizeof(StackType) * n;
  n *= 2;
  new_size = sizeof(StackIndex) * msa->ptr_num + sizeof(StackType) * n;
  if (is_alloca != 0) {
    new_alloc_base = (char* )xmalloc(new_size);
    if (IS_NULL(new_alloc_base)) {
      STACK_SAVE;
      return ONIGERR_MEMORY;
    }
    xmemcpy(new_alloc_base, alloc_base, size);
  }
  else {
    if (msa->match_stack_limit != 0 && n > msa->match_stack_limit) {
      if ((unsigned int )(stk_end - stk_base) == msa->match_stack_limit)
        return ONIGERR_MATCH_STACK_LIMIT_OVER;
      else
        n = msa->match_stack_limit;
    }
    new_alloc_base = (char* )xrealloc(alloc_base, new_size);
    if (IS_NULL(new_alloc_base)) {
      STACK_SAVE;
      return ONIGERR_MEMORY;
    }
  }

  alloc_base = new_alloc_base;
  used = (int )(stk - stk_base);
  *arg_alloc_base = alloc_base;
  *arg_stk_base   = (StackType* )(alloc_base
                                  + (sizeof(StackIndex) * msa->ptr_num));
  *arg_stk      = *arg_stk_base + used;
  *arg_stk_end  = *arg_stk_base + n;
  return 0;
}