match_device_hwaddr_eval(const char *spec_str, MatchDeviceData *match_data)
{
    if (G_UNLIKELY(!match_data->hwaddr.is_parsed)) {
        match_data->hwaddr.is_parsed = TRUE;

        if (match_data->hwaddr.value) {
            gsize l;

            if (!_nm_utils_hwaddr_aton(match_data->hwaddr.value,
                                       match_data->hwaddr.bin,
                                       sizeof(match_data->hwaddr.bin),
                                       &l))
                g_return_val_if_reached(FALSE);
            match_data->hwaddr.len = l;
        } else
            return FALSE;
    } else if (!match_data->hwaddr.len)
        return FALSE;

    return nm_utils_hwaddr_matches(spec_str, -1, match_data->hwaddr.bin, match_data->hwaddr.len);
}