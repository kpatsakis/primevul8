get_case_fold_codes_by_str(OnigCaseFoldType flag,
			   const OnigUChar* p, const OnigUChar* end,
			   OnigCaseFoldCodeItem items[],
			   OnigEncoding enc ARG_UNUSED)
{
  return onigenc_get_case_fold_codes_by_str_with_map(
	     numberof(CaseFoldMap), CaseFoldMap, 0,
	     flag, p, end, items);
}