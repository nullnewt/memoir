// TODO:

// add ability to create a new diary folder for the user if there isn't one
// https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c

// add $home etc and same for windows so it works for all systems - enviroment variables

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>

int main() {
  const char * documents = "documents";
  const char * pass = "opensesame";
  const char * append = "append";
  const char * create = "create";
  const char * view = "view";
  const char * help = "help";
  const char * yes = "yes";
  const char * fin = "fin";
  const char * no = "no";

  char entry[150];
  char filename[65];
  char currenttime[20];
  char passtxt[PATH_MAX];
  char docent[PATH_MAX];
  char doc[PATH_MAX];
  char paspath[PATH_MAX];

  char * appendstr;
  char * diarystr;
  char * setpass;
  char * reenter;
  char * diary;

  appendstr = malloc(sizeof(char) * 1024);
  diarystr = malloc(sizeof(char) * 1024);
  diary = malloc(sizeof(char) * 1024);

  struct tm * timenow;
  time_t now = time(NULL);
  timenow = gmtime( & now);

  FILE * fp;
  FILE * pc;

  strcat(strcpy(doc, getenv("HOME")), "/Documents/Mementries/%d-%m-%Y.txt");
  strcat(strcpy(docent, getenv("HOME")), "/Documents/Mementries");
  strcat(strcpy(passtxt, getenv("HOME")), "/.config/memoirpass/pass.txt");
  strcat(strcpy(paspath, getenv("HOME")), "/.config/memoirpass/");


  void timestamp() {
    strftime(filename, sizeof(filename), doc, timenow);
    strftime(currenttime, 20, "%I:%M%p: ", timenow);
  }

  void landingmsg() {
    printf("Would you like to create an entry or append an existing entry?\n");
    printf("Please choose create or append, use help to list commands.\n");
  }

  void screenwipe() {
    sleep(1);
    printf("\e[1;1H\e[2J");
  }

  void promptpass() {
    while (1) {
      setpass = strdup(getpass("Enter Password: "));
      reenter = strdup(getpass("Re-enter Password: "));
      if (strcmp(setpass, reenter) == 0) {
        break;
      } else {
        printf("You failed the check, try again.\n");
      }
    }
  }

  void promptapp() {
    while (1) {
      printf("Is your entry complete? yes or no: ");
      scanf("%s", entry);
      if (strcmp(yes, entry) == 0) {
        screenwipe();
        landingmsg();
        break;
      } else if (strcmp(no, entry) == 0) {
        fp = fopen(filename, "a");
        printf("%s ", appendstr);
        scanf(" %[^\n]s*c", diarystr);
        fprintf(fp, "%s%s %s\n", currenttime, appendstr, diarystr);
        fclose(fp);
      }
    }
  }

  void promptcre() {
    while (1) {
      printf("Is your entry complete? yes or no: ");
      scanf("%s", entry);

      if (strcmp(yes, entry) == 0) {
        screenwipe();
        landingmsg();
        break;
      } else if (strcmp(no, entry) == 0) {
        fp = fopen(filename, "w+");
        printf("%s ", diary);
        scanf(" %[^\n]s*c", diarystr);
        fprintf(fp, "%s%s %s", currenttime, diary, diarystr);
        fclose(fp);
      }
    }
  }
  if (access(passtxt, F_OK) == 0) {
    while (1) {
      printf("Please enter the password to your diary: ");
      scanf("%s", entry);

      if (strcmp(pass, entry) == 0) { //compare strings
        printf("You entered the right password!\n");
        screenwipe();
        landingmsg();
        while (1) {
          scanf("%s", entry);
          if (fp == NULL) {
            perror("Error opening your file. ");
          } else if (strcmp(append, entry) == 0) {
            timestamp();
            fp = fopen(filename, "a");
            screenwipe();
            printf("Begin diary entry:\nPress enter to confirm your diary entry.\n");
            scanf(" %[^\n]s", appendstr);
            fprintf(fp, "%s%s\n", currenttime, appendstr);
            fclose(fp);
            promptapp();
          } else if (strcmp(create, entry) == 0) {
            timestamp();
            fp = fopen(filename, "w"); // creates file
            screenwipe(); // wipes  cmd again for diary entry
            printf("Begin diary entry:\nPress enter to confirm your diary entry.\n"); // diary entry prompt 
            scanf(" %[^\n]s", diary); // takes diary entry 
            fprintf(fp, "%s%s\n", currenttime, diary);
            fclose(fp); // close the file after entry is done
            promptcre();
          } else if (strcmp(view, entry) == 0) {
            DIR * d; // directory pointer
            struct dirent * dir;
            d = opendir("/mnt/c/Users/Me Lol/Desktop/diary/"); // set dir
            if (d) {
              while ((dir = readdir(d)) != NULL) { // read dir
                printf("%s\n", dir -> d_name); // print directory in char 
              }
              closedir(d); // close dir
            }
            sleep(1);
            landingmsg();
          } else if (strcmp(help, entry) == 0) {
            printf("Current list of commands: \n\nThe \"create\" entry will make a new diary entry text file with the date set as the name of the file."
              "\n\nThe \"append\" entry will append text to the last file created, including time before user input.\n\nThe \"fin\" entry will leave the prompt.\n"
              "\nThe \"view\" entry will list the current files in the diary directory.\n");
          } else if (strcmp(fin, entry) == 0) {
            printf("Exiting...\n");
            sleep(1);
            break;
          } else {
            printf("Please enter a command. Entry is case sensitive. \nUse the help option to list commands.\n");
          }
        }
      } else {
        printf("Wrong password, please try again.\n");
      }
    }
    screenwipe();
    landingmsg();

  } else {
    printf("Welcome to the introduction for your personalised diary."
      "\nIt seems you don't have a password set, let's fix that!\n");
    promptpass();
    mkdir(paspath, 0700);
    pc = fopen(passtxt, "w");
    fprintf(pc, "%s", setpass);
    fclose(pc);
    printf("%s will now be set as your password on next launch.\n\n", setpass);
    printf("It seems you also don't have a folder directory set for your diary entries.\n"
      "Would you like to set it on your own or have it in your docuemnts folder?\n"
      "Please choose \"docuemnts\" or \"own\"\n");
    while (1) {
      scanf("%s", entry);
      if (strcmp(documents, entry) == 0) 
        mkdir(docent, 0700);
      printf("Successfully made folder.");      
    }else if(strcmp(own, entry) == 0 ) {


      }

    }
  }

  free(appendstr);
  free(diarystr);
  free(setpass);
  free(reenter);
  free(diary);

  return (0);
}