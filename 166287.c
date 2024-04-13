void opj_j2k_setup_end_compress (opj_j2k_t *p_j2k)
{
        /* preconditions */
        assert(p_j2k != 00);

        /* DEVELOPER CORNER, insert your custom procedures */
        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_eoc );

        if (OPJ_IS_CINEMA(p_j2k->m_cp.rsiz)) {
                opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_updated_tlm);
        }

        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_write_epc );
        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_end_encoding );
        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_destroy_header_memory);
}