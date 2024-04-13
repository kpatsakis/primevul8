  inline void Expand::append_block(Block_Ptr b)
  {
    if (b->is_root()) call_stack.push_back(b);
    for (size_t i = 0, L = b->length(); i < L; ++i) {
      Statement_Ptr stm = b->at(i);
      Statement_Obj ith = stm->perform(this);
      if (ith) block_stack.back()->append(ith);
    }
    if (b->is_root()) call_stack.pop_back();
  }