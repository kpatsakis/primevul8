static void addfile(const char *name, const char *suffix)
{
    struct filename *p;
    unsigned int l;

    if (!name || !suffix) {
        return;
    }
    if (matches >= max_ls_files) {
        return;
    }
    matches++;
    l = (unsigned int) (strlen(name) + strlen(suffix));
    if (l > colwidth) {
        colwidth = l;
    }
    if ((p = malloc(offsetof(struct filename, line) + l + 1U)) == NULL) {
        return;
    }
    if (SNCHECK(snprintf(p->line, l + 1U, "%s%s", name, suffix), l + 1U)) {
        _EXIT(EXIT_FAILURE);
    }
    if (tail != NULL) {
        tail->down = p;
    } else {
        head = p;
    }
    tail = p;
    filenames++;
}