match_data_s390_subchannels_eval(const char *spec_str, MatchDeviceData *match_data)
{
    guint32 a, b, c;

    if (G_UNLIKELY(!match_data->s390_subchannels.is_parsed)) {
        match_data->s390_subchannels.is_parsed = TRUE;

        if (!match_data->s390_subchannels.value
            || !match_device_s390_subchannels_parse(match_data->s390_subchannels.value,
                                                    &match_data->s390_subchannels.a,
                                                    &match_data->s390_subchannels.b,
                                                    &match_data->s390_subchannels.c)) {
            match_data->s390_subchannels.value = NULL;
            return FALSE;
        }
    } else if (!match_data->s390_subchannels.value)
        return FALSE;

    if (!match_device_s390_subchannels_parse(spec_str, &a, &b, &c))
        return FALSE;
    return match_data->s390_subchannels.a == a && match_data->s390_subchannels.b == b
           && match_data->s390_subchannels.c == c;
}