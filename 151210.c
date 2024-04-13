  Statement_Ptr Expand::operator()(Each_Ptr e)
  {
    std::vector<std::string> variables(e->variables());
    Expression_Obj expr = e->list()->perform(&eval);
    List_Obj list = 0;
    Map_Obj map;
    if (expr->concrete_type() == Expression::MAP) {
      map = Cast<Map>(expr);
    }
    else if (Selector_List_Ptr ls = Cast<Selector_List>(expr)) {
      Listize listize;
      Expression_Obj rv = ls->perform(&listize);
      list = Cast<List>(rv);
    }
    else if (expr->concrete_type() != Expression::LIST) {
      list = SASS_MEMORY_NEW(List, expr->pstate(), 1, SASS_COMMA);
      list->append(expr);
    }
    else {
      list = Cast<List>(expr);
    }
    // remember variables and then reset them
    Env env(environment(), true);
    env_stack.push_back(&env);
    call_stack.push_back(e);
    Block_Ptr body = e->block();

    if (map) {
      for (auto key : map->keys()) {
        Expression_Obj k = key->perform(&eval);
        Expression_Obj v = map->at(key)->perform(&eval);

        if (variables.size() == 1) {
          List_Obj variable = SASS_MEMORY_NEW(List, map->pstate(), 2, SASS_SPACE);
          variable->append(k);
          variable->append(v);
          env.set_local(variables[0], variable);
        } else {
          env.set_local(variables[0], k);
          env.set_local(variables[1], v);
        }
        append_block(body);
      }
    }
    else {
      // bool arglist = list->is_arglist();
      if (list->length() == 1 && Cast<Selector_List>(list)) {
        list = Cast<List>(list);
      }
      for (size_t i = 0, L = list->length(); i < L; ++i) {
        Expression_Obj item = list->at(i);
        // unwrap value if the expression is an argument
        if (Argument_Obj arg = Cast<Argument>(item)) item = arg->value();
        // check if we got passed a list of args (investigate)
        if (List_Obj scalars = Cast<List>(item)) {
          if (variables.size() == 1) {
            List_Obj var = scalars;
            // if (arglist) var = (*scalars)[0];
            env.set_local(variables[0], var);
          } else {
            for (size_t j = 0, K = variables.size(); j < K; ++j) {
              Expression_Obj res = j >= scalars->length()
                ? SASS_MEMORY_NEW(Null, expr->pstate())
                : (*scalars)[j]->perform(&eval);
              env.set_local(variables[j], res);
            }
          }
        } else {
          if (variables.size() > 0) {
            env.set_local(variables.at(0), item);
            for (size_t j = 1, K = variables.size(); j < K; ++j) {
              Expression_Obj res = SASS_MEMORY_NEW(Null, expr->pstate());
              env.set_local(variables[j], res);
            }
          }
        }
        append_block(body);
      }
    }
    call_stack.pop_back();
    env_stack.pop_back();
    return 0;
  }