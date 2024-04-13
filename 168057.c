nm_utils_create_dhcp_iaid(gboolean      legacy_unstable_byteorder,
                          const guint8 *interface_id,
                          gsize         interface_id_len)
{
    guint64 u64;
    guint32 u32;

    u64 = c_siphash_hash(HASH_KEY, interface_id, interface_id_len);
    u32 = (u64 & 0xffffffffu) ^ (u64 >> 32);
    if (legacy_unstable_byteorder) {
        /* legacy systemd code dhcp_identifier_set_iaid() generates the iaid
         * dependent on the host endianness. Since this function returns the IAID
         * in native-byte order, we need to account for that.
         *
         * On little endian systems, we want the legacy-behavior is identical to
         * the endianness-agnostic behavior. So, we need to swap the bytes on
         * big-endian systems.
         *
         * (https://github.com/systemd/systemd/pull/10614). */
        return htole32(u32);
    } else {
        /* we return the value as-is, in native byte order. */
        return u32;
    }
}