get_dir_rec_size(struct iso9660 *iso9660, struct isoent *isoent,
    enum dir_rec_type t, enum vdd_type vdd_type)
{

	return (set_directory_record(NULL, SIZE_MAX,
	    isoent, iso9660, t, vdd_type));
}