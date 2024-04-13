OPJ_BOOL j2k_read_ppm_v2 (
                                                opj_j2k_t *p_j2k,
                                                OPJ_BYTE * p_header_data,
                                                OPJ_UINT32 p_header_size,
                                                struct opj_event_mgr * p_manager
                                        )
{

        opj_cp_t *l_cp = 00;
        OPJ_UINT32 l_remaining_data, l_Z_ppm, l_N_ppm;

        /* preconditions */
        assert(p_header_data != 00);
        assert(p_j2k != 00);
        assert(p_manager != 00);

        if (p_header_size < 1) {
                opj_event_msg(p_manager, EVT_ERROR, "Error reading PPM marker\n");
                return OPJ_FALSE;
        }

        l_cp = &(p_j2k->m_cp);
        l_cp->ppm = 1;

        opj_read_bytes(p_header_data,&l_Z_ppm,1);               /* Z_ppm */
        ++p_header_data;
        --p_header_size;

        /* First PPM marker */
        if (l_Z_ppm == 0) {
                if (p_header_size < 4) {
                        opj_event_msg(p_manager, EVT_ERROR, "Error reading PPM marker\n");
                        return OPJ_FALSE;
                }

                opj_read_bytes(p_header_data,&l_N_ppm,4);               /* N_ppm */
                p_header_data+=4;
                p_header_size-=4;

                /* First PPM marker: Initialization */
                l_cp->ppm_len = l_N_ppm;
                l_cp->ppm_data_size = 0;

                l_cp->ppm_buffer = (OPJ_BYTE *) opj_malloc(l_cp->ppm_len);
                if (l_cp->ppm_buffer == 00) {
                        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory reading ppm marker\n");
                        return OPJ_FALSE;
                }
                memset(l_cp->ppm_buffer,0,l_cp->ppm_len);

                l_cp->ppm_data = l_cp->ppm_buffer;
        }

        while (1) {
                if (l_cp->ppm_data_size == l_cp->ppm_len) {
                        if (p_header_size >= 4) {
                                /* read a N_ppm */
                                opj_read_bytes(p_header_data,&l_N_ppm,4);               /* N_ppm */
                                p_header_data+=4;
                                p_header_size-=4;
                                l_cp->ppm_len += l_N_ppm ;

                                OPJ_BYTE *new_ppm_buffer = (OPJ_BYTE *) opj_realloc(l_cp->ppm_buffer, l_cp->ppm_len);
                                if (! new_ppm_buffer) {
                                        opj_free(l_cp->ppm_buffer);
                                        l_cp->ppm_buffer = NULL;
                                        l_cp->ppm_len = 0;
                                        l_cp->ppm_data = NULL;
                                        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory reading ppm marker\n");
                                        return OPJ_FALSE;
                                }
                                l_cp->ppm_buffer = new_ppm_buffer;
                                memset(l_cp->ppm_buffer+l_cp->ppm_data_size,0,l_N_ppm);
                                l_cp->ppm_data = l_cp->ppm_buffer;
                        }
                        else {
                                return OPJ_FALSE;
                        }
                }

                l_remaining_data = l_cp->ppm_len - l_cp->ppm_data_size;

                if (l_remaining_data <= p_header_size) {
                        /* we must store less information than available in the packet */
                        memcpy(l_cp->ppm_buffer + l_cp->ppm_data_size , p_header_data , l_remaining_data);
                        l_cp->ppm_data_size = l_cp->ppm_len;
                        p_header_size -= l_remaining_data;
                        p_header_data += l_remaining_data;
                }
                else {
                        memcpy(l_cp->ppm_buffer + l_cp->ppm_data_size , p_header_data , p_header_size);
                        l_cp->ppm_data_size += p_header_size;
                        p_header_data += p_header_size;
                        p_header_size = 0;
                        break;
                }
        }

        return OPJ_TRUE;
}