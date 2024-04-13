void opj_j2k_setup_decoding_validation (opj_j2k_t *p_j2k)
{
        /* preconditions*/
        assert(p_j2k != 00);

        opj_procedure_list_add_procedure(p_j2k->m_validation_list, (opj_procedure)opj_j2k_build_decoder);
        opj_procedure_list_add_procedure(p_j2k->m_validation_list, (opj_procedure)opj_j2k_decoding_validation);
        /* DEVELOPER CORNER, add your custom validation procedure */

}