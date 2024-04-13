static inline void advance_token_pos(const char **const str,
                                     const char **const start,
                                     const char **const end)
{
    *start = *str;
    *end   = *start;
    while (**end != '\0' && **end != ',') ++*end;
    *str = *end + (**end == ',');
    rskip_spaces((char**)end, (char*)*start);
}