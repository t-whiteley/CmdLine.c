#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_SIZE 1024
#define MAX_LINE_SIZE 256
#define MAX_PATH_SIZE 256
#define SAVE_NAME "save.txt"


typedef struct File {
    char* name;
    char* contents;
    struct Dir* parent;
} File;

typedef struct Dir {
    int len_name;
    char* name;
    struct Dir* parent;
    int num_dirs;
    struct Dir** dirs;
    int num_files;
    File** files;
} Dir;



// terminal commands
Dir* mk_drive();
Dir* mk_dir(Dir* parent_dir, char* dir_name);
Dir* change_dir(Dir* curr_dir, char* expr);
void print_tree(Dir* d, int lvl);
void print_path(Dir* curr_d);
void print_dir(Dir* curr_d);

// saving and loading commands
Dir* load_save(char* file_name);
void local_save(Dir* td, char* file_name);



int main() {
    // Dir* tdrive = mkdrive();
    // Dir* curr_dir = tdrive;
    // local_save(tdrive, SAVE_NAME);

    Dir* tdrive = load_save(SAVE_NAME);
    Dir* curr_dir = tdrive;

    char c;
    char expr[MAX_LINE_SIZE];
    int loop = 1;

    printf("[IMPLEMENTED CMDS: {p, l, e, c}]\n");
    while (loop) {
        print_path(curr_dir);
        switch (c = getc(stdin)) {
            // TREE OF FULL DIR
            case 'p':
                print_tree(tdrive, 1);
                break;

            // LIST CURR DIR
            case 'l':
                print_dir(curr_dir);
                break;

            // CHANGE DIR
            case 'c':
                fgets(expr, MAX_LINE_SIZE, stdin);
                curr_dir = change_dir(curr_dir, expr);
                break;
            
            // EXIT PROGRAM
            case 'e':
                loop = 0;
                break;
            
            default:
                printf("INVALID CMD");
                break;
        }

        // checks if it isnt a cmd that clears line anyways
        if (c != 'c') {
            while ((getchar()) != '\n');
        }
    }
}




// terminal commands
Dir* mkdrive() {
    char name[] = "tdrive";

    Dir* td = (Dir*) malloc(sizeof(Dir));
    td->len_name = strlen(name) + 2;
    td->name = (char*) malloc(td->len_name * sizeof(char));
    strcpy(td->name, name);
    strcat(td->name, "/");
    td->parent = NULL;
    td->num_dirs = 0;
    td->dirs = (Dir**) malloc(sizeof(Dir*) * td->num_dirs);
    td->num_files = 0;
    td->files = (File**) malloc(sizeof(File*) * td->num_files);
    return td;
}

Dir* mkdir(Dir* parent_dir, char* dir_name) {
    Dir* d = (Dir*) malloc(sizeof(Dir));
    d->len_name = strlen(dir_name) + 2;
    d->name = (char*) malloc(d->len_name * sizeof(char));
    strcpy(d->name, dir_name);
    strcat(d->name, "/");
    d->parent = parent_dir;
    d->num_dirs = 0;
    d->dirs = (Dir**) malloc(sizeof(Dir*) * d->num_dirs);
    d->num_files = 0;
    d->files = (File**) malloc(sizeof(File*) * d->num_files);

    parent_dir->dirs = (Dir**) realloc(parent_dir->dirs, sizeof(Dir*) * parent_dir->num_dirs);
    parent_dir->dirs[parent_dir->num_dirs] = d;
    parent_dir->num_dirs++;

    return d;
}

Dir* change_dir(Dir* curr_dir, char* expr) {
    for (int i = 0; i < strlen(expr) - 1; i++) {
        expr[i] = expr[i + 1];
    }
    expr[strlen(expr) - 2] = '\0';

    if (!(strcmp(expr, ".."))) {
        return curr_dir->parent;
    }

    for (int i = 0; i < curr_dir->num_dirs; i++) {
        if (!(strcmp(expr, curr_dir->dirs[i]->name))) {
            return curr_dir->dirs[i];
        }
    }

    printf("NOT A VALID DIR\n");
    return curr_dir;
}

void print_tree(Dir* d, int lvl) {
    for (int i = 0; i < lvl; i++) {
        printf("   ");
    }
    printf("↳");
    printf("%s\n", d->name);

    if (!(d->num_dirs)) return;

    for (int i = 0; i < d->num_dirs; i++) {
        print_tree(d->dirs[i], lvl + 1);
    }
}

void print_path(Dir* curr_d) {
    int i = 1;
    char** name_list = (char**)malloc(sizeof(char*) * i);
    name_list[0] = strdup(curr_d->name);

    while (curr_d->parent) {
        name_list = (char**)realloc(name_list, sizeof(char*) * (i + 1));
        curr_d = curr_d->parent;
        name_list[i] = strdup(curr_d->name);
        i++;
    }

    char path[MAX_PATH_SIZE] = "";
    for (int j = i - 1; j >= 0; j--) {
        strcat(path, name_list[j]);
    }

    strcat(path, " :: ");
    printf("%s", path);

    // Free the allocated memory
    for (int k = 0; k < i; k++) {
        free(name_list[k]);
    }
    free(name_list);
}

void print_dir(Dir* curr_d) {
    for (int i = 0; i < curr_d->num_dirs; i++) {
        printf("   ↳");
        printf("%s\n", curr_d->dirs[i]->name);
    }
}


// saving and loading commands
Dir* load_dir(FILE* file, Dir* parent_dir) {
    Dir* d = (Dir*)malloc(sizeof(Dir));

    fscanf(file, "%d", &(d->len_name));

    d->name = (char*) malloc(sizeof(char) * d->len_name);
    fscanf(file, "%s", d->name);

    d->parent = parent_dir;

    fscanf(file, "%d", &(d->num_dirs));
    d->dirs = (Dir**)malloc(sizeof(Dir*) * d->num_dirs);

    for (int i = 0; i < d->num_dirs; i++) {
        d->dirs[i] = load_dir(file, d);
    }

    return d;
}

Dir* load_save(char* file_name) {
    FILE* f = fopen(file_name, "r");
    Dir* d = load_dir(f, NULL);
    fclose(f);
    return d;
}

void save_dir(Dir* d, FILE* file) {
    fprintf(file, "%d\n", d->len_name);
    fprintf(file, "%s\n", d->name);
    fprintf(file, "%d\n", d->num_dirs);
    for (int i = 0; i < d->num_dirs; i++) {
        save_dir(d->dirs[i], file);
    }
}

void local_save(Dir* td, char* file_name) {
    FILE* f = fopen(file_name, "w");
    save_dir(td, f);
    fclose(f);
}