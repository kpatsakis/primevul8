void opj_j2k_setup_header_reading (opj_j2k_t *p_j2k)
{
        /* preconditions*/
        assert(p_j2k != 00);

        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_read_header_procedure);

        /* DEVELOPER CORNER, add your custom procedures */
        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_copy_default_tcp_and_create_tcd);

}