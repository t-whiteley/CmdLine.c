#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_FILE_SIZE 1024
#define MAX_LINE_SIZE 256
#define MAX_PATH_SIZE 256
#define SAVE_NAME "save.txt"


typedef struct File {
    int len_name;
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
void mk_file(Dir* parent_dir, char* file_name);
Dir* change_dir(Dir* curr_dir, char* expr);
void print_tree(Dir* d, int lvl);
void print_path(Dir* curr_d);
void print_dir(Dir* curr_d);

// helper functions
void trim_whitespace(char* s);
void help();

// saving and loading commands
Dir* load_save(char* file_name);
void local_save(Dir* td, char* file_name);


int main() {
    // STARTING WITH NO SAVE
    // Dir* tdrive = mk_drive();
    // Dir* s1 = mk_dir(tdrive, "sub1");
    // local_save(tdrive, SAVE_NAME);
    // Dir* curr_dir = tdrive;

    Dir* tdrive = load_save(SAVE_NAME);
    Dir* curr_dir = tdrive;

    char c;
    char expr[MAX_LINE_SIZE];
    int loop = 1;


    printf("[Type 'h' for help]\n");
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

            // MAKE DIR
            case 'm':
                fgets(expr, MAX_LINE_SIZE, stdin);
                trim_whitespace(expr);
                mk_dir(curr_dir, expr);
                local_save(tdrive, SAVE_NAME);
                break;

            // CHANGE DIR
            case 'c':
                fgets(expr, MAX_LINE_SIZE, stdin);
                trim_whitespace(expr);
                curr_dir = change_dir(curr_dir, expr);
                break;

            // MAKE FILE
            case 'f':
                fgets(expr, MAX_LINE_SIZE, stdin);
                trim_whitespace(expr);
                mk_file(curr_dir, expr);
                local_save(tdrive, SAVE_NAME);
                break;
            
            // EXIT PROGRAM
            case 'e':
                loop = 0;
                break;
            
            case 'h':
                help();
                break;
        
            default:
                printf("INVALID CMD\n");
                break;
        }

        // checks if it isnt a cmd that clears line anyways before clearing
        if (c != 'c' && c != 'm' && c != 'f' && c != '\n') {
            while ((getchar()) != '\n');
        }
    }
}




// terminal commands
Dir* mk_drive() {
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

Dir* mk_dir(Dir* parent_dir, char* dir_name) {
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

void mk_file(Dir* parent_dir, char* file_name) {
    File* f = (File*) malloc(sizeof(File));
    f->len_name = strlen(file_name) + 1;
    f->name = (char*) malloc(f->len_name * sizeof(char));
    strcpy(f->name, file_name);
    f->parent = parent_dir;
    
    parent_dir->files = (File**) realloc(parent_dir->files, sizeof(File*) * parent_dir->num_files);
    parent_dir->files[parent_dir->num_files] = f;
    parent_dir->num_files++;
}

Dir* change_dir(Dir* curr_dir, char* expr) {

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

    for (int j = 0; j < d->num_files; j++) {
        for (int i = 0; i < lvl+1; i++) {
            printf("   ");
        }
        printf("→");
        printf("%s\n", d->files[j]->name);
    }

    if (!(d->num_dirs)) return;

    for (int k = 0; k < d->num_dirs; k++) {
        print_tree(d->dirs[k], lvl + 1);
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
    for (int j = 0; j < curr_d->num_files; j++) {
        printf("   →");
        printf("%s\n", curr_d->files[j]->name);
    }
}


// helper functions
void trim_whitespace(char* s) {
    int start = 0;
    while (isspace(s[start])) {
        start++;
    }

    int end = strlen(s) - 1;
    while (end > start && isspace(s[end])) {
        end--;
    }

    int length = end - start + 1;
    if (start > 0) {
        memmove(s, s + start, length);
    }

    s[length] = '\0';
}

void help() {
    printf("MANUAL:\n");
    printf("\tp: print entire tree of directories\n");
    printf("\tl: list subdirectories in a directory\n");
    printf("\te: exit the terminal\n");
    printf("\tc dir1/: change current directory to this sub-dir\n");
    printf("\tm dir1: make new sub-dir in current directory named dir1\n");
    printf("\tf dir1: make new file in current directory named dir1\n");
}


// saving and loading commands
Dir* load_dir(FILE* file, Dir* parent_dir) {
    Dir* d = (Dir*)malloc(sizeof(Dir));

    fscanf(file, "%d", &(d->len_name));
    d->name = (char*) malloc(sizeof(char) * d->len_name);
    fscanf(file, "%s", d->name);
    d->parent = parent_dir;
    fscanf(file, "%d", &(d->num_dirs));
    fscanf(file, "%d", &(d->num_files));

    d->files = (File**) malloc(sizeof(File*) * d->num_files);
    for (int i = 0; i < d->num_files; i++) {
        File* f = (File*) malloc(sizeof(File));
        fscanf(file, "%d", &(f->len_name));
        f->name = (char*) malloc(sizeof(char) * f->len_name);
        fscanf(file, "%s", f->name);
        f->parent = d;
        d->files[i] = f;
    }

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
    fprintf(file, "%d\n", d->num_files);
    
    for (int i = 0; i < d->num_files; i++) {
        fprintf(file, "%d\n", d->files[i]->len_name);
        fprintf(file, "%s\n", d->files[i]->name);
    }

    for (int i = 0; i < d->num_dirs; i++) {
        save_dir(d->dirs[i], file);
    }
    // for (int i = 0; i < d->num_files; i++) {
    //     fprintf(file, "%d\n", d->len_name);
    //     fprintf(file, "%s\n", d->name);
    // }
}

void local_save(Dir* td, char* file_name) {
    FILE* f = fopen(file_name, "w");
    save_dir(td, f);
    fclose(f);
}