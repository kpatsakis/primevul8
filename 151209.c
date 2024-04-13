  Statement_Ptr Expand::operator()(Import_Ptr imp)
  {
    Import_Obj result = SASS_MEMORY_NEW(Import, imp->pstate());
    if (imp->import_queries() && imp->import_queries()->size()) {
      Expression_Obj ex = imp->import_queries()->perform(&eval);
      result->import_queries(Cast<List>(ex));
    }
    for ( size_t i = 0, S = imp->urls().size(); i < S; ++i) {
      result->urls().push_back(imp->urls()[i]->perform(&eval));
    }
    // all resources have been dropped for Input_Stubs
    // for ( size_t i = 0, S = imp->incs().size(); i < S; ++i) {}
    return result.detach();
  }