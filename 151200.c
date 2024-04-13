  Statement_Ptr Expand::operator()(Warning_Ptr w)
  {
    // eval handles this too, because warnings may occur in functions
    w->perform(&eval);
    return 0;
  }