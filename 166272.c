opj_j2k_t* opj_j2k_create_decompress(void)
{
        opj_j2k_t *l_j2k = (opj_j2k_t*) opj_calloc(1,sizeof(opj_j2k_t));
        if (!l_j2k) {
                return 00;
        }

        l_j2k->m_is_decoder = 1;
        l_j2k->m_cp.m_is_decoder = 1;

        l_j2k->m_specific_param.m_decoder.m_default_tcp = (opj_tcp_t*) opj_calloc(1,sizeof(opj_tcp_t));
        if (!l_j2k->m_specific_param.m_decoder.m_default_tcp) {
                opj_j2k_destroy(l_j2k);
                return 00;
        }

        l_j2k->m_specific_param.m_decoder.m_header_data = (OPJ_BYTE *) opj_calloc(1,OPJ_J2K_DEFAULT_HEADER_SIZE);
        if (! l_j2k->m_specific_param.m_decoder.m_header_data) {
                opj_j2k_destroy(l_j2k);
                return 00;
        }

        l_j2k->m_specific_param.m_decoder.m_header_data_size = OPJ_J2K_DEFAULT_HEADER_SIZE;

        l_j2k->m_specific_param.m_decoder.m_tile_ind_to_dec = -1 ;

        l_j2k->m_specific_param.m_decoder.m_last_sot_read_pos = 0 ;

        /* codestream index creation */
        l_j2k->cstr_index = opj_j2k_create_cstr_index();
        if (!l_j2k->cstr_index){
                opj_j2k_destroy(l_j2k);
                return 00;
        }

        /* validation list creation */
        l_j2k->m_validation_list = opj_procedure_list_create();
        if (! l_j2k->m_validation_list) {
                opj_j2k_destroy(l_j2k);
                return 00;
        }

        /* execution list creation */
        l_j2k->m_procedure_list = opj_procedure_list_create();
        if (! l_j2k->m_procedure_list) {
                opj_j2k_destroy(l_j2k);
                return 00;
        }

        return l_j2k;
}