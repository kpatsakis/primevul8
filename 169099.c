static int process_event_tail(ASS_Track *track, ASS_Event *event,
                              char *str, int n_ignored)
{
    char *token;
    char *tname;
    char *p = str;
    int i;
    ASS_Event *target = event;

    char *format = strdup(track->event_format);
    if (!format)
        return -1;
    char *q = format;           // format scanning pointer

    for (i = 0; i < n_ignored; ++i) {
        NEXT(q, tname);
    }

    while (1) {
        NEXT(q, tname);
        if (ass_strcasecmp(tname, "Text") == 0) {
            char *last;
            event->Text = strdup(p);
            if (event->Text && *event->Text != 0) {
                last = event->Text + strlen(event->Text) - 1;
                if (last >= event->Text && *last == '\r')
                    *last = 0;
            }
            event->Duration -= event->Start;
            free(format);
            return event->Text ? 0 : -1;           // "Text" is always the last
        }
        NEXT(p, token);

        ALIAS(End, Duration)    // temporarily store end timecode in event->Duration
        PARSE_START
            INTVAL(Layer)
            STYLEVAL(Style)
            STRVAL(Name)
            STRVAL(Effect)
            INTVAL(MarginL)
            INTVAL(MarginR)
            INTVAL(MarginV)
            TIMEVAL(Start)
            TIMEVAL(Duration)
        PARSE_END
    }
    free(format);
    return 1;
}