idr_init(struct iso9660 *iso9660, struct vdd *vdd, struct idr *idr)
{

	idr->idrent_pool = NULL;
	idr->pool_size = 0;
	if (vdd->vdd_type != VDD_JOLIET) {
		if (iso9660->opt.iso_level <= 3) {
			memcpy(idr->char_map, d_characters_map,
			    sizeof(idr->char_map));
		} else {
			memcpy(idr->char_map, d1_characters_map,
			    sizeof(idr->char_map));
			idr_relaxed_filenames(idr->char_map);
		}
	}
}