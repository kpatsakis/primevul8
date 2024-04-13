ncp_get_known_namespace(struct ncp_server *server, __u8 volume)
{
#if defined(CONFIG_NCPFS_OS2_NS) || defined(CONFIG_NCPFS_NFS_NS)
	int result;
	__u8 *namespace;
	__u16 no_namespaces;

	ncp_init_request(server);
	ncp_add_byte(server, 24);	/* Subfunction: Get Name Spaces Loaded */
	ncp_add_word(server, 0);
	ncp_add_byte(server, volume);

	if ((result = ncp_request(server, 87)) != 0) {
		ncp_unlock_server(server);
		return NW_NS_DOS; /* not result ?? */
	}

	result = NW_NS_DOS;
	no_namespaces = ncp_reply_le16(server, 0);
	namespace = ncp_reply_data(server, 2);

	while (no_namespaces > 0) {
		ncp_dbg(1, "found %d on %d\n", *namespace, volume);

#ifdef CONFIG_NCPFS_NFS_NS
		if ((*namespace == NW_NS_NFS) && !(server->m.flags&NCP_MOUNT_NO_NFS)) 
		{
			result = NW_NS_NFS;
			break;
		}
#endif	/* CONFIG_NCPFS_NFS_NS */
#ifdef CONFIG_NCPFS_OS2_NS
		if ((*namespace == NW_NS_OS2) && !(server->m.flags&NCP_MOUNT_NO_OS2))
		{
			result = NW_NS_OS2;
		}
#endif	/* CONFIG_NCPFS_OS2_NS */
		namespace += 1;
		no_namespaces -= 1;
	}
	ncp_unlock_server(server);
	return result;
#else	/* neither OS2 nor NFS - only DOS */
	return NW_NS_DOS;
#endif	/* defined(CONFIG_NCPFS_OS2_NS) || defined(CONFIG_NCPFS_NFS_NS) */
}