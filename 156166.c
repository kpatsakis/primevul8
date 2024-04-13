backref_check_at_nested_level(regex_t* reg,
                              StackType* top, StackType* stk_base,
                              int nest, int mem_num, MemNumType* memp)
{
  int level;
  StackType* k;

  level = 0;
  k = top;
  k--;
  while (k >= stk_base) {
    if (k->type == STK_CALL_FRAME) {
      level--;
    }
    else if (k->type == STK_RETURN) {
      level++;
    }
    else if (level == nest) {
      if (k->type == STK_MEM_END) {
        if (mem_is_in_memp(k->zid, mem_num, memp)) {
          return 1;
        }
      }
    }
    k--;
  }

  return 0;
}