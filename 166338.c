static OPJ_BOOL opj_j2k_read_ppt (  opj_j2k_t *p_j2k,
                                    OPJ_BYTE * p_header_data,
                                    OPJ_UINT32 p_header_size,
                                    opj_event_mgr_t * p_manager
                                    )
{
        opj_cp_t *l_cp = 00;
        opj_tcp_t *l_tcp = 00;
        OPJ_UINT32 l_Z_ppt;

        /* preconditions */
        assert(p_header_data != 00);
        assert(p_j2k != 00);
        assert(p_manager != 00);

        /* We need to have the Z_ppt element at minimum */
        if (p_header_size < 1) {
                opj_event_msg(p_manager, EVT_ERROR, "Error reading PPT marker\n");
                return OPJ_FALSE;
        }

        l_cp = &(p_j2k->m_cp);
        if (l_cp->ppm){
                opj_event_msg(p_manager, EVT_ERROR, "Error reading PPT marker: packet header have been previously found in the main header (PPM marker).\n");
                return OPJ_FALSE;
        }

        l_tcp = &(l_cp->tcps[p_j2k->m_current_tile_number]);
        l_tcp->ppt = 1;

        opj_read_bytes(p_header_data,&l_Z_ppt,1);               /* Z_ppt */
        ++p_header_data;
        --p_header_size;

        /* Allocate buffer to read the packet header */
        if (l_Z_ppt == 0) {
                /* First PPT marker */
                l_tcp->ppt_data_size = 0;
                l_tcp->ppt_len = p_header_size;

                opj_free(l_tcp->ppt_buffer);
                l_tcp->ppt_buffer = (OPJ_BYTE *) opj_calloc(l_tcp->ppt_len, sizeof(OPJ_BYTE) );
                if (l_tcp->ppt_buffer == 00) {
                        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read PPT marker\n");
                        return OPJ_FALSE;
                }
                l_tcp->ppt_data = l_tcp->ppt_buffer;

                /* memset(l_tcp->ppt_buffer,0,l_tcp->ppt_len); */
        }
        else {
                OPJ_BYTE *new_ppt_buffer;
                l_tcp->ppt_len += p_header_size;

                new_ppt_buffer = (OPJ_BYTE *) opj_realloc(l_tcp->ppt_buffer, l_tcp->ppt_len);
                if (! new_ppt_buffer) {
                        opj_free(l_tcp->ppt_buffer);
                        l_tcp->ppt_buffer = NULL;
                        l_tcp->ppt_len = 0;
                        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read PPT marker\n");
                        return OPJ_FALSE;
                }
                l_tcp->ppt_buffer = new_ppt_buffer;
                l_tcp->ppt_data = l_tcp->ppt_buffer;

                memset(l_tcp->ppt_buffer+l_tcp->ppt_data_size,0,p_header_size);
        }

        /* Read packet header from buffer */
        memcpy(l_tcp->ppt_buffer+l_tcp->ppt_data_size,p_header_data,p_header_size);

        l_tcp->ppt_data_size += p_header_size;

        return OPJ_TRUE;
}