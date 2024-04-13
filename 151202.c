  void Expand::expand_selector_list(Selector_Obj s, Selector_List_Obj extender) {

    if (Selector_List_Obj sl = Cast<Selector_List>(s)) {
      for (Complex_Selector_Obj complex_selector : sl->elements()) {
        Complex_Selector_Obj tail = complex_selector;
        while (tail) {
          if (tail->head()) for (Simple_Selector_Obj header : tail->head()->elements()) {
            if (Cast<Parent_Selector>(header) == NULL) continue; // skip all others
            std::string sel_str(complex_selector->to_string(ctx.c_options));
            error("Can't extend " + sel_str + ": can't extend parent selectors", header->pstate(), traces);
          }
          tail = tail->tail();
        }
      }
    }


    Selector_List_Obj contextualized = Cast<Selector_List>(s->perform(&eval));
    if (contextualized == false) return;
    for (auto complex_sel : contextualized->elements()) {
      Complex_Selector_Obj c = complex_sel;
      if (!c->head() || c->tail()) {
        std::string sel_str(contextualized->to_string(ctx.c_options));
        error("Can't extend " + sel_str + ": can't extend nested selectors", c->pstate(), traces);
      }
      Compound_Selector_Obj target = c->head();
      if (contextualized->is_optional()) target->is_optional(true);
      for (size_t i = 0, L = extender->length(); i < L; ++i) {
        Complex_Selector_Obj sel = (*extender)[i];
        if (!(sel->head() && sel->head()->length() > 0 &&
            Cast<Parent_Selector>((*sel->head())[0])))
        {
          Compound_Selector_Obj hh = SASS_MEMORY_NEW(Compound_Selector, (*extender)[i]->pstate());
          hh->media_block((*extender)[i]->media_block());
          Complex_Selector_Obj ssel = SASS_MEMORY_NEW(Complex_Selector, (*extender)[i]->pstate());
          ssel->media_block((*extender)[i]->media_block());
          if (sel->has_line_feed()) ssel->has_line_feed(true);
          Parent_Selector_Obj ps = SASS_MEMORY_NEW(Parent_Selector, (*extender)[i]->pstate());
          ps->media_block((*extender)[i]->media_block());
          hh->append(ps);
          ssel->tail(sel);
          ssel->head(hh);
          sel = ssel;
        }
        // if (c->has_line_feed()) sel->has_line_feed(true);
        ctx.subset_map.put(target, std::make_pair(sel, target));
      }
    }

  }