  Expand::Expand(Context& ctx, Env* env, SelectorStack* stack)
  : ctx(ctx),
    traces(ctx.traces),
    eval(Eval(*this)),
    recursions(0),
    in_keyframes(false),
    at_root_without_rule(false),
    old_at_root_without_rule(false),
    env_stack(EnvStack()),
    block_stack(BlockStack()),
    call_stack(CallStack()),
    selector_stack(SelectorStack()),
    media_stack(MediaStack())
  {
    env_stack.push_back(0);
    env_stack.push_back(env);
    block_stack.push_back(0);
    call_stack.push_back(0);
    if (stack == NULL) { selector_stack.push_back(0); }
    else { selector_stack.insert(selector_stack.end(), stack->begin(), stack->end()); }
    media_stack.push_back(0);
  }