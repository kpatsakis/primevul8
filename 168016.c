nm_utils_stable_id_parse(const char *stable_id,
                         const char *deviceid,
                         const char *hwaddr,
                         const char *bootid,
                         const char *uuid,
                         char **     out_generated)
{
    gsize    i, idx_start;
    GString *str = NULL;

    g_return_val_if_fail(out_generated, NM_UTILS_STABLE_TYPE_RANDOM);

    if (!stable_id) {
        *out_generated = NULL;
        return NM_UTILS_STABLE_TYPE_UUID;
    }

    /* the stable-id allows for some dynamic by performing text-substitutions
     * of ${...} patterns.
     *
     * At first, it looks a bit like bash parameter substitution.
     * In contrast however, the process is unambiguous so that the resulting
     * effective id differs if:
     *  - the original, untranslated stable-id differs
     *  - or any of the subsitutions differs.
     *
     * The reason for that is, for example if you specify "${CONNECTION}" in the
     * stable-id, then the resulting ID should be always(!) unique for this connection.
     * There should be no way another connection could specify any stable-id that results
     * in the same addresses to be generated (aside hash collisions).
     *
     *
     * For example: say you have a connection with UUID
     * "123e4567-e89b-12d3-a456-426655440000" which happens also to be
     * the current boot-id.
     * Then:
     *   (1) connection.stable-id = <NULL>
     *   (2) connection.stable-id = "123e4567-e89b-12d3-a456-426655440000"
     *   (3) connection.stable-id = "${CONNECTION}"
     *   (3) connection.stable-id = "${BOOT}"
     * will all generate different addresses, although in one way or the
     * other, they all mangle the uuid "123e4567-e89b-12d3-a456-426655440000".
     *
     * For example, with stable-id="${FOO}${BAR}" the substitutions
     *   - FOO="ab", BAR="c"
     *   - FOO="a",  BAR="bc"
     * should give a different effective id.
     *
     * For example, with FOO="x" and BAR="x", the stable-ids
     *   - "${FOO}${BAR}"
     *   - "${BAR}${FOO}"
     * should give a different effective id.
     */

    idx_start = 0;
    for (i = 0; stable_id[i];) {
        if (stable_id[i] != '$') {
            i++;
            continue;
        }

#define CHECK_PREFIX(prefix)                                                  \
    ({                                                                        \
        gboolean _match = FALSE;                                              \
                                                                              \
        if (g_str_has_prefix(&stable_id[i], "" prefix "")) {                  \
            _match = TRUE;                                                    \
            if (!str)                                                         \
                str = g_string_sized_new(256);                                \
            i += NM_STRLEN(prefix);                                           \
            g_string_append_len(str, &(stable_id)[idx_start], i - idx_start); \
            idx_start = i;                                                    \
        }                                                                     \
        _match;                                                               \
    })
        if (CHECK_PREFIX("${CONNECTION}"))
            _stable_id_append(str, uuid);
        else if (CHECK_PREFIX("${BOOT}"))
            _stable_id_append(str, bootid);
        else if (CHECK_PREFIX("${DEVICE}"))
            _stable_id_append(str, deviceid);
        else if (CHECK_PREFIX("${MAC}"))
            _stable_id_append(str, hwaddr);
        else if (g_str_has_prefix(&stable_id[i], "${RANDOM}")) {
            /* RANDOM makes not so much sense for cloned-mac-address
             * as the result is similar to specifying "cloned-mac-address=random".
             * It makes however sense for RFC 7217 Stable Privacy IPv6 addresses
             * where this is effectively the only way to generate a different
             * (random) host identifier for each connect.
             *
             * With RANDOM, the user can switch the lifetime of the
             * generated cloned-mac-address and IPv6 host identifier
             * by toggling only the stable-id property of the connection.
             * With RANDOM being the most short-lived, ~non-stable~ variant.
             */
            if (str)
                g_string_free(str, TRUE);
            *out_generated = NULL;
            return NM_UTILS_STABLE_TYPE_RANDOM;
        } else {
            /* The text following the '$' is not recognized as valid
             * substitution pattern. Treat it verbatim. */
            i++;

            /* Note that using unrecognized substitution patterns might
             * yield different results with future versions. Avoid that,
             * by not using '$' (except for actual substitutions) or escape
             * it as "$$" (which is guaranteed to be treated verbatim
             * in future). */
            if (stable_id[i] == '$')
                i++;
        }
    }
#undef CHECK_PREFIX

    if (!str) {
        *out_generated = NULL;
        return NM_UTILS_STABLE_TYPE_STABLE_ID;
    }

    if (idx_start < i)
        g_string_append_len(str, &stable_id[idx_start], i - idx_start);
    *out_generated = g_string_free(str, FALSE);
    return NM_UTILS_STABLE_TYPE_GENERATED;
}