#include <stdio.h>
#include <string.h>
#include "DS-Lab-Assignment/utils.h"
#include "DS-Lab-Assignment/dbms/dbms.h"
#include "DS-Lab-Assignment/dbms/dbmsUtils.h"


int main(int argc, char **argv) {
    entry_t entry_test_save;
    entry_test_save.type = ENT_TYPE_UD;
//    strcpy(entry_test_save.msg.content, "test");
    entry_test_save.user.status = 0;
    strcpy(entry_test_save.user.ip, "localhost");
    entry_test_save.user.port = 10000;
    entry_test_save.user.last_msg_id = 100;

    char buffer[1024];
    cpy_entry(buffer, &entry_test_save, SAVE_TO_DB);

    entry_t entry_test_load;
    entry_test_load.type = ENT_TYPE_UD;
    cpy_entry(buffer, &entry_test_load, LOAD_FROM_DB);

    printf("%c, %s, %u, %u", entry_test_load.user.status, entry_test_load.user.ip,
           entry_test_load.user.port, entry_test_load.user.last_msg_id);
    return 0;
}
