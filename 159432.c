init_normal_cmds(void)
{
    int		i;

    // Fill the index table with a one to one relation.
    for (i = 0; i < (int)NV_CMDS_SIZE; ++i)
	nv_cmd_idx[i] = i;

    // Sort the commands by the command character.
    qsort((void *)&nv_cmd_idx, (size_t)NV_CMDS_SIZE, sizeof(short), nv_compare);

    // Find the first entry that can't be indexed by the command character.
    for (i = 0; i < (int)NV_CMDS_SIZE; ++i)
	if (i != nv_cmds[nv_cmd_idx[i]].cmd_char)
	    break;
    nv_max_linear = i - 1;
}