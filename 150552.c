yajl_string_encode2(const yajl_print_t print,
                    void * ctx,
                    const unsigned char * str,
                    unsigned int len,
                    unsigned int htmlSafe)
{
    unsigned int beg = 0;
    unsigned int end = 0;    
    char hexBuf[7];
    hexBuf[0] = '\\'; hexBuf[1] = 'u'; hexBuf[2] = '0'; hexBuf[3] = '0';
    hexBuf[6] = 0;

    while (end < len) {
        const char * escaped = NULL;
        switch (str[end]) {
            case '\r': escaped = "\\r"; break;
            case '\n': escaped = "\\n"; break;
            case '\\': escaped = "\\\\"; break;
            /* case '/': escaped = "\\/"; break; */
            case '"': escaped = "\\\""; break;
            case '\f': escaped = "\\f"; break;
            case '\b': escaped = "\\b"; break;
            case '\t': escaped = "\\t"; break;
            case '/':
              if (htmlSafe) {
                escaped = "\\/";
              }
              break;
            default:
                if ((unsigned char) str[end] < 32) {
                    CharToHex(str[end], hexBuf + 4);
                    escaped = hexBuf;
                }
                break;
        }
        if (escaped != NULL) {
            print(ctx, (const char *) (str + beg), end - beg);
            print(ctx, escaped, (unsigned int)strlen(escaped));
            beg = ++end;
        } else {
            ++end;
        }
    }
    print(ctx, (const char *) (str + beg), end - beg);
}