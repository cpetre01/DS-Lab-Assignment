# tests

# DBMS tests
set(TARGET_DBMS_TESTS testDbms)
add_executable(${TARGET_DBMS_TESTS})
target_sources(${TARGET_DBMS_TESTS} PRIVATE testDbms.c)
target_link_libraries(${TARGET_DBMS_TESTS}
        PRIVATE ${TARGET_DBMS}
        )
