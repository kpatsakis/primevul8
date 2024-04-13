
int cli_checkfp_pe(cli_ctx *ctx, uint8_t *authsha1, stats_section_t *hashes, uint32_t flags) {
    uint16_t e_magic; /* DOS signature ("MZ") */
    uint16_t nsections;
    uint32_t e_lfanew; /* address of new exe header */
    struct pe_image_file_hdr file_hdr;
    union {
        struct pe_image_optional_hdr64 opt64;
        struct pe_image_optional_hdr32 opt32;
    } pe_opt;
    const struct pe_image_section_hdr *section_hdr;
    ssize_t at;
    unsigned int i, pe_plus = 0, hlen;
    size_t fsize;
    uint32_t valign, falign, hdr_size;
    struct cli_exe_section *exe_sections;
    struct pe_image_data_dir *dirs;
    fmap_t *map = *ctx->fmap;
    void *hashctx=NULL;

    if (flags & CL_CHECKFP_PE_FLAG_STATS)
        if (!(hashes))
            return CL_EFORMAT;

    if (flags == CL_CHECKFP_PE_FLAG_NONE)
        return CL_VIRUS;

    if(!(DCONF & PE_CONF_CATALOG))
        return CL_EFORMAT;

    if(fmap_readn(map, &e_magic, 0, sizeof(e_magic)) != sizeof(e_magic))
        return CL_EFORMAT;

    if(EC16(e_magic) != PE_IMAGE_DOS_SIGNATURE && EC16(e_magic) != PE_IMAGE_DOS_SIGNATURE_OLD)
        return CL_EFORMAT;

    if(fmap_readn(map, &e_lfanew, 58 + sizeof(e_magic), sizeof(e_lfanew)) != sizeof(e_lfanew))
        return CL_EFORMAT;

    e_lfanew = EC32(e_lfanew);
    if(!e_lfanew)
        return CL_EFORMAT;

    if(fmap_readn(map, &file_hdr, e_lfanew, sizeof(struct pe_image_file_hdr)) != sizeof(struct pe_image_file_hdr))
        return CL_EFORMAT;

    if(EC32(file_hdr.Magic) != PE_IMAGE_NT_SIGNATURE)
        return CL_EFORMAT;

    nsections = EC16(file_hdr.NumberOfSections);
    if(nsections < 1 || nsections > 96)
        return CL_EFORMAT;

    if(EC16(file_hdr.SizeOfOptionalHeader) < sizeof(struct pe_image_optional_hdr32))
        return CL_EFORMAT;

    at = e_lfanew + sizeof(struct pe_image_file_hdr);
    if(fmap_readn(map, &optional_hdr32, at, sizeof(struct pe_image_optional_hdr32)) != sizeof(struct pe_image_optional_hdr32))
        return CL_EFORMAT;

    at += sizeof(struct pe_image_optional_hdr32);

    /* This will be a chicken and egg problem until we drop 9x */
    if(EC16(optional_hdr64.Magic)==PE32P_SIGNATURE) {
        if(EC16(file_hdr.SizeOfOptionalHeader)!=sizeof(struct pe_image_optional_hdr64))
            return CL_EFORMAT;

        pe_plus = 1;
    }

    if(!pe_plus) { /* PE */
        if (EC16(file_hdr.SizeOfOptionalHeader)!=sizeof(struct pe_image_optional_hdr32)) {
            /* Seek to the end of the long header */
            at += EC16(file_hdr.SizeOfOptionalHeader)-sizeof(struct pe_image_optional_hdr32);
        }

        hdr_size = EC32(optional_hdr32.SizeOfHeaders);
        dirs = optional_hdr32.DataDirectory;
    } else { /* PE+ */
        size_t readlen = sizeof(struct pe_image_optional_hdr64) - sizeof(struct pe_image_optional_hdr32);
        /* read the remaining part of the header */
        if((size_t)fmap_readn(map, &optional_hdr32 + 1, at, readlen) != readlen)
            return CL_EFORMAT;

        at += sizeof(struct pe_image_optional_hdr64) - sizeof(struct pe_image_optional_hdr32);
        hdr_size = EC32(optional_hdr64.SizeOfHeaders);
        dirs = optional_hdr64.DataDirectory;
    }

    fsize = map->len;

    valign = (pe_plus)?EC32(optional_hdr64.SectionAlignment):EC32(optional_hdr32.SectionAlignment);
    falign = (pe_plus)?EC32(optional_hdr64.FileAlignment):EC32(optional_hdr32.FileAlignment);

    section_hdr = fmap_need_off_once(map, at, sizeof(*section_hdr) * nsections);
    if(!section_hdr)
        return CL_EFORMAT;

    at += sizeof(*section_hdr) * nsections;

    exe_sections = (struct cli_exe_section *) cli_calloc(nsections, sizeof(struct cli_exe_section));
    if(!exe_sections)
        return CL_EMEM;

    for(i = 0; falign!=0x200 && i<nsections; i++) {
        /* file alignment fallback mode - blah */
        if (falign && section_hdr[i].SizeOfRawData && EC32(section_hdr[i].PointerToRawData)%falign && !(EC32(section_hdr[i].PointerToRawData)%0x200))
            falign = 0x200;
    }

    hdr_size = PESALIGN(hdr_size, falign); /* Aligned headers virtual size */

    if (flags & CL_CHECKFP_PE_FLAG_STATS) {
        hashes->nsections = nsections;
        hashes->sections = cli_calloc(nsections, sizeof(struct cli_section_hash));
        if (!(hashes->sections)) {
            free(exe_sections);
            return CL_EMEM;
        }
    }

    for(i = 0; i < nsections; i++) {
        exe_sections[i].rva = PEALIGN(EC32(section_hdr[i].VirtualAddress), valign);
        exe_sections[i].vsz = PESALIGN(EC32(section_hdr[i].VirtualSize), valign);
        exe_sections[i].raw = PEALIGN(EC32(section_hdr[i].PointerToRawData), falign);
        exe_sections[i].rsz = PESALIGN(EC32(section_hdr[i].SizeOfRawData), falign);

        if (!exe_sections[i].vsz && exe_sections[i].rsz)
            exe_sections[i].vsz=PESALIGN(exe_sections[i].ursz, valign);

        if (exe_sections[i].rsz && fsize>exe_sections[i].raw && !CLI_ISCONTAINED(0, (uint32_t) fsize, exe_sections[i].raw, exe_sections[i].rsz))
            exe_sections[i].rsz = fsize - exe_sections[i].raw;

        if (exe_sections[i].rsz && exe_sections[i].raw >= fsize) {
            free(exe_sections);
            return CL_EFORMAT;
        }

        if (exe_sections[i].urva>>31 || exe_sections[i].uvsz>>31 || (exe_sections[i].rsz && exe_sections[i].uraw>>31) || exe_sections[i].ursz>>31) {
            free(exe_sections);
            return CL_EFORMAT;
        }
    }

    cli_qsort(exe_sections, nsections, sizeof(*exe_sections), sort_sects);
    hashctx = cl_hash_init("sha1");
    if (!(hashctx)) {
        if (flags & CL_CHECKFP_PE_FLAG_AUTHENTICODE)
            flags ^= CL_CHECKFP_PE_FLAG_AUTHENTICODE;
    }

    if (flags & CL_CHECKFP_PE_FLAG_AUTHENTICODE) {
        /* Check to see if we have a security section. */
        if(!cli_hm_have_size(ctx->engine->hm_fp, CLI_HASH_SHA1, 2) && dirs[4].Size < 8) {
            if (flags & CL_CHECKFP_PE_FLAG_STATS) {
                /* If stats is enabled, continue parsing the sample */
                flags ^= CL_CHECKFP_PE_FLAG_AUTHENTICODE;
            } else {
                if (hashctx)
                    cl_hash_destroy(hashctx);
                return CL_BREAK;
            }
        }
    }

#define hash_chunk(where, size, isStatAble, section) \
    do { \
        const uint8_t *hptr; \
        if(!(size)) break; \
        if(!(hptr = fmap_need_off_once(map, where, size))){ \
            free(exe_sections); \
            if (hashctx) \
                cl_hash_destroy(hashctx); \
            return CL_EFORMAT; \
        } \
        if (flags & CL_CHECKFP_PE_FLAG_AUTHENTICODE && hashctx) \
            cl_update_hash(hashctx, (void *)hptr, size); \
        if (isStatAble && flags & CL_CHECKFP_PE_FLAG_STATS) { \
            void *md5ctx; \
            md5ctx = cl_hash_init("md5"); \
            if (md5ctx) { \
                cl_update_hash(md5ctx, (void *)hptr, size); \
                cl_finish_hash(md5ctx, hashes->sections[section].md5); \
            } \
        } \
    } while(0)

    while (flags & CL_CHECKFP_PE_FLAG_AUTHENTICODE) {
        /* MZ to checksum */
        at = 0;
        hlen = e_lfanew + sizeof(struct pe_image_file_hdr) + (pe_plus ? offsetof(struct pe_image_optional_hdr64, CheckSum) : offsetof(struct pe_image_optional_hdr32, CheckSum));
        hash_chunk(0, hlen, 0, 0);
        at = hlen + 4;

        /* Checksum to security */
        if(pe_plus)
            hlen = offsetof(struct pe_image_optional_hdr64, DataDirectory[4]) - offsetof(struct pe_image_optional_hdr64, CheckSum) - 4;
        else
            hlen = offsetof(struct pe_image_optional_hdr32, DataDirectory[4]) - offsetof(struct pe_image_optional_hdr32, CheckSum) - 4;
        hash_chunk(at, hlen, 0, 0);
        at += hlen + 8;

        if(at > hdr_size) {
            if (flags & CL_CHECKFP_PE_FLAG_STATS) {
                flags ^= CL_CHECKFP_PE_FLAG_AUTHENTICODE;
                break;
            } else {
                free(exe_sections);
                if (hashctx)
                    cl_hash_destroy(hashctx);
                return CL_EFORMAT;
            }
        }

        /* Security to End of header */
        hlen = hdr_size - at;
        hash_chunk(at, hlen, 0, 0);

        at = hdr_size;
        break;
    }

    /* Hash the sections */
    for(i = 0; i < nsections; i++) {
        if(!exe_sections[i].rsz)
            continue;

        hash_chunk(exe_sections[i].raw, exe_sections[i].rsz, 1, i);
        if (flags & CL_CHECKFP_PE_FLAG_AUTHENTICODE)
            at += exe_sections[i].rsz;
    }

    while (flags & CL_CHECKFP_PE_FLAG_AUTHENTICODE) {
        if((size_t)at < fsize) {
            hlen = fsize - at;
            if(dirs[4].Size > hlen) {
                if (flags & CL_CHECKFP_PE_FLAG_STATS) {
                    flags ^= CL_CHECKFP_PE_FLAG_AUTHENTICODE;
                    break;
                } else {
                    free(exe_sections);
                    if (hashctx)
                        cl_hash_destroy(hashctx);
                    return CL_EFORMAT;
                }
            }

            hlen -= dirs[4].Size;
            hash_chunk(at, hlen, 0, 0);
            at += hlen;
        }

        break;
    } while (0);

    free(exe_sections);

    if (flags & CL_CHECKFP_PE_FLAG_AUTHENTICODE && hashctx) {
        cl_finish_hash(hashctx, authsha1);

        if(cli_debug_flag) {
            char shatxt[SHA1_HASH_SIZE*2+1];
            for(i=0; i<SHA1_HASH_SIZE; i++)
                sprintf(&shatxt[i*2], "%02x", authsha1[i]);
            cli_dbgmsg("Authenticode: %s\n", shatxt);
        }

        hlen = dirs[4].Size;
        if(hlen < 8)
            return CL_VIRUS;

        hlen -= 8;

        return asn1_check_mscat((struct cl_engine *)(ctx->engine), map, at + 8, hlen, authsha1);
    } else {
        if (hashctx)
            cl_hash_destroy(hashctx);
        return CL_VIRUS;
    }