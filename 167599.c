/* check hash section sigs */
static int scan_pe_mdb (cli_ctx * ctx, struct cli_exe_section *exe_section)
{
    struct cli_matcher * mdb_sect = ctx->engine->hm_mdb;
    unsigned char * hashset[CLI_HASH_AVAIL_TYPES];
    const char * virname = NULL;
    int foundsize[CLI_HASH_AVAIL_TYPES];
    int foundwild[CLI_HASH_AVAIL_TYPES];
    enum CLI_HASH_TYPE type;
    int ret = CL_CLEAN;
    unsigned char * md5 = NULL;
 
    /* pick hashtypes to generate */
    for(type = CLI_HASH_MD5; type < CLI_HASH_AVAIL_TYPES; type++) {
        foundsize[type] = cli_hm_have_size(mdb_sect, type, exe_section->rsz);
        foundwild[type] = cli_hm_have_wild(mdb_sect, type);
        if(foundsize[type] || foundwild[type]) {
            hashset[type] = cli_malloc(hashlen[type]);
            if(!hashset[type]) {
                cli_errmsg("scan_pe: cli_malloc failed!\n");
                for(; type > 0;)
                    free(hashset[--type]);
                return CL_EMEM;
            }
        }
        else {
            hashset[type] = NULL;
        }
    }

    /* Generate hashes */
    cli_hashsect(*ctx->fmap, exe_section, hashset, foundsize, foundwild);

    /* Print hash */
    if (cli_debug_flag) {
        md5 = hashset[CLI_HASH_MD5];
        if (md5) {
            cli_dbgmsg("MDB: %u:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                exe_section->rsz, md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7],
                md5[8], md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15]);
        } else if (cli_always_gen_section_hash) {
            const void *hashme = fmap_need_off_once(*ctx->fmap, exe_section->raw, exe_section->rsz);
            if (!(hashme)) {
                cli_errmsg("scan_pe_mdb: unable to read section data\n");
                ret = CL_EREAD;
                goto end;
            }

            md5 = cli_malloc(16);
            if (!(md5)) {
                cli_errmsg("scan_pe_mdb: cli_malloc failed!\n");
                ret = CL_EMEM;
                goto end;
            }

            cl_hash_data("md5", hashme, exe_section->rsz, md5, NULL);

            cli_dbgmsg("MDB: %u:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                exe_section->rsz, md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7],
                md5[8], md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15]);

            free(md5);

        } else {
            cli_dbgmsg("MDB: %u:notgenerated\n", exe_section->rsz);
        }
    }

    /* Do scans */
    for(type = CLI_HASH_MD5; type < CLI_HASH_AVAIL_TYPES; type++) {
       if(foundsize[type] && cli_hm_scan(hashset[type], exe_section->rsz, &virname, mdb_sect, type) == CL_VIRUS) {
            cli_append_virus(ctx, virname);
            ret = CL_VIRUS;
            if (!SCAN_ALL) {
                break;
            }
       }
       if(foundwild[type] && cli_hm_scan_wild(hashset[type], &virname, mdb_sect, type) == CL_VIRUS) {
            cli_append_virus(ctx, virname);
            ret = CL_VIRUS;
            if (!SCAN_ALL) {
                break;
            }
       }
    }

end:
    for(type = CLI_HASH_AVAIL_TYPES; type > 0;)
        free(hashset[--type]);
    return ret;