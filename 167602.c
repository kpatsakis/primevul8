
static int versioninfo_cb(void *opaque, uint32_t type, uint32_t name, uint32_t lang, uint32_t rva) {
    struct vinfo_list *vlist = (struct vinfo_list *)opaque;

    cli_dbgmsg("versioninfo_cb: type: %x, name: %x, lang: %x, rva: %x\n", type, name, lang, rva);
    vlist->rvas[vlist->count] = rva;
    if(++vlist->count == sizeof(vlist->rvas) / sizeof(vlist->rvas[0]))
	return 1;
    return 0;