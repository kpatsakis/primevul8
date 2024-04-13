void ass_process_data(ASS_Track *track, char *data, int size)
{
    char *str = malloc(size + 1);
    if (!str)
        return;

    memcpy(str, data, size);
    str[size] = '\0';

    ass_msg(track->library, MSGL_V, "Event: %s", str);
    process_text(track, str);
    free(str);
}