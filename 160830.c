randomize_buffer( byte *buffer, size_t length, int level )
{
    char *p = get_random_bits( length*8, level, 1 );
    memcpy( buffer, p, length );
    xfree(p);
}