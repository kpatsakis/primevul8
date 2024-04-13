ncp_mount_subdir(struct ncp_server *server,
		 __u8 volNumber, __u8 srcNS, __le32 dirEntNum,
		 __u32* volume, __le32* newDirEnt, __le32* newDosEnt)
{
	int dstNS;
	int result;

	ncp_update_known_namespace(server, volNumber, &dstNS);
	if ((result = ncp_ObtainSpecificDirBase(server, srcNS, dstNS, volNumber, 
				      dirEntNum, NULL, newDirEnt, newDosEnt)) != 0)
	{
		return result;
	}
	*volume = volNumber;
	server->m.mounted_vol[1] = 0;
	server->m.mounted_vol[0] = 'X';
	return 0;
}