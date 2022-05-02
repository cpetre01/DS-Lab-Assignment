#ifndef DBMS_H
#define DBMS_H

/* functions called by the server to manage the DB */
int db_init_db(void);
int db_get_num_pend_msgs(const char *username);
int db_empty_db(void);
int db_io_op_usr_ent(entry_t *entry, char mode);
int db_creat_usr_tbl(entry_t *entry);
int db_del_usr_tbl(const char *username);

#endif //DBMS_H
