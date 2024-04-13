  Statement* Expand::operator()(Extension_Ptr e)
  {
    if (Selector_List_Ptr extender = selector()) {
      Selector_List_Ptr sl = e->selector();
      // abort on invalid selector
      if (sl == NULL) return NULL;
      if (Selector_Schema_Ptr schema = sl->schema()) {
        if (schema->has_real_parent_ref()) {
          // put root block on stack again (ignore parents)
          // selector schema must not connect in eval!
          block_stack.push_back(block_stack.at(1));
          sl = eval(sl->schema());
          block_stack.pop_back();
        } else {
          selector_stack.push_back(0);
          sl = eval(sl->schema());
          selector_stack.pop_back();
        }
      }
      for (Complex_Selector_Obj cs : sl->elements()) {
        if (!cs.isNull() && !cs->head().isNull()) {
          cs->head()->media_block(media_stack.back());
        }
      }
      selector_stack.push_back(0);
      expand_selector_list(sl, extender);
      selector_stack.pop_back();
    }
    return 0;
  }