void opj_j2k_setup_header_writing (opj_j2k_t *p_j2k)
{
        /* preconditions */
        assert(p_j2k != 00);

        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_init_info );
        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_soc );
        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_siz );
        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_cod );
        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_qcd );

        if (OPJ_IS_CINEMA(p_j2k->m_cp.rsiz)) {
                /* No need for COC or QCC, QCD and COD are used
                opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_all_coc );
                opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_all_qcc );
                */
                opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_tlm );

                if (p_j2k->m_cp.rsiz == OPJ_PROFILE_CINEMA_4K) {
                        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_poc );
                }
        }

        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_regions);

        if (p_j2k->m_cp.comment != 00)  {
                opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_com);
        }

        /* DEVELOPER CORNER, insert your custom procedures */
        if (p_j2k->m_cp.rsiz & OPJ_EXTENSION_MCT) {
                opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_mct_data_group );
        }
        /* End of Developer Corner */

        if (p_j2k->cstr_index) {
                opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_get_end_header );
        }

        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_create_tcd);
        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_update_rates);
}