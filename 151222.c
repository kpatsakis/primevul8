  Statement_Ptr Expand::operator()(At_Root_Block_Ptr a)
  {
    Block_Obj ab = a->block();
    Expression_Obj ae = a->expression();

    if (ae) ae = ae->perform(&eval);
    else ae = SASS_MEMORY_NEW(At_Root_Query, a->pstate());

    LOCAL_FLAG(at_root_without_rule, true);
    LOCAL_FLAG(in_keyframes, false);

                                       ;

    Block_Obj bb = ab ? operator()(ab) : NULL;
    At_Root_Block_Obj aa = SASS_MEMORY_NEW(At_Root_Block,
                                        a->pstate(),
                                        bb,
                                        Cast<At_Root_Query>(ae));
    return aa.detach();
  }