static OPJ_BOOL opj_j2k_read_com (  opj_j2k_t *p_j2k,
                                    OPJ_BYTE * p_header_data,
                                    OPJ_UINT32 p_header_size,
                                    opj_event_mgr_t * p_manager
                                    )
{
        /* preconditions */
        assert(p_j2k != 00);
        assert(p_manager != 00);
        assert(p_header_data != 00);
  (void)p_header_size;

        return OPJ_TRUE;
}