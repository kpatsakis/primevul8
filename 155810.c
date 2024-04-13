opj_image_t* OPJ_CALLCONV opj_image_tile_create(OPJ_UINT32 numcmpts,
        opj_image_cmptparm_t *cmptparms, OPJ_COLOR_SPACE clrspc)
{
    OPJ_UINT32 compno;
    opj_image_t *image = 00;

    image = (opj_image_t*) opj_calloc(1, sizeof(opj_image_t));
    if (image) {

        image->color_space = clrspc;
        image->numcomps = numcmpts;

        /* allocate memory for the per-component information */
        image->comps = (opj_image_comp_t*)opj_calloc(image->numcomps,
                       sizeof(opj_image_comp_t));
        if (!image->comps) {
            opj_image_destroy(image);
            return 00;
        }

        /* create the individual image components */
        for (compno = 0; compno < numcmpts; compno++) {
            opj_image_comp_t *comp = &image->comps[compno];
            comp->dx = cmptparms[compno].dx;
            comp->dy = cmptparms[compno].dy;
            comp->w = cmptparms[compno].w;
            comp->h = cmptparms[compno].h;
            comp->x0 = cmptparms[compno].x0;
            comp->y0 = cmptparms[compno].y0;
            comp->prec = cmptparms[compno].prec;
            comp->sgnd = cmptparms[compno].sgnd;
            comp->data = 0;
        }
    }

    return image;
}