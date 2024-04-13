static void opj_j2k_setup_decoding (opj_j2k_t *p_j2k)
{
        /* preconditions*/
        assert(p_j2k != 00);

        opj_procedure_list_add_procedure(p_j2k->m_procedure_list,(opj_procedure)opj_j2k_decode_tiles);
        /* DEVELOPER CORNER, add your custom procedures */

}