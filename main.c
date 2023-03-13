#include <sqlite3.h>
#include <stdio.h>

#define HANDLE_SQLITE_ERROR(msg) do{ if (rc != SQLITE_OK ) { fprintf(stderr, (msg), sqlite3_errmsg(db)); sqlite3_close(db); return 1;}} while(0)

int callback_list_all(void *, int, char **, char **);

int main(int argc, char **argv) {
    printf("SQLITE VERSION: %s\n", sqlite3_libversion()); 
    
    sqlite3 *db;
    char *err_msg = 0;
    char *sql;
    sqlite3_stmt *res;

    int rc = sqlite3_open(argv[1], &db);
    HANDLE_SQLITE_ERROR("Cannot open database: %s\n");

    printf("LISTING ALL USERS!\n");
    sql = "SELECT * FROM users";
    rc = sqlite3_exec(db, sql, callback_list_all, 0, &err_msg);
    HANDLE_SQLITE_ERROR("Cannot select all users: %s\n");

    printf("GETTING SINGLE USER!\n");
    sql = "SELECT username, email, last_login FROM users WHERE username = ?"; 
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(res, 1, argv[2], -1, SQLITE_STATIC);
    } else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
    int step = sqlite3_step(res);
    if (step == SQLITE_ROW) {
        printf("User %s has email %s and last logged in on %s\n\n", 
               sqlite3_column_text(res, 0), 
               sqlite3_column_text(res, 1),
               sqlite3_column_text(res, 2)
        );
    }

    printf("GETTING MESSAGES INVOLVING %s\n", argv[2]);
    sql = "SELECT \"from\", \"to\", content FROM messages WHERE \"from\" = ?1 OR \"to\" = ?1"; 
    
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(res, 1, argv[2], -1, SQLITE_STATIC);
    } else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
    // Read all rows
    while(sqlite3_step(res) == SQLITE_ROW){
        printf("%10s -> %10s: %s\n", 
               sqlite3_column_text(res, 0), // from
               sqlite3_column_text(res, 1), // to
               sqlite3_column_text(res, 2) // content
        );
    }

    sqlite3_close(db);
    return 0;
}

int callback_list_all(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}