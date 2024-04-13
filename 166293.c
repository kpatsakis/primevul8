OPJ_BOOL j2k_read_ppm_v3 (
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

        /* Minimum size of PPM marker is equal to the size of Zppm element */
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
                if (l_cp->ppm_data != NULL) {
                        opj_event_msg(p_manager, EVT_ERROR, "Zppm O already processed. Found twice.\n");
                        opj_free(l_cp->ppm_data);
                        l_cp->ppm_data = NULL;
                        l_cp->ppm_buffer = NULL;
                        l_cp->ppm = 0; /* do not use PPM */
                        return OPJ_FALSE;
                }
                /* We need now at least the Nppm^0 element */
                if (p_header_size < 4) {
                        opj_event_msg(p_manager, EVT_ERROR, "Error reading PPM marker\n");
                        return OPJ_FALSE;
                }

                opj_read_bytes(p_header_data,&l_N_ppm,4);               /* First N_ppm */
                p_header_data+=4;
                p_header_size-=4;

                /* sanity check: how much bytes is left for Ippm */
                if( p_header_size < l_N_ppm )
                  {
                  opj_event_msg(p_manager, EVT_ERROR, "Not enough bytes (%u) to hold Ippm series (%u), Index (%d)\n", p_header_size, l_N_ppm, l_Z_ppm );
                  opj_free(l_cp->ppm_data);
                  l_cp->ppm_data = NULL;
                  l_cp->ppm_buffer = NULL;
                  l_cp->ppm = 0; /* do not use PPM */
                  return OPJ_FALSE;
                  }

                /* First PPM marker: Initialization */
                l_cp->ppm_len = l_N_ppm;
                l_cp->ppm_data_read = 0;

                l_cp->ppm_data = (OPJ_BYTE *) opj_calloc(1,l_cp->ppm_len);
                l_cp->ppm_buffer = l_cp->ppm_data;
                if (l_cp->ppm_data == 00) {
                        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read ppm marker\n");
                        return OPJ_FALSE;
                }

                l_cp->ppm_data_current = l_cp->ppm_data;

                /*l_cp->ppm_data = l_cp->ppm_buffer;*/
        }
        else {
                if (p_header_size < 4) {
                        opj_event_msg(p_manager, EVT_WARNING, "Empty PPM marker\n");
                        return OPJ_TRUE;
                }
                else {
                        /* Uncompleted Ippm series in the previous PPM marker?*/
                        if (l_cp->ppm_data_read < l_cp->ppm_len) {
                                /* Get the place where add the remaining Ippm series*/
                                l_cp->ppm_data_current = &(l_cp->ppm_data[l_cp->ppm_data_read]);
                                l_N_ppm = l_cp->ppm_len - l_cp->ppm_data_read;
                        }
                        else {
                                OPJ_BYTE *new_ppm_data;
                                opj_read_bytes(p_header_data,&l_N_ppm,4);               /* First N_ppm */
                                p_header_data+=4;
                                p_header_size-=4;

                                /* sanity check: how much bytes is left for Ippm */
                                if( p_header_size < l_N_ppm )
                                  {
                                  opj_event_msg(p_manager, EVT_ERROR, "Not enough bytes (%u) to hold Ippm series (%u), Index (%d)\n", p_header_size, l_N_ppm, l_Z_ppm );
                                  opj_free(l_cp->ppm_data);
                                  l_cp->ppm_data = NULL;
                                  l_cp->ppm_buffer = NULL;
                                  l_cp->ppm = 0; /* do not use PPM */
                                  return OPJ_FALSE;
                                  }
                                /* Increase the size of ppm_data to add the new Ippm series*/
                                assert(l_cp->ppm_data == l_cp->ppm_buffer && "We need ppm_data and ppm_buffer to be the same when reallocating");
                                new_ppm_data = (OPJ_BYTE *) opj_realloc(l_cp->ppm_data, l_cp->ppm_len + l_N_ppm);
                                if (! new_ppm_data) {
                                        opj_free(l_cp->ppm_data);
                                        l_cp->ppm_data = NULL;
                                        l_cp->ppm_buffer = NULL;  /* TODO: no need for a new local variable: ppm_buffer and ppm_data are enough */
                                        l_cp->ppm_len = 0;
                                        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to increase the size of ppm_data to add the new Ippm series\n");
                                        return OPJ_FALSE;
                                }
                                l_cp->ppm_data = new_ppm_data;
                                l_cp->ppm_buffer = l_cp->ppm_data;

                                /* Keep the position of the place where concatenate the new series*/
                                l_cp->ppm_data_current = &(l_cp->ppm_data[l_cp->ppm_len]);
                                l_cp->ppm_len += l_N_ppm;
                        }
                }
        }

        l_remaining_data = p_header_size;

        while (l_remaining_data >= l_N_ppm) {
                /* read a complete Ippm series*/
                memcpy(l_cp->ppm_data_current, p_header_data, l_N_ppm);
                p_header_size -= l_N_ppm;
                p_header_data += l_N_ppm;

                l_cp->ppm_data_read += l_N_ppm; /* Increase the number of data read*/

                if (p_header_size)
                {
                        if (p_header_size < 4) {
                                opj_free(l_cp->ppm_data);
                                l_cp->ppm_data = NULL;
                                l_cp->ppm_buffer = NULL;  /* TODO: no need for a new local variable: ppm_buffer and ppm_data are enough */
                                l_cp->ppm_len = 0;
                                l_cp->ppm = 0;
                                opj_event_msg(p_manager, EVT_ERROR, "Error reading PPM marker\n");
                                return OPJ_FALSE;
                        }
                        opj_read_bytes(p_header_data,&l_N_ppm,4);               /* N_ppm^i */
                        p_header_data+=4;
                        p_header_size-=4;
                }
                else {
                        l_remaining_data = p_header_size;
                        break;
                }

                l_remaining_data = p_header_size;

                /* Next Ippm series is a complete series ?*/
                if (l_remaining_data >= l_N_ppm) {
                        OPJ_BYTE *new_ppm_data;
                        /* Increase the size of ppm_data to add the new Ippm series*/
                        assert(l_cp->ppm_data == l_cp->ppm_buffer && "We need ppm_data and ppm_buffer to be the same when reallocating");
                        /* Overflow check */
                        if ((l_cp->ppm_len + l_N_ppm) < l_N_ppm) {
                                opj_free(l_cp->ppm_data);
                                l_cp->ppm_data = NULL;
                                l_cp->ppm_buffer = NULL;  /* TODO: no need for a new local variable: ppm_buffer and ppm_data are enough */
                                l_cp->ppm_len = 0;
                                opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to increase the size of ppm_data to add the new (complete) Ippm series\n");
                                return OPJ_FALSE;
                        }
                        new_ppm_data = (OPJ_BYTE *) opj_realloc(l_cp->ppm_data, l_cp->ppm_len + l_N_ppm);
                        if (! new_ppm_data) {
                                opj_free(l_cp->ppm_data);
                                l_cp->ppm_data = NULL;
                                l_cp->ppm_buffer = NULL;  /* TODO: no need for a new local variable: ppm_buffer and ppm_data are enough */
                                l_cp->ppm_len = 0;
                                opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to increase the size of ppm_data to add the new (complete) Ippm series\n");
                                return OPJ_FALSE;
                        }
                        l_cp->ppm_data = new_ppm_data;
                        l_cp->ppm_buffer = l_cp->ppm_data;

                        /* Keep the position of the place where concatenate the new series */
                        l_cp->ppm_data_current = &(l_cp->ppm_data[l_cp->ppm_len]);
                        l_cp->ppm_len += l_N_ppm;
                }

        }

        /* Need to read an incomplete Ippm series*/
        if (l_remaining_data) {
                OPJ_BYTE *new_ppm_data;
                assert(l_cp->ppm_data == l_cp->ppm_buffer && "We need ppm_data and ppm_buffer to be the same when reallocating");

                /* Overflow check */
                if ((l_cp->ppm_len + l_N_ppm) < l_N_ppm) {
                        opj_free(l_cp->ppm_data);
                        l_cp->ppm_data = NULL;
                        l_cp->ppm_buffer = NULL;  /* TODO: no need for a new local variable: ppm_buffer and ppm_data are enough */
                        l_cp->ppm_len = 0;
                        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to increase the size of ppm_data to add the new (complete) Ippm series\n");
                        return OPJ_FALSE;
                }
                new_ppm_data = (OPJ_BYTE *) opj_realloc(l_cp->ppm_data, l_cp->ppm_len + l_N_ppm);
                if (! new_ppm_data) {
                        opj_free(l_cp->ppm_data);
                        l_cp->ppm_data = NULL;
                        l_cp->ppm_buffer = NULL;  /* TODO: no need for a new local variable: ppm_buffer and ppm_data are enough */
                        l_cp->ppm_len = 0;
                        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to increase the size of ppm_data to add the new (incomplete) Ippm series\n");
                        return OPJ_FALSE;
                }
                l_cp->ppm_data = new_ppm_data;
                l_cp->ppm_buffer = l_cp->ppm_data;

                /* Keep the position of the place where concatenate the new series*/
                l_cp->ppm_data_current = &(l_cp->ppm_data[l_cp->ppm_len]);
                l_cp->ppm_len += l_N_ppm;

                /* Read incomplete Ippm series*/
                memcpy(l_cp->ppm_data_current, p_header_data, l_remaining_data);
                p_header_size -= l_remaining_data;
                p_header_data += l_remaining_data;

                l_cp->ppm_data_read += l_remaining_data; /* Increase the number of data read*/
        }

#ifdef CLEAN_MSD

                if (l_cp->ppm_data_size == l_cp->ppm_len) {
                        if (p_header_size >= 4) {
                                /* read a N_ppm*/
                                opj_read_bytes(p_header_data,&l_N_ppm,4);               /* N_ppm */
                                p_header_data+=4;
                                p_header_size-=4;
                                l_cp->ppm_len += l_N_ppm ;

                                OPJ_BYTE *new_ppm_buffer = (OPJ_BYTE *) opj_realloc(l_cp->ppm_buffer, l_cp->ppm_len);
                                if (! new_ppm_buffer) {
                                        opj_free(l_cp->ppm_buffer);
                                        l_cp->ppm_buffer = NULL;
                                        l_cp->ppm_len = 0;
                                        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read ppm marker\n");
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