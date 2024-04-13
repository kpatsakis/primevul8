new_string(const char *s, Py_ssize_t len, struct tok_state *tok)
{
    char* result = (char *)PyMem_MALLOC(len + 1);
    if (!result) {
        tok->done = E_NOMEM;
        return NULL;
    }
    memcpy(result, s, len);
    result[len] = '\0';
    return result;
}