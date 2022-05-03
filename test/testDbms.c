#include <stdio.h>
#include <string.h>
#include "DS-Lab-Assignment/utils.h"
#include "DS-Lab-Assignment/dbms/dbms.h"


int main(int argc, char **argv) {
    if (db_init_db() == DBMS_ERR_ANY) return -1;

    printf("TEST: create user\n");
    /* create a user */
    entry_t entry_test_create;
    strcpy(entry_test_create.username, "cosmin");
    entry_test_create.type = ENT_TYPE_UD;
    entry_test_create.user.status = STATUS_DCN;
    strcpy(entry_test_create.user.ip, "");
    entry_test_create.user.port = 0;
    entry_test_create.user.last_msg_id = 1;

    db_creat_usr_tbl(&entry_test_create);

    /* read created user */
    entry_t entry_test_load_created;
    entry_test_load_created.type = ENT_TYPE_UD;
    strcpy(entry_test_load_created.username, "cosmin");
    db_io_op_usr_ent(&entry_test_load_created, READ);

    printf("%c, %s, %u, %u\n", entry_test_load_created.user.status, entry_test_load_created.user.ip,
           entry_test_load_created.user.port, entry_test_load_created.user.last_msg_id);

    printf("TEST: modify user\n");
    /* modify created user */
    /* create a user */
    entry_t entry_test_modify;
    strcpy(entry_test_modify.username, "cosmin");
    entry_test_modify.type = ENT_TYPE_UD;
    entry_test_modify.user.status = STATUS_CN;
    strcpy(entry_test_modify.user.ip, "localhost");
    entry_test_modify.user.port = 80;
    entry_test_modify.user.last_msg_id = 999;
    db_io_op_usr_ent(&entry_test_modify, MODIFY);

    /* read modified user */
    entry_t entry_test_load_modified;
    entry_test_load_modified.type = ENT_TYPE_UD;
    strcpy(entry_test_load_modified.username, "cosmin");
    db_io_op_usr_ent(&entry_test_load_modified, READ);

    printf("%c, %s, %u, %u\n", entry_test_load_modified.user.status, entry_test_load_modified.user.ip,
           entry_test_load_modified.user.port, entry_test_load_modified.user.last_msg_id);

    printf("TEST: delete user\n");
    /* delete created user */
    db_del_usr_tbl("cosmin");

    /* read deleted user */
    entry_t entry_test_load_deleted;
    entry_test_load_deleted.type = ENT_TYPE_UD;
    strcpy(entry_test_load_deleted.username, "cosmin");
    db_io_op_usr_ent(&entry_test_load_deleted, READ);

    printf("%c, %s, %u, %u\n", entry_test_load_deleted.user.status, entry_test_load_deleted.user.ip,
           entry_test_load_deleted.user.port, entry_test_load_deleted.user.last_msg_id);

    return 0;
}
