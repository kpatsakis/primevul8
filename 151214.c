  Statement_Ptr Expand::operator()(If_Ptr i)
  {
    Env env(environment(), true);
    env_stack.push_back(&env);
    call_stack.push_back(i);
    Expression_Obj rv = i->predicate()->perform(&eval);
    if (*rv) {
      append_block(i->block());
    }
    else {
      Block_Ptr alt = i->alternative();
      if (alt) append_block(alt);
    }
    call_stack.pop_back();
    env_stack.pop_back();
    return 0;
  }