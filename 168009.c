nm_utils_stable_id_random(void)
{
    char buf[15];

    nm_utils_random_bytes(buf, sizeof(buf));
    return g_base64_encode((guchar *) buf, sizeof(buf));
}