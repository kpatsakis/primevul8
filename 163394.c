    BSONObj spec() {
        return BSON("$and" << BSON_ARRAY("$a" << 0));
    }