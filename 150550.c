yajl_string_encode(yajl_buf buf, const unsigned char * str,
                   unsigned int len, unsigned int htmlSafe)
{
    yajl_string_encode2((const yajl_print_t) &yajl_buf_append, buf, str, len, htmlSafe);
}