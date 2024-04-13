
static void cli_multifree(void *f, ...) {
    void *ff;
    va_list ap;
    free(f);
    va_start(ap, f);
    while((ff=va_arg(ap, void*))) free(ff);
    va_end(ap);