
uint32_t cli_rawaddr(uint32_t rva, const struct cli_exe_section *shp, uint16_t nos, unsigned int *err, size_t fsize, uint32_t hdr_size)
{
    int i, found = 0;
    uint32_t ret;

    if (rva<hdr_size) { /* Out of section EP - mapped to imagebase+rva */
	if (rva >= fsize) {
	    *err=1;
	    return 0;
	}
        *err=0;
	return rva;
    }

    for(i = nos-1; i >= 0; i--) {
        if(shp[i].rsz && shp[i].rva <= rva && shp[i].rsz > rva - shp[i].rva) {
	    found = 1;
	    break;
	}
    }

    if(!found) {
	*err = 1;
	return 0;
    }

    ret = rva - shp[i].rva + shp[i].raw;
    *err = 0;
    return ret;