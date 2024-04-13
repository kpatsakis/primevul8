
static unsigned int cli_hashsect(fmap_t *map, struct cli_exe_section *s, unsigned char **digest, int * foundhash, int * foundwild)
{
    const void *hashme;

    if (s->rsz > CLI_MAX_ALLOCATION) {
        cli_dbgmsg("cli_hashsect: skipping hash calculation for too big section\n");
        return 0;
    }

    if(!s->rsz) return 0;
    if(!(hashme=fmap_need_off_once(map, s->raw, s->rsz))) {
        cli_dbgmsg("cli_hashsect: unable to read section data\n");
        return 0;
    }

    if(foundhash[CLI_HASH_MD5] || foundwild[CLI_HASH_MD5])
        cl_hash_data("md5", hashme, s->rsz, digest[CLI_HASH_MD5], NULL);
    if(foundhash[CLI_HASH_SHA1] || foundwild[CLI_HASH_SHA1])
        cl_sha1(hashme, s->rsz, digest[CLI_HASH_SHA1], NULL);
    if(foundhash[CLI_HASH_SHA256] || foundwild[CLI_HASH_SHA256])
        cl_sha256(hashme, s->rsz, digest[CLI_HASH_SHA256], NULL);

    return 1;