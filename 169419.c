kex_agree_instr(unsigned char *haystack, unsigned long haystack_len,
                const unsigned char *needle, unsigned long needle_len)
{
    unsigned char *s;

    /* Haystack too short to bother trying */
    if (haystack_len < needle_len) {
        return NULL;
    }

    /* Needle at start of haystack */
    if ((strncmp((char *) haystack, (char *) needle, needle_len) == 0) &&
        (needle_len == haystack_len || haystack[needle_len] == ',')) {
        return haystack;
    }

    s = haystack;
    /* Search until we run out of comas or we run out of haystack,
       whichever comes first */
    while ((s = (unsigned char *) strchr((char *) s, ','))
           && ((haystack_len - (s - haystack)) > needle_len)) {
        s++;
        /* Needle at X position */
        if ((strncmp((char *) s, (char *) needle, needle_len) == 0) &&
            (((s - haystack) + needle_len) == haystack_len
             || s[needle_len] == ',')) {
            return s;
        }
    }

    return NULL;
}