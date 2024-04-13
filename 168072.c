nm_match_spec_config(const GSList *specs, guint cur_nm_version, const char *env)
{
    const GSList *iter;
    gboolean      has_match        = FALSE;
    gboolean      has_match_except = FALSE;
    gboolean      has_except       = FALSE;
    gboolean      has_not_except   = FALSE;

    if (!specs)
        return NM_MATCH_SPEC_NO_MATCH;

    for (iter = specs; iter; iter = g_slist_next(iter)) {
        const char *spec_str = iter->data;
        gboolean    except;
        gboolean    v_match;

        if (!spec_str || !*spec_str)
            continue;

        spec_str = match_except(spec_str, &except);

        if (except)
            has_except = TRUE;
        else
            has_not_except = TRUE;

        if ((except && has_match_except) || (!except && has_match)) {
            /* evaluating the match does not give new information. Skip it. */
            continue;
        }

        if (_MATCH_CHECK(spec_str, MATCH_TAG_CONFIG_NM_VERSION))
            v_match = match_config_eval(spec_str, MATCH_TAG_CONFIG_NM_VERSION, cur_nm_version);
        else if (_MATCH_CHECK(spec_str, MATCH_TAG_CONFIG_NM_VERSION_MIN))
            v_match = match_config_eval(spec_str, MATCH_TAG_CONFIG_NM_VERSION_MIN, cur_nm_version);
        else if (_MATCH_CHECK(spec_str, MATCH_TAG_CONFIG_NM_VERSION_MAX))
            v_match = match_config_eval(spec_str, MATCH_TAG_CONFIG_NM_VERSION_MAX, cur_nm_version);
        else if (_MATCH_CHECK(spec_str, MATCH_TAG_CONFIG_ENV))
            v_match = env && env[0] && !strcmp(spec_str, env);
        else
            v_match = FALSE;

        if (!v_match)
            continue;

        if (except)
            has_match_except = TRUE;
        else
            has_match = TRUE;
    }

    return _match_result(has_except, has_not_except, has_match, has_match_except);
}