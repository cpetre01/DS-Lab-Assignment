#ifndef SERVICES_H
#define SERVICES_H

/*** Services ***/
void init_db(reply_t *reply);
void insert_item(request_t *request, reply_t *reply);
void get_item(request_t *request, reply_t *reply);
void modify_item(request_t *request, reply_t *reply);
void delete_item(request_t *request, reply_t *reply);
void item_exists(request_t *request, reply_t *reply);

#endif //SERVICES_H
