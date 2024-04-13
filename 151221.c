  Statement_Ptr Expand::operator()(Return_Ptr r)
  {
    error("@return may only be used within a function", r->pstate(), traces);
    return 0;
  }