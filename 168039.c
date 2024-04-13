_host_id_read_timestamp(gboolean      use_secret_key_file,
                        const guint8 *host_id,
                        gsize         host_id_len,
                        gint64 *      out_timestamp_ns)
{
    struct stat st;
    gint64      now;
    guint64     v;

    if (use_secret_key_file && stat(SECRET_KEY_FILE, &st) == 0) {
        /* don't check for overflow or timestamps in the future. We get whatever
         * (bogus) date is on the file. */
        *out_timestamp_ns = nm_utils_timespec_to_nsec(&st.st_mtim);
        return TRUE;
    }

    /* generate a fake timestamp based on the host-id.
     *
     * This really should never happen under normal circumstances. We already
     * are in a code path, where the system has a problem (unable to get good randomness
     * and/or can't access the secret_key). In such a scenario, a fake timestamp is the
     * least of our problems.
     *
     * At least, generate something sensible so we don't have to worry about the
     * timestamp. It is wrong to worry about using a fake timestamp (which is tied to
     * the secret_key) if we are unable to access the secret_key file in the first place.
     *
     * Pick a random timestamp from the past two years. Yes, this timestamp
     * is not stable across restarts, but apparently neither is the host-id
     * nor the secret_key itself. */

#define EPOCH_TWO_YEARS (G_GINT64_CONSTANT(2 * 365 * 24 * 3600) * NM_UTILS_NSEC_PER_SEC)

    v = nm_hash_siphash42(1156657133u, host_id, host_id_len);

    now = time(NULL);
    *out_timestamp_ns =
        NM_MAX((gint64) 1,
               (now * NM_UTILS_NSEC_PER_SEC) - ((gint64)(v % ((guint64)(EPOCH_TWO_YEARS)))));
    return FALSE;
}