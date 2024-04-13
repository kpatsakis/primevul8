  Selector_List_Obj Expand::selector()
  {
    if (selector_stack.size() > 0)
      return selector_stack.back();
    return 0;
  }