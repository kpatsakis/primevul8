  Statement_Ptr Expand::operator()(Comment_Ptr c)
  {
    if (ctx.output_style() == COMPRESSED) {
      // comments should not be evaluated in compact
      // https://github.com/sass/libsass/issues/2359
      if (!c->is_important()) return NULL;
    }
    eval.is_in_comment = true;
    Comment_Ptr rv = SASS_MEMORY_NEW(Comment, c->pstate(), Cast<String>(c->text()->perform(&eval)), c->is_important());
    eval.is_in_comment = false;
    // TODO: eval the text, once we're parsing/storing it as a String_Schema
    return rv;
  }