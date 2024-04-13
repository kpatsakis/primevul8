onig_init(void)
{
  if (onig_inited != 0)
    return 0;

  onig_inited = 1;

#if defined(ONIG_DEBUG_MEMLEAK) && defined(_MSC_VER)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  onigenc_init();
  /* onigenc_set_default_caseconv_table((UChar* )0); */

#ifdef ONIG_DEBUG_STATISTICS
  onig_statistics_init();
#endif

  return 0;
}