tok_dump(int type, char *start, char *end)
{
    printf("%s", _Ta3Parser_TokenNames[type]);
    if (type == NAME || type == NUMBER || type == STRING || type == OP)
        printf("(%.*s)", (int)(end - start), start);
}