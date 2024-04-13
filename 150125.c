cp1251_apply_all_case_fold(OnigCaseFoldType flag,
			       OnigApplyAllCaseFoldFunc f, void* arg, OnigEncoding enc ARG_UNUSED)
{
  return onigenc_apply_all_case_fold_with_map(
             numberof(CaseFoldMap), CaseFoldMap, 0,
             flag, f, arg);
}