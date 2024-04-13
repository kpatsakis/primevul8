  Statement_Ptr Expand::operator()(Assignment_Ptr a)
  {
    Env* env = environment();
    const std::string& var(a->variable());
    if (a->is_global()) {
      if (a->is_default()) {
        if (env->has_global(var)) {
          Expression_Obj e = Cast<Expression>(env->get_global(var));
          if (!e || e->concrete_type() == Expression::NULL_VAL) {
            env->set_global(var, a->value()->perform(&eval));
          }
        }
        else {
          env->set_global(var, a->value()->perform(&eval));
        }
      }
      else {
        env->set_global(var, a->value()->perform(&eval));
      }
    }
    else if (a->is_default()) {
      if (env->has_lexical(var)) {
        auto cur = env;
        while (cur && cur->is_lexical()) {
          if (cur->has_local(var)) {
            if (AST_Node_Obj node = cur->get_local(var)) {
              Expression_Obj e = Cast<Expression>(node);
              if (!e || e->concrete_type() == Expression::NULL_VAL) {
                cur->set_local(var, a->value()->perform(&eval));
              }
            }
            else {
              throw std::runtime_error("Env not in sync");
            }
            return 0;
          }
          cur = cur->parent();
        }
        throw std::runtime_error("Env not in sync");
      }
      else if (env->has_global(var)) {
        if (AST_Node_Obj node = env->get_global(var)) {
          Expression_Obj e = Cast<Expression>(node);
          if (!e || e->concrete_type() == Expression::NULL_VAL) {
            env->set_global(var, a->value()->perform(&eval));
          }
        }
      }
      else if (env->is_lexical()) {
        env->set_local(var, a->value()->perform(&eval));
      }
      else {
        env->set_local(var, a->value()->perform(&eval));
      }
    }
    else {
      env->set_lexical(var, a->value()->perform(&eval));
    }
    return 0;
  }