
static int sort_sects(const void *first, const void *second) {
    const struct cli_exe_section *a = first, *b = second;
    return (a->raw - b->raw);