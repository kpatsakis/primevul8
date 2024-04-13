exim_exit(int rc, const uschar * process)
{
search_tidyup();
DEBUG(D_any)
  debug_printf(">>>>>>>>>>>>>>>> Exim pid=%d %s%s%sterminating with rc=%d "
    ">>>>>>>>>>>>>>>>\n", (int)getpid(),
    process ? "(" : "", process, process ? ") " : "", rc);
exit(rc);
}