  Statement_Ptr Expand::operator()(For_Ptr f)
  {
    std::string variable(f->variable());
    Expression_Obj low = f->lower_bound()->perform(&eval);
    if (low->concrete_type() != Expression::NUMBER) {
      traces.push_back(Backtrace(low->pstate()));
      throw Exception::TypeMismatch(traces, *low, "integer");
    }
    Expression_Obj high = f->upper_bound()->perform(&eval);
    if (high->concrete_type() != Expression::NUMBER) {
      traces.push_back(Backtrace(high->pstate()));
      throw Exception::TypeMismatch(traces, *high, "integer");
    }
    Number_Obj sass_start = Cast<Number>(low);
    Number_Obj sass_end = Cast<Number>(high);
    // check if units are valid for sequence
    if (sass_start->unit() != sass_end->unit()) {
      std::stringstream msg; msg << "Incompatible units: '"
        << sass_start->unit() << "' and '"
        << sass_end->unit() << "'.";
      error(msg.str(), low->pstate(), traces);
    }
    double start = sass_start->value();
    double end = sass_end->value();
    // only create iterator once in this environment
    Env env(environment(), true);
    env_stack.push_back(&env);
    call_stack.push_back(f);
    Block_Ptr body = f->block();
    if (start < end) {
      if (f->is_inclusive()) ++end;
      for (double i = start;
           i < end;
           ++i) {
        Number_Obj it = SASS_MEMORY_NEW(Number, low->pstate(), i, sass_end->unit());
        env.set_local(variable, it);
        append_block(body);
      }
    } else {
      if (f->is_inclusive()) --end;
      for (double i = start;
           i > end;
           --i) {
        Number_Obj it = SASS_MEMORY_NEW(Number, low->pstate(), i, sass_end->unit());
        env.set_local(variable, it);
        append_block(body);
      }
    }
    call_stack.pop_back();
    env_stack.pop_back();
    return 0;
  }