  Statement_Ptr Expand::operator()(Error_Ptr e)
  {
    // eval handles this too, because errors may occur in functions
    e->perform(&eval);
    return 0;
  }