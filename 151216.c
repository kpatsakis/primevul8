  Env* Expand::environment()
  {
    if (env_stack.size() > 0)
      return env_stack.back();
    return 0;
  }