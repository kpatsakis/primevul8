nm_utils_stable_id_generated_complete(const char *stable_id_generated)
{
    nm_auto_free_checksum GChecksum *sum = NULL;
    guint8                           buf[NM_UTILS_CHECKSUM_LENGTH_SHA1];
    char *                           base64;

    /* for NM_UTILS_STABLE_TYPE_GENERATED we generate a possibly long string
     * by doing text-substitutions in nm_utils_stable_id_parse().
     *
     * Let's shorten the (possibly) long stable_id to something more compact. */

    g_return_val_if_fail(stable_id_generated, NULL);

    sum = g_checksum_new(G_CHECKSUM_SHA1);
    g_checksum_update(sum, (guchar *) stable_id_generated, strlen(stable_id_generated));
    nm_utils_checksum_get_digest(sum, buf);

    /* we don't care to use the sha1 sum in common hex representation.
     * Use instead base64, it's 27 chars (stripping the padding) vs.
     * 40. */

    base64 = g_base64_encode((guchar *) buf, sizeof(buf));
    nm_assert(strlen(base64) == 28);
    nm_assert(base64[27] == '=');

    base64[27] = '\0';
    return base64;
}