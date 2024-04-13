  Statement_Ptr Expand::operator()(Mixin_Call_Ptr c)
  {
    if (recursions > maxRecursion) {
      throw Exception::StackError(traces, *c);
    }

    recursions ++;

    Env* env = environment();
    std::string full_name(c->name() + "[m]");
    if (!env->has(full_name)) {
      error("no mixin named " + c->name(), c->pstate(), traces);
    }
    Definition_Obj def = Cast<Definition>((*env)[full_name]);
    Block_Obj body = def->block();
    Parameters_Obj params = def->parameters();

    if (c->block() && c->name() != "@content" && !body->has_content()) {
      error("Mixin \"" + c->name() + "\" does not accept a content block.", c->pstate(), traces);
    }
    Expression_Obj rv = c->arguments()->perform(&eval);
    Arguments_Obj args = Cast<Arguments>(rv);
    std::string msg(", in mixin `" + c->name() + "`");
    traces.push_back(Backtrace(c->pstate(), msg));
    ctx.callee_stack.push_back({
      c->name().c_str(),
      c->pstate().path,
      c->pstate().line + 1,
      c->pstate().column + 1,
      SASS_CALLEE_MIXIN,
      { env }
    });

    Env new_env(def->environment());
    env_stack.push_back(&new_env);
    if (c->block()) {
      // represent mixin content blocks as thunks/closures
      Definition_Obj thunk = SASS_MEMORY_NEW(Definition,
                                          c->pstate(),
                                          "@content",
                                          SASS_MEMORY_NEW(Parameters, c->pstate()),
                                          c->block(),
                                          Definition::MIXIN);
      thunk->environment(env);
      new_env.local_frame()["@content[m]"] = thunk;
    }

    bind(std::string("Mixin"), c->name(), params, args, &new_env, &eval, traces);

    Block_Obj trace_block = SASS_MEMORY_NEW(Block, c->pstate());
    Trace_Obj trace = SASS_MEMORY_NEW(Trace, c->pstate(), c->name(), trace_block);

    env->set_global("is_in_mixin", bool_true);
    if (Block_Ptr pr = block_stack.back()) {
      trace_block->is_root(pr->is_root());
    }
    block_stack.push_back(trace_block);
    for (auto bb : body->elements()) {
      if (Ruleset_Ptr r = Cast<Ruleset>(bb)) {
        r->is_root(trace_block->is_root());
      }
      Statement_Obj ith = bb->perform(this);
      if (ith) trace->block()->append(ith);
    }
    block_stack.pop_back();
    env->del_global("is_in_mixin");

    ctx.callee_stack.pop_back();
    env_stack.pop_back();
    traces.pop_back();

    recursions --;
    return trace.detach();
  }