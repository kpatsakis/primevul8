void OPJ_CALLCONV opj_image_destroy(opj_image_t *image)
{
    if (image) {
        if (image->comps) {
            OPJ_UINT32 compno;

            /* image components */
            for (compno = 0; compno < image->numcomps; compno++) {
                opj_image_comp_t *image_comp = &(image->comps[compno]);
                if (image_comp->data) {
                    opj_free(image_comp->data);
                }
            }
            opj_free(image->comps);
        }

        if (image->icc_profile_buf) {
            opj_free(image->icc_profile_buf);
        }

        opj_free(image);
    }
}