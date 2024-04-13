  Statement_Ptr Expand::operator()(Media_Block_Ptr m)
  {
    Media_Block_Obj cpy = SASS_MEMORY_COPY(m);
    // Media_Blocks are prone to have circular references
    // Copy could leak memory if it does not get picked up
    // Looks like we are able to reset block reference for copy
    // Good as it will ensure a low memory overhead for this fix
    // So this is a cheap solution with a minimal price
    ctx.ast_gc.push_back(cpy); cpy->block(0);
    Expression_Obj mq = eval(m->media_queries());
    std::string str_mq(mq->to_string(ctx.c_options));
    char* str = sass_copy_c_string(str_mq.c_str());
    ctx.strings.push_back(str);
    Parser p(Parser::from_c_str(str, ctx, traces, mq->pstate()));
    mq = p.parse_media_queries(); // re-assign now
    cpy->media_queries(mq);
    media_stack.push_back(cpy);
    Block_Obj blk = operator()(m->block());
    Media_Block_Ptr mm = SASS_MEMORY_NEW(Media_Block,
                                      m->pstate(),
                                      mq,
                                      blk);
    media_stack.pop_back();
    mm->tabs(m->tabs());
    return mm;
  }