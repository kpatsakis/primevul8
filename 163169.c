    BSONObj spec() {
        return BSON("$or" << BSON_ARRAY(false << true));
    }