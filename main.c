/*
 * Part III
 * Print all the directory entries with their size in any directory
 * Use the -l argument to display the info about the directory/file
 * Use the -a argument to display the hidden directories/files
 * Use the -R argument to recursively check each directory
 *
 * Author: Christian Loera - 20.02.2018
 */
#include<grp.h>
#include<pwd.h>
#include<time.h>
#include<stdio.h>
#include<getopt.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>

#define LIST        1
#define ALL         2
#define RECURSIVE   4

#define RESET   "\x1b[0m"
#define BOLD    "\x1b[1m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define BLUE    "\x1b[34m"

// User arguments functions
int arg_check(int ac, char* av[]);
void arg_error(char* fault_cmd, char* prg);

// Print help
void help(char* prg);

// Recursively Check every directory
void recursive(char* filepath, int *d, int *f, int *l, int *o, int bit);

// Printing the directories/files
void print(int bit, const char *fp, char *fn);

// Print all inside list
void print_all(char *fp, int bit);

// Main program
int main(int argc, char* argv[]) {
    struct stat namestatus;
    struct dirent **namelist;

    char *filepath;
    char buff[255];

    int bit = 0, i, n;
    unsigned long namesize;

    // Iterating through the user arguments
    printf("Command: $ %s ", argv[0]);
    for(i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
        char *temp = argv[i];

        // If the user passed an argument with the '-' infront, goto arg_check() to read what they passed
        if(temp[0] == '-') {
            if((bit |= arg_check(argc, argv)) == -1) { arg_error(temp, argv[0]); } }
        // Check if the user passed a filepath
        else if(!stat(argv[i], &namestatus)) {
            if(strcmp(argv[i], ".")==0 || strcmp(argv[i], "..")==0) { printf("\nIgnoring directory [ %s ]\n"); }
            else { filepath = argv[i]; }
        }
        // Oh shit boi! User passed an incorrect argument
        else { arg_error(argv[i], argv[0]); }
    }

    // Check if user didn't pass a path; if they didn't: get current current path
    if(stat(filepath, &namestatus) < 0) { char temp_buff[155]; getcwd(temp_buff, sizeof(temp_buff)); filepath = temp_buff; }
    printf("\n\n");

    // Checking if the user has passed a regular file
    snprintf(buff, sizeof(buff), "%s", filepath);
    stat(buff, &namestatus);
    if(S_ISREG(namestatus.st_mode) || (S_ISLNK(namestatus.st_mode))) { print(bit, filepath, ""); exit(0); }

    printf("Directory listing: %s\n", filepath);

    // Checking if user asked to recursively check ever directory
    if(bit & RECURSIVE) {
        int directories = 0, files = 0, links = 0, other = 0;
        print_all(filepath, bit);
        recursive(filepath, &directories, &files, &links, &other, bit);
        printf("\n\nDIRECTORIES: %d\n", directories);
        printf("FILES: %8d\n", files);
        printf("LINKS: %8d\n", links);
        printf("OTHER: %8d\n", other);
        printf("TOTAL: %8d\n\n", directories + files + links + other);
    } else {
        print_all(filepath, bit);
        printf("\n");
    } return 0;
}

// User arguments functions
int arg_check(int ac, char* av[]) {
    int b = 0, c;
    const char *s_opt = "laRh";
    struct option l_opt[] = {
        {     "help", no_argument, NULL, 'h'},
        {     "list", no_argument, NULL, 'l'},
        {      "all", no_argument, NULL, 'a'},
        {"recursive", no_argument, NULL, 'R'},
    };

    while((c = getopt_long(ac, av, s_opt, l_opt, NULL)) != -1) {
        switch(c) {
            case 'l': b |= LIST;       break;
            case 'a': b |= ALL;        break;
            case 'R': b |= RECURSIVE;  break;
            case 'h':
                help(av[0]);
                exit(0);
            default:
                return -1;
        }
    } return b;
}

void arg_error(char* fault_cmd, char* prg) {
    fprintf(stderr, "Error: unknown command : [ %s ]\n", fault_cmd);
    help(prg);
    exit(2);
}

// Print help
void help(char* prg) {
    printf("\n\nUsage: %s [-l] [-a] [-R] [file directory]\n\n", prg);
    printf("Optional commands:\n");
    printf("   -l, --list       : Display information\n");
    printf("   -a, --all        : Display hidden files\n");
    printf("   -R, --recursive  : List subdirectories recursively\n");
    printf("   -h, --help       : Display passable arguments\n");
}

// Recursively Check every directory
void recursive(char* filepath, int *d, int *f, int *l, int *o, int bit) {
    DIR *dir;
    struct dirent *nl, **dl;
    int hidden = 0, n, total;

    /*
     * Open a directory stream corresponding to the directory namef
     * This returns a pointer to the directory stream
     * If pointer is NULL, an error has occured
     */
    if((dir = opendir(filepath)) == NULL) { return; }

    /*
     * Returns a pointer to a dirent structure representing the next directory
     * If NULL is returned, it has reached the end of the directory
     */
    while((nl = readdir(dir)) != NULL) {
        /*
         * d_type returns what type of file dir is
         * DT_DIR is a value for directories
         */
        if(nl->d_type == DT_DIR) {
            char fp[1024];

            // Ignore any directories that are "." and ".."
            if((strcmp(nl->d_name, ".")!=0) && (strcmp(nl->d_name,"..")!=0)) {
                hidden = 0;
                if((nl->d_name[0] == '.') && !(bit & ALL)) { hidden = 1; }
                if(!hidden) {
                    (*d)++;
                    snprintf(fp, sizeof(fp), "%s/%s", filepath, nl->d_name);
                    if(filepath[strlen(filepath) - 1] == '/') { filepath[strlen(filepath) - 1] = 0; }

                    // Print what is in current directory
                    printf("\n\n%s/%s:\n", filepath, nl->d_name);
                    print_all(fp, bit);
                    recursive(fp, d, f, l, o, bit);
                }
            }
        } else { 
            if(nl->d_type == DT_REG) { (*f)++; }
            else if(nl->d_type == DT_LNK) { (*l)++; }
            else { (*o)++; }
        }
    }
}

// Print all inside list
void print_all(char *fp, int bit) {
    struct dirent **nl;
    int n =  scandir(fp, &nl, NULL, alphasort);
    if(n < 0) { perror("Error"); exit(2); }

    int total = n - 1;
    while(n--) {
        print(bit, fp, nl[total - n]->d_name);
        free(nl[total - n]);
    } free(nl);
}

// Printing the directories/files
void print(int bit, const char *fp, char *fn) {
    int hidden = 0, m, r;
    static int current_width = 0;
    char buff[255];
    char *linkname;
    struct stat ns;

    if((fn[0] == '.') && !(bit & ALL)) { hidden = 1; }
    if(!hidden) {
        // Writing into buff the directory path with the directory/file name
        if(strcmp(fn, "") == 0) {
            snprintf(buff, sizeof(buff), "%s", fp);
            fn = malloc(sizeof(fp));
            strcpy(fn, fp);
        } else { snprintf(buff, sizeof(buff), "%s/%s", fp, fn); }

        if(lstat(buff, &ns) != -1) {
            linkname = malloc(ns.st_size + 1);
            r = readlink(buff, linkname, ns.st_size + 1);
            linkname[ns.st_size] = '\0';
        } else { fprintf(stderr, "Woop! Don't know what happened here...\n"); exit(2); }

        if(bit & LIST) {
            // Getting the mode of the directory/file name
            int m = ns.st_mode;
            if(S_ISDIR(m)) { printf("d"); }
            else if(S_ISLNK(m)) { printf("l"); }
            else { printf("-"); }
            printf((m & S_IRUSR) ? "r":"-");
            printf((m & S_IWUSR) ? "w":"-");
            printf((m & S_IXUSR) ? "x":"-");
            printf((m & S_IRGRP) ? "r":"-");
            printf((m & S_IWGRP) ? "w":"-");
            printf((m & S_IXGRP) ? "x":"-");
            printf((m & S_IROTH) ? "r":"-");
            printf((m & S_IWOTH) ? "w":"-");
            printf((m & S_IXOTH) ? "x":"-");

            // Getting the number of directories/files the directories have
            printf("%3lu", ns.st_nlink);

            // Getting the user and group ID
            struct passwd *uid;
            struct group *gid;
            uid = getpwuid(ns.st_uid);
            gid = getgrgid(ns.st_gid);
            printf("%5s", uid->pw_name);
            printf("%5s", gid->gr_name);

            // Getting the last time the directory/file was changed
            char time_mod[150];
            strftime(time_mod, 150, "%c", localtime(&ns.st_ctime));

            // Printing out the files with color
            printf("%8zu %s ", ns.st_size, time_mod); 
            if(S_ISDIR(m)) { printf(BLUE "%s\n", fn); printf(RESET); }
            else if(S_ISLNK(m)) { printf(RED BOLD "%s -> %s\n", fn, linkname); printf(RESET); }
            else if((m & S_IXUSR) && (m & S_IXGRP) && (m & S_IXOTH)) { printf(GREEN "%s\n", fn); printf(RESET); }
            else { printf("%s\n", fn); }
        } else {
            if(S_ISDIR(ns.st_mode)) { printf(BLUE "%s[%zu] ", fn, ns.st_size); printf(RESET); }
            else if(S_ISLNK(ns.st_mode)) { printf(RED BOLD "%s[%zu] ", fn, ns.st_size); printf(RESET); }
            else if((ns.st_mode & S_IXUSR) && (ns.st_mode & S_IXGRP) && (ns.st_mode & S_IXOTH)) { printf(GREEN "%s[%zu] ", fn, ns.st_size); printf(RESET); }
            else { printf("%s[%zu] ", fn, ns.st_size); }
        }
    }
}
