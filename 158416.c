OPJ_BOOL opj_tcd_update_tile_data ( opj_tcd_t *p_tcd,
                                    OPJ_BYTE * p_dest,
                                    OPJ_UINT32 p_dest_length
                                    )
{
        OPJ_UINT32 i,j,k,l_data_size = 0;
        opj_image_comp_t * l_img_comp = 00;
        opj_tcd_tilecomp_t * l_tilec = 00;
        opj_tcd_resolution_t * l_res;
        OPJ_UINT32 l_size_comp, l_remaining;
        OPJ_UINT32 l_stride, l_width,l_height;

        l_data_size = opj_tcd_get_decoded_tile_size(p_tcd);
        if (l_data_size > p_dest_length) {
                return OPJ_FALSE;
        }

        l_tilec = p_tcd->tcd_image->tiles->comps;
        l_img_comp = p_tcd->image->comps;

        for (i=0;i<p_tcd->image->numcomps;++i) {
                l_size_comp = l_img_comp->prec >> 3; /*(/ 8)*/
                l_remaining = l_img_comp->prec & 7;  /* (%8) */
                l_res = l_tilec->resolutions + l_img_comp->resno_decoded;
                l_width = (OPJ_UINT32)(l_res->x1 - l_res->x0);
                l_height = (OPJ_UINT32)(l_res->y1 - l_res->y0);
                l_stride = (OPJ_UINT32)(l_tilec->x1 - l_tilec->x0) - l_width;

                if (l_remaining) {
                        ++l_size_comp;
                }

                if (l_size_comp == 3) {
                        l_size_comp = 4;
                }

                switch (l_size_comp)
                        {
                        case 1:
                                {
                                        OPJ_CHAR * l_dest_ptr = (OPJ_CHAR *) p_dest;
                                        const OPJ_INT32 * l_src_ptr = l_tilec->data;

                                        if (l_img_comp->sgnd) {
                                                for (j=0;j<l_height;++j) {
                                                        for (k=0;k<l_width;++k) {
                                                                *(l_dest_ptr++) = (OPJ_CHAR) (*(l_src_ptr++));
                                                        }
                                                        l_src_ptr += l_stride;
                                                }
                                        }
                                        else {
                                                for (j=0;j<l_height;++j) {
                                                        for     (k=0;k<l_width;++k) {
                                                                *(l_dest_ptr++) = (OPJ_CHAR) ((*(l_src_ptr++))&0xff);
                                                        }
                                                        l_src_ptr += l_stride;
                                                }
                                        }

                                        p_dest = (OPJ_BYTE *)l_dest_ptr;
                                }
                                break;
                        case 2:
                                {
                                        const OPJ_INT32 * l_src_ptr = l_tilec->data;
                                        OPJ_INT16 * l_dest_ptr = (OPJ_INT16 *) p_dest;

                                        if (l_img_comp->sgnd) {
                                                for (j=0;j<l_height;++j) {
                                                        for (k=0;k<l_width;++k) {
                                                                *(l_dest_ptr++) = (OPJ_INT16) (*(l_src_ptr++));
                                                        }
                                                        l_src_ptr += l_stride;
                                                }
                                        }
                                        else {
                                                for (j=0;j<l_height;++j) {
                                                        for (k=0;k<l_width;++k) {
                                                                *(l_dest_ptr++) = (OPJ_INT16) ((*(l_src_ptr++))&0xffff);
                                                        }
                                                        l_src_ptr += l_stride;
                                                }
                                        }

                                        p_dest = (OPJ_BYTE*) l_dest_ptr;
                                }
                                break;
                        case 4:
                                {
                                        OPJ_INT32 * l_dest_ptr = (OPJ_INT32 *) p_dest;
                                        OPJ_INT32 * l_src_ptr = l_tilec->data;

                                        for (j=0;j<l_height;++j) {
                                                for (k=0;k<l_width;++k) {
                                                        *(l_dest_ptr++) = (*(l_src_ptr++));
                                                }
                                                l_src_ptr += l_stride;
                                        }

                                        p_dest = (OPJ_BYTE*) l_dest_ptr;
                                }
                                break;
                }

                ++l_img_comp;
                ++l_tilec;
        }

        return OPJ_TRUE;
}