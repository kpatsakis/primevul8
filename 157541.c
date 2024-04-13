header_callback(char *ptr, size_t size, size_t nmemb, void *stream)
{
    return util_write_callback(1, ptr, size, nmemb, stream);
}