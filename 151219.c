  Statement_Ptr Expand::operator()(Supports_Block_Ptr f)
  {
    Expression_Obj condition = f->condition()->perform(&eval);
    Supports_Block_Obj ff = SASS_MEMORY_NEW(Supports_Block,
                                       f->pstate(),
                                       Cast<Supports_Condition>(condition),
                                       operator()(f->block()));
    return ff.detach();
  }