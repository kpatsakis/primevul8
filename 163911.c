ldns_str2rdf_loc(ldns_rdf **rd, const char *str)
{
	uint32_t latitude = 0;
	uint32_t longitude = 0;
	uint32_t altitude = 0;

	uint8_t *data;
	uint32_t equator = (uint32_t) ldns_power(2, 31);

	uint32_t h = 0;
	uint32_t m = 0;
	uint8_t size_b = 1, size_e = 2;
	uint8_t horiz_pre_b = 1, horiz_pre_e = 6;
	uint8_t vert_pre_b = 1, vert_pre_e = 3;

	double s = 0.0;
	bool northerness;
	bool easterness;

	char *my_str = (char *) str;

	/* only support version 0 */
	if (isdigit((int) *my_str)) {
		h = (uint32_t) strtol(my_str, &my_str, 10);
	} else {
		return LDNS_STATUS_INVALID_STR;
	}

	while (isblank((int) *my_str)) {
		my_str++;
	}

	if (isdigit((int) *my_str)) {
		m = (uint32_t) strtol(my_str, &my_str, 10);
	} else if (*my_str == 'N' || *my_str == 'S') {
		goto north;
	} else {
		return LDNS_STATUS_INVALID_STR;
	}

	while (isblank((int) *my_str)) {
		my_str++;
	}

	if (isdigit((int) *my_str)) {
		s = strtod(my_str, &my_str);
	}
north:
	while (isblank((int) *my_str)) {
		my_str++;
	}

	if (*my_str == 'N') {
		northerness = true;
	} else if (*my_str == 'S') {
		northerness = false;
	} else {
		return LDNS_STATUS_INVALID_STR;
	}

	my_str++;

	/* store number */
	s = 1000.0 * s;
	/* add a little to make floor in conversion a round */
	s += 0.0005;
	latitude = (uint32_t) s;
	latitude += 1000 * 60 * m;
	latitude += 1000 * 60 * 60 * h;
	if (northerness) {
		latitude = equator + latitude;
	} else {
		latitude = equator - latitude;
	}
	while (isblank((unsigned char)*my_str)) {
		my_str++;
	}

	if (isdigit((int) *my_str)) {
		h = (uint32_t) strtol(my_str, &my_str, 10);
	} else {
		return LDNS_STATUS_INVALID_STR;
	}

	while (isblank((int) *my_str)) {
		my_str++;
	}

	if (isdigit((int) *my_str)) {
		m = (uint32_t) strtol(my_str, &my_str, 10);
	} else if (*my_str == 'E' || *my_str == 'W') {
		goto east;
	} else {
		return LDNS_STATUS_INVALID_STR;
	}

	while (isblank((unsigned char)*my_str)) {
		my_str++;
	}

	if (isdigit((int) *my_str)) {
		s = strtod(my_str, &my_str);
	}

east:
	while (isblank((unsigned char)*my_str)) {
		my_str++;
	}

	if (*my_str == 'E') {
		easterness = true;
	} else if (*my_str == 'W') {
		easterness = false;
	} else {
		return LDNS_STATUS_INVALID_STR;
	}

	my_str++;

	/* store number */
	s *= 1000.0;
	/* add a little to make floor in conversion a round */
	s += 0.0005;
	longitude = (uint32_t) s;
	longitude += 1000 * 60 * m;
	longitude += 1000 * 60 * 60 * h;

	if (easterness) {
		longitude += equator;
	} else {
		longitude = equator - longitude;
	}

	altitude = (uint32_t)(strtod(my_str, &my_str)*100.0 +
		10000000.0 + 0.5);
	if (*my_str == 'm' || *my_str == 'M') {
		my_str++;
	}

	if (strlen(my_str) > 0) {
		if(!loc_parse_cm(my_str, &my_str, &size_b, &size_e))
			return LDNS_STATUS_INVALID_STR;
	}

	if (strlen(my_str) > 0) {
		if(!loc_parse_cm(my_str, &my_str, &horiz_pre_b, &horiz_pre_e))
			return LDNS_STATUS_INVALID_STR;
	}

	if (strlen(my_str) > 0) {
		if(!loc_parse_cm(my_str, &my_str, &vert_pre_b, &vert_pre_e))
			return LDNS_STATUS_INVALID_STR;
	}

	data = LDNS_XMALLOC(uint8_t, 16);
        if(!data) {
                return LDNS_STATUS_MEM_ERR;
        }
	data[0] = 0;
	data[1] = 0;
	data[1] = ((size_b << 4) & 0xf0) | (size_e & 0x0f);
	data[2] = ((horiz_pre_b << 4) & 0xf0) | (horiz_pre_e & 0x0f);
	data[3] = ((vert_pre_b << 4) & 0xf0) | (vert_pre_e & 0x0f);
	ldns_write_uint32(data + 4, latitude);
	ldns_write_uint32(data + 8, longitude);
	ldns_write_uint32(data + 12, altitude);

	*rd = ldns_rdf_new_frm_data(
		LDNS_RDF_TYPE_LOC, 16, data);

	LDNS_FREE(data);
	return *rd?LDNS_STATUS_OK:LDNS_STATUS_MEM_ERR;
}