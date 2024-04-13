  Statement_Ptr Expand::operator()(Content_Ptr c)
  {
    Env* env = environment();
    // convert @content directives into mixin calls to the underlying thunk
    if (!env->has("@content[m]")) return 0;

    if (block_stack.back()->is_root()) {
      selector_stack.push_back(0);
    }

    Mixin_Call_Obj call = SASS_MEMORY_NEW(Mixin_Call,
                                       c->pstate(),
                                       "@content",
                                       SASS_MEMORY_NEW(Arguments, c->pstate()));

    Trace_Obj trace = Cast<Trace>(call->perform(this));

    if (block_stack.back()->is_root()) {
      selector_stack.pop_back();
    }

    return trace.detach();
  }