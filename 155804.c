void opj_copy_image_header(const opj_image_t* p_image_src,
                           opj_image_t* p_image_dest)
{
    OPJ_UINT32 compno;

    /* preconditions */
    assert(p_image_src != 00);
    assert(p_image_dest != 00);

    p_image_dest->x0 = p_image_src->x0;
    p_image_dest->y0 = p_image_src->y0;
    p_image_dest->x1 = p_image_src->x1;
    p_image_dest->y1 = p_image_src->y1;

    if (p_image_dest->comps) {
        for (compno = 0; compno < p_image_dest->numcomps; compno++) {
            opj_image_comp_t *image_comp = &(p_image_dest->comps[compno]);
            if (image_comp->data) {
                opj_free(image_comp->data);
            }
        }
        opj_free(p_image_dest->comps);
        p_image_dest->comps = NULL;
    }

    p_image_dest->numcomps = p_image_src->numcomps;

    p_image_dest->comps = (opj_image_comp_t*) opj_malloc(p_image_dest->numcomps *
                          sizeof(opj_image_comp_t));
    if (!p_image_dest->comps) {
        p_image_dest->comps = NULL;
        p_image_dest->numcomps = 0;
        return;
    }

    for (compno = 0; compno < p_image_dest->numcomps; compno++) {
        memcpy(&(p_image_dest->comps[compno]),
               &(p_image_src->comps[compno]),
               sizeof(opj_image_comp_t));
        p_image_dest->comps[compno].data = NULL;
    }

    p_image_dest->color_space = p_image_src->color_space;
    p_image_dest->icc_profile_len = p_image_src->icc_profile_len;

    if (p_image_dest->icc_profile_len) {
        p_image_dest->icc_profile_buf = (OPJ_BYTE*)opj_malloc(
                                            p_image_dest->icc_profile_len);
        if (!p_image_dest->icc_profile_buf) {
            p_image_dest->icc_profile_buf = NULL;
            p_image_dest->icc_profile_len = 0;
            return;
        }
        memcpy(p_image_dest->icc_profile_buf,
               p_image_src->icc_profile_buf,
               p_image_src->icc_profile_len);
    } else {
        p_image_dest->icc_profile_buf = NULL;
    }

    return;
}