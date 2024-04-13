  Statement_Ptr Expand::operator()(Ruleset_Ptr r)
  {
    LOCAL_FLAG(old_at_root_without_rule, at_root_without_rule);

    if (in_keyframes) {
      Block_Ptr bb = operator()(r->block());
      Keyframe_Rule_Obj k = SASS_MEMORY_NEW(Keyframe_Rule, r->pstate(), bb);
      if (r->selector()) {
        if (Selector_List_Ptr s = r->selector()) {
          selector_stack.push_back(0);
          k->name(s->eval(eval));
          selector_stack.pop_back();
        }
      }
      return k.detach();
    }

    // reset when leaving scope
    LOCAL_FLAG(at_root_without_rule, false);

    // `&` is allowed in `@at-root`!
    bool has_parent_selector = false;
    for (size_t i = 0, L = selector_stack.size(); i < L && !has_parent_selector; i++) {
      Selector_List_Obj ll = selector_stack.at(i);
      has_parent_selector = ll != 0 && ll->length() > 0;
    }

    Selector_List_Obj sel = r->selector();
    if (sel) sel = sel->eval(eval);

    // check for parent selectors in base level rules
    if (r->is_root() || (block_stack.back() && block_stack.back()->is_root())) {
      if (Selector_List_Ptr selector_list = Cast<Selector_List>(r->selector())) {
        for (Complex_Selector_Obj complex_selector : selector_list->elements()) {
          Complex_Selector_Ptr tail = complex_selector;
          while (tail) {
            if (tail->head()) for (Simple_Selector_Obj header : tail->head()->elements()) {
              Parent_Selector_Ptr ptr = Cast<Parent_Selector>(header);
              if (ptr == NULL || (!ptr->real() || has_parent_selector)) continue;
              std::string sel_str(complex_selector->to_string(ctx.c_options));
              error("Base-level rules cannot contain the parent-selector-referencing character '&'.", header->pstate(), traces);
            }
            tail = tail->tail();
          }
        }
      }
    }
    else {
      if (sel->length() == 0 || sel->has_parent_ref()) {
        if (sel->has_real_parent_ref() && !has_parent_selector) {
          error("Base-level rules cannot contain the parent-selector-referencing character '&'.", sel->pstate(), traces);
        }
      }
    }

    // do not connect parent again
    sel->remove_parent_selectors();
    selector_stack.push_back(sel);
    Env env(environment());
    if (block_stack.back()->is_root()) {
      env_stack.push_back(&env);
    }
    sel->set_media_block(media_stack.back());
    Block_Obj blk = 0;
    if (r->block()) blk = operator()(r->block());
    Ruleset_Ptr rr = SASS_MEMORY_NEW(Ruleset,
                                  r->pstate(),
                                  sel,
                                  blk);
    selector_stack.pop_back();
    if (block_stack.back()->is_root()) {
      env_stack.pop_back();
    }

    rr->is_root(r->is_root());
    rr->tabs(r->tabs());

    return rr;
  }