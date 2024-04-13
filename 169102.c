static bool format_line_compare(const char *fmt1, const char *fmt2)
{
    while (true) {
        const char *tk1_start, *tk2_start;
        const char *tk1_end, *tk2_end;

        skip_spaces((char**)&fmt1);
        skip_spaces((char**)&fmt2);
        if (!*fmt1 || !*fmt2)
            break;

        advance_token_pos(&fmt1, &tk1_start, &tk1_end);
        advance_token_pos(&fmt2, &tk2_start, &tk2_end);

        if ((tk1_end-tk1_start) != (tk2_end-tk2_start))
            return false;
        if (ass_strncasecmp(tk1_start, tk2_start, tk1_end-tk1_start))
            return false;
    }
    return *fmt1 == *fmt2;
}