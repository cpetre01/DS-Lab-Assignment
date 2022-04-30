//void init_db(reply_t *reply) {
//    /* execute client request */
//    pthread_mutex_lock(&mutex_db);
//
//    int req_error_code = db_empty_db();
//
//    pthread_mutex_unlock(&mutex_db);
//
//    /* fill server reply */
//    set_server_error_code_std(reply, req_error_code);
//}


//void insert_item(request_t *request, reply_t *reply) {
//    /* execute client request */
//    pthread_mutex_lock(&mutex_db);
//
//    int req_error_code = db_write_item(request->item.key, request->item.value1,
//                                       &(request->item.value2),&(request->item.value3), CREATE);
//
//    pthread_mutex_unlock(&mutex_db);
//
//    /* fill server reply */
//    set_server_error_code_std(reply, req_error_code);
//}


//void get_item(request_t *request, reply_t *reply) {
//    /* execute client request */
//    pthread_mutex_lock(&mutex_db);
//
//    int req_error_code = db_read_item(request->item.key, reply->item.value1,
//                                      &(reply->item.value2), &(reply->item.value3));
//
//    pthread_mutex_unlock(&mutex_db);
//
//    /* fill server reply */
//    reply->item.key = request->item.key;
//    set_server_error_code_std(reply, req_error_code);
//}


//void modify_item(request_t *request, reply_t *reply){
//    /* execute client request */
//    pthread_mutex_lock(&mutex_db);
//
//    int req_error_code = db_write_item(request->item.key, request->item.value1,
//                                       &(request->item.value2), &(request->item.value3), MODIFY);
//
//    pthread_mutex_unlock(&mutex_db);
//
//    /* fill server reply */
//    set_server_error_code_std(reply, req_error_code);
//}


//void delete_item(request_t *request, reply_t *reply) {
//    /* execute client request */
//    pthread_mutex_lock(&mutex_db);
//
//    int req_error_code = db_delete_item(request->item.key);
//
//    pthread_mutex_unlock(&mutex_db);
//
//    /* fill server reply */
//    set_server_error_code_std(reply, req_error_code);
//}


//void item_exists(request_t *request, reply_t *reply) {
//    /* execute client request */
//    pthread_mutex_lock(&mutex_db);
//
//    int req_error_code = db_item_exists(request->item.key);
//
//    pthread_mutex_unlock(&mutex_db);
//
//    /* fill server reply */
//    switch (req_error_code) {
//        case 1: reply->server_error_code = SRV_EXISTS; break;
//        case 0: reply->server_error_code = SRV_NOT_EXISTS; break;
//        default: break;
//    }
//}