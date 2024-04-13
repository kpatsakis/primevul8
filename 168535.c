ncp_lookup_volume(struct ncp_server *server,
		  const char *volname, struct nw_info_struct *target)
{
	int result;

	memset(target, 0, sizeof(*target));
	result = ncp_get_volume_root(server, volname,
			&target->volNumber, &target->dirEntNum, &target->DosDirNum);
	if (result) {
		return result;
	}
	ncp_update_known_namespace(server, target->volNumber, NULL);
	target->nameLen = strlen(volname);
	memcpy(target->entryName, volname, target->nameLen+1);
	target->attributes = aDIR;
	/* set dates to Jan 1, 1986  00:00 */
	target->creationTime = target->modifyTime = cpu_to_le16(0x0000);
	target->creationDate = target->modifyDate = target->lastAccessDate = cpu_to_le16(0x0C21);
	target->nfs.mode = 0;
	return 0;
}