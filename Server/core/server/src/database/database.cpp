#include <stdio.h>
#include <sqlite3.h>
#include <stdbool.h>

sqlite3* db = NULL;

void openDB(const char* filename);
void closeDB();
void deleteAllData(sqlite3* database);
void createTable(sqlite3* database);
void insertData(sqlite3* database);
void updateData(sqlite3* database);
void deleteData(sqlite3* database);
void readDataStmt(sqlite3* db);


void openDB(const char* filename){
    int opened = sqlite3_open(filename, &db);
    if(opened){
        fprintf(stderr, "Database could not be opened: %s \n", sqlite3_errmsg(db));
    } else {
        printf("Base de donnée ouverte avec succès\n");
    }
}

void closeDB() {
    if (db != NULL) {
        sqlite3_close(db);
        printf("Base de donnée fermée avec succès\n");
        db = NULL;
    }
}

void deleteAllData(sqlite3* database){
    char* errMsg = 0;
    const char* sql = "DELETE FROM STUDENT; ";
    sqlite3_exec(database, sql, NULL, 0, &errMsg);
    if (errMsg) { sqlite3_free(errMsg); }
}

void createTable(sqlite3* database){
    char* errMsg = 0;
    const char* sql = "CREATE TABLE IF NOT EXISTS STUDENT("
                "ID INT PRIMARY KEY NOT NULL,"
                "PRENOM TEXT NOT NULL,"
                "NOM TEXT NOT NULL,"
                "SURNOM TEXT NOT NULL)";

    int rc = sqlite3_exec(database, sql, NULL, 0, &errMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr, "Error in executing SQL (CREATE): %s \n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Table STUDENT à été crée avec succés (ou existait déja)\n");
    }
}

void insertData(sqlite3* database){
    char* errMsg = 0;
    const char* sql = "INSERT INTO STUDENT (ID, PRENOM, NOM, SURNOM) "
                "VALUES(1, 'Noe', 'Choplin', 'Chat Majeste'),"
                "(2, 'Mael', 'Eouzan', 'Falling Table'),"
                "(3, 'Nikolas', 'Podevin', 'Inori Aizawa')";

    int rc = sqlite3_exec(database, sql, NULL, 0, &errMsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Error occured (INSERT): %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Operation insert successful \n");
    }
}

void updateData(sqlite3* database){
    char* errMsg = 0;
    const char* sql = "UPDATE STUDENT SET SURNOM = 'Falling Table' WHERE ID=2; ";
    printf("-----------------------------------------------------\n");

    int rc = sqlite3_exec(database, sql, NULL, 0, &errMsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Erreur rencontrée (UPDATE): %s\n", errMsg);
        sqlite3_free(errMsg);
    } 
}

void deleteData(sqlite3* database){
    char* errMsg = 0;
    const char* sql = "DELETE FROM STUDENT WHERE ID=3; ";

    int rc = sqlite3_exec(database, sql, NULL, 0, &errMsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Erreur rencontrée (DELETE): %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Deleted record successfully \n");
    }
}


void readDataStmt(sqlite3* db) {
    const char* sql = "SELECT * FROM STUDENT; ";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if(rc != SQLITE_OK){
        printf("error rencontrée: %s", sqlite3_errmsg(db));
    } else {

        int NoOfCols = sqlite3_column_count(stmt);
        bool done = false;
        while(!done){
            switch(sqlite3_step(stmt)){
            case SQLITE_ROW:
                for(int i=0; i<NoOfCols; i++){

                    const char* colName = sqlite3_column_name(stmt, i);
                    const unsigned char* text = sqlite3_column_text(stmt, i);
                    printf("Col: %s = %s \n", colName, text ? (const char*)text : "NULL");
                }
                printf("\n");

                break;
            case SQLITE_DONE:
                printf("-------------------------------- \n");
                printf("fini de lire toutes les colonnes \n");

                sqlite3_finalize(stmt);
                done = true;
                break;
            }
        }
    }
}

int main(int, char**){
    const char* filename = "db.sqlite3";

    openDB(filename);

    if (db == NULL) {
        return 1;
    }
    deleteAllData(db);
    createTable(db);
    insertData(db);
    updateData(db);
    //    deleteData(db);
    readDataStmt(db);
    closeDB();
    return 0;
}
