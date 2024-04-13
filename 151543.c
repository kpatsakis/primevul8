parse_identifier(const char *id)
{
    unsigned int parsed = 0;

    assert(id);

    if (!isalpha(id[0]) && (id[0] != '_')) {
        return -parsed;
    }

    ++parsed;
    ++id;

    while (isalnum(id[0]) || (id[0] == '_') || (id[0] == '-') || (id[0] == '.')) {
        ++parsed;
        ++id;
    }

    return parsed;
}