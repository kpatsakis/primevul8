print_error_text(PyObject *f, int offset, PyObject *text_obj)
{
    const char *text;
    const char *nl;

    text = PyUnicode_AsUTF8(text_obj);
    if (text == NULL)
        return;

    if (offset >= 0) {
        if (offset > 0 && (size_t)offset == strlen(text) && text[offset - 1] == '\n')
            offset--;
        for (;;) {
            nl = strchr(text, '\n');
            if (nl == NULL || nl-text >= offset)
                break;
            offset -= (int)(nl+1-text);
            text = nl+1;
        }
        while (*text == ' ' || *text == '\t' || *text == '\f') {
            text++;
            offset--;
        }
    }
    PyFile_WriteString("    ", f);
    PyFile_WriteString(text, f);
    if (*text == '\0' || text[strlen(text)-1] != '\n')
        PyFile_WriteString("\n", f);
    if (offset == -1)
        return;
    PyFile_WriteString("    ", f);
    while (--offset > 0)
        PyFile_WriteString(" ", f);
    PyFile_WriteString("^\n", f);
}