  Statement_Ptr Expand::operator()(Directive_Ptr a)
  {
    LOCAL_FLAG(in_keyframes, a->is_keyframes());
    Block_Ptr ab = a->block();
    Selector_List_Ptr as = a->selector();
    Expression_Ptr av = a->value();
    selector_stack.push_back(0);
    if (av) av = av->perform(&eval);
    if (as) as = eval(as);
    selector_stack.pop_back();
    Block_Ptr bb = ab ? operator()(ab) : NULL;
    Directive_Ptr aa = SASS_MEMORY_NEW(Directive,
                                  a->pstate(),
                                  a->keyword(),
                                  as,
                                  bb,
                                  av);
    return aa;
  }