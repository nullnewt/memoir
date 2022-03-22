// TODO:
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
#include <openssl/aes.h>
#include <glob.h>

  const char *documents = "documents";
  const char *append = "append";
  const char *create = "create";
  const char *view = "view";
  const char *help = "help";
  const char *y = "y";
  const char *n = "n";
  const char *fin = "fin";

  unsigned char indata[AES_BLOCK_SIZE];
  unsigned char outdata[AES_BLOCK_SIZE];
  unsigned char decryptdata[AES_BLOCK_SIZE];
  unsigned char userkey[] = "\x09\x8F\x6B\xCD\x46\x21\xD3\x73\xCA\xDE\x4E\x83\x26\x27\xB4\xF6";
  unsigned char IV[] = "\x0A\x91\x72\x71\x6A\xE6\x42\x84\x09\x88\x5B\x8B\x82\x9C\xCB\x05";

  char filebuffer[10];
  char entry[150];
  char filename[65];
  char incfilename[65];
  char currenttime[20];
  char passtxt[PATH_MAX];
  char passpath[PATH_MAX];
  char encpass[PATH_MAX];
  char docent[PATH_MAX];
  char doc[PATH_MAX];

  char *appendstr;
  char *diarystr;
  char *setpass;
  char *reenter;
  char *diary;
  char *passcheck;
  char **found;
  char **seen;

  int *numbuffer;
  int holding;
  int i;
  int r;
  int x;
  char z;

  FILE *ifp;
  FILE *ofp;
  FILE *fp;
  FILE *pc;
  FILE *opfp;
  FILE *f;


int main()

{
    
  appendstr = malloc(sizeof(char) *1024);
  diarystr = malloc(sizeof(char) *1024);
  diary = malloc(sizeof(char) *1024);
  numbuffer = malloc(sizeof(char) *1024);

  AES_KEY key;
  AES_set_encrypt_key(userkey, 128, &key);

  struct tm * timenow;
  time_t now = time(NULL);
  timenow = gmtime(&now);

  strcat(strcpy(passtxt, getenv("HOME")), "/.config/memoirpass/pass.txt");
  strcat(strcpy(encpass, getenv("HOME")), "/.config/memoirpass/passenc");
  strcat(strcpy(passpath, getenv("HOME")), "/.config/memoirpass/");
  strcat(strcpy(docent, getenv("HOME")), "/Documents/Mementries");

  void timestamp()
  {
    strcat(strcpy(doc, getenv("HOME")), "/Documents/Mementries/%d-%m-%Y.txt");
    strftime(filename, sizeof(filename), doc, timenow);
    strftime(currenttime, 20, "%I:%M%p: ", timenow);
  }

  void landingmsg()
  {
    printf("Would you like to create an entry or append an existing entry?\n");
    printf("Please choose create or append, use help to list commands.\n");
  }

  void screenwipe()
  {
    printf("\e[1;1H\e[2J");
  }

  void promptpass()
  {
    while (1)
    {
      setpass = strdup(getpass("Enter Password: "));
      reenter = strdup(getpass("Re-enter Password: "));
      if (strcmp(setpass, reenter) == 0)
      {
      break;
      }
      else
      {
        printf("You failed the check, try again.\n");
      }
    }
  }

  void promptapp()
  {
    while (1)
    {
      printf("Is your entry complete? Y or N: ");
      scanf("%s", entry);
      if (strcmp(y, entry) == 0)
      {
        screenwipe();
        landingmsg();
        break;
      }
      else if (strcmp(n, entry) == 0)
      {
        fp = fopen(filename, "a");
        if (fp == NULL)
          perror("Error: ");
        printf("%s ", appendstr);
        scanf(" %[^\n]s*c", diarystr);
        fprintf(fp, "%s%s %s\n", currenttime, appendstr, diarystr);
        fclose(fp);
      }
    }
  }

  void promptcre()
  {
    while (1)
    {
      printf("Is your entry complete? Y or N: ");
      scanf("%s", entry);
      if (strcmp(y, entry) == 0)
      {
        screenwipe();
        landingmsg();
        break;
      }
      else if (strcmp(n, entry) == 0)
      {
        fp = fopen(filename, "w+");
        printf("%s ", diary);
        scanf(" %[^\n]s*c", diarystr);
        fprintf(fp, "%s%s %s", currenttime, diary, diarystr);
        fclose(fp);
      }
    }
  }

  void encryptentries(){
    chdir(docent);
    system("ls -1 *.txt | wc -l  >> output.txt"); 
    FILE *f = fopen("output.txt", "r"); 
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    int *numbuffer = malloc(fsize + 1);
    fread(numbuffer, fsize, 1, f);
    fclose(f);
    numbuffer[fsize] = 0;
    holding = *numbuffer;
    remove("output.txt"); 

   for (i = 0; i < holding; ++i){
        {
        glob_t gstruct;
        r = glob("*.txt", GLOB_ERR, NULL, &gstruct); 
        if (r == 0)
        {
          if (r == GLOB_NOMATCH)
          perror("Error: ");
           else perror("Error: ");
        }
        found = gstruct.gl_pathv;
        while (*found)
        {
          sprintf(filebuffer, "%s", *found);
          found++;
        }
        ifp = fopen(filebuffer, "r+"); 
        if (ifp == NULL)
            perror("Error input file");
        sprintf(incfilename,"file%d",i);
        ofp = fopen(incfilename, "w");
        if (ofp == NULL)
            perror("Error output file");
        int postion = 0;
        int bytes_read, bytes_write;
        while(i < holding)
        {
          unsigned char ivec[AES_BLOCK_SIZE]; 
          memcpy(ivec, IV, AES_BLOCK_SIZE);
          bytes_read = fread(indata, 1, AES_BLOCK_SIZE, ifp);
          AES_cfb128_encrypt(indata, outdata, bytes_read, &key, ivec, &postion, AES_ENCRYPT);
          bytes_write = fwrite(outdata, 1, bytes_read, ofp);
          if (bytes_read < AES_BLOCK_SIZE)
             remove(filebuffer); 
          break;
        }     
    fclose(ifp);
    fclose(ofp);
    }
  }
}
  void encryptpass()
  {
    ifp = fopen(passtxt, "r+");
    if (ifp == NULL)
      perror("Error: ");
    ofp = fopen(encpass, "w+");
    if (ofp == NULL)
      perror("Error: ");
    int postion = 0;
    int bytes_read, bytes_write;
    while (1)
    {
      unsigned char ivec[AES_BLOCK_SIZE];
      memcpy(ivec, IV, AES_BLOCK_SIZE);
      bytes_read = fread(indata, 1, AES_BLOCK_SIZE, ifp);
      AES_cfb128_encrypt(indata, outdata, bytes_read, &key, ivec, &postion, AES_ENCRYPT);
      bytes_write = fwrite(outdata, 1, bytes_read, ofp);
      if (bytes_read < AES_BLOCK_SIZE)
         remove(passtxt);
      break;
    }

    fclose(ifp);
    fclose(ofp);
  }

  void decryptpass()
  {
    FILE *ifp, *ofp;
    ifp = fopen(encpass, "r+");
    ofp = fopen(passtxt, "w+");
    int postion = 0;
    int bytes_read, bytes_write;
    while (1)
    {
      unsigned char ivec[AES_BLOCK_SIZE];
      memcpy(ivec, IV, AES_BLOCK_SIZE);
      bytes_read = fread(outdata, 1, AES_BLOCK_SIZE, ifp);
      AES_cfb128_encrypt(outdata, decryptdata, bytes_read, &key, ivec, &postion, AES_DECRYPT);
      bytes_write = fwrite(decryptdata, 1, bytes_read, ofp);
      if (bytes_read < AES_BLOCK_SIZE)
         remove(encpass);
      break;
    }

    fclose(ifp);
    fclose(ofp);
  }

  if (access(encpass, F_OK) == 0)
  {
    decryptpass();
    // decryptentries();
    while (1)
    {
      FILE *f = fopen(passtxt, "r");
      fseek(f, 0, SEEK_END);
      long fsize = ftell(f);
      fseek(f, 0, SEEK_SET);
      char *pass = malloc(fsize + 1);
      fread(pass, fsize, 1, f);
      fclose(f);
      pass[fsize] = 0;
      encryptpass();
      passcheck = strdup(getpass("Please enter the password to your diary: "));
      if (strcmp(pass, passcheck) == 0)
      {
        //compare strings
        printf("You entered the right password!\n");
        screenwipe();
        landingmsg();
        while (1)
        {
          scanf("%s", entry);
          if (strcmp(append, entry) == 0)
          {
            timestamp();
            fp = fopen(filename, "a");
            screenwipe();
            printf("Begin diary entry:\nPress enter to confirm your diary entry.\n");
            scanf(" %[^\n]s", appendstr);
            fprintf(fp, "%s%s\n", currenttime, appendstr);
            fclose(fp);
            promptapp();
          }
          else if (strcmp(create, entry) == 0)
          {
            timestamp();
            fp = fopen(filename, "w");
            screenwipe();
            printf("Begin diary entry:\nPress enter to confirm your diary entry.\n");
            scanf(" %[^\n]s", diary);
            fprintf(fp, "%s%s\n", currenttime, diary);
            fclose(fp);
            promptcre();
          }
          else if (strcmp(view, entry) == 0)
          {
            {
              glob_t gstruct;
              int r;
              chdir(docent);
              r = glob("*.txt", GLOB_ERR, NULL, &gstruct);
              if (r != 0)
              {
                if (r == GLOB_NOMATCH)
                  perror("Error: ");
                else perror("Error: ");
              }

              seen = gstruct.gl_pathv;
              while (*seen)
              {
                printf("%s\n", *seen);
                seen++;
              }
            }
            landingmsg();
          }
          else if (strcmp(help, entry) == 0)
          {
            printf("Current list of commands: \n\nThe \"create\" "
              "entry will make a new diary entry text file"
              "with the date set as the name of the file."
              "\n\nThe \"append\" entry will append text to the last file created,"
              "including time before user input.\n"
              "\nThe \"fin\" entry will leave the prompt.\n"
              "\nThe \"view\" entry will list the current files in the diary directory.\n");
          }
          else if (strcmp(fin, entry) == 0)
          {
            encryptentries();
            printf("Exiting...\n");
            exit(0);
          }
          else
          {
            printf("Please enter a command. Entry is case sensitive."
              "\nUse the help option to list commands.\n");
          }
        }
      }
      else
      {
        printf("Wrong password, please try again.\n");
        decryptpass();
      }
    }

    screenwipe();
    landingmsg();

  }
  else
  {
    printf("Welcome to the introduction for your personalised diary."
      "\nIt seems you don't have a password set, let's fix that!\n");
    promptpass();
    mkdir(passpath, 0700);
    pc = fopen(passtxt, "w");
    if (pc == NULL)
      perror("Error: ");
    fprintf(pc, "%s", setpass);
    fclose(pc);
    encryptpass();
    printf("%s will now be set as your password on next launch.\n\n", setpass);
    printf("It seems you also don't have a folder directory set for your diary entries.\n"
      "Would you like to set it on your own or have it in your docuemnts folder?\n"
      "Please choose \"docuemnts\" or \"own\"\n");
    while (1)
    {
      scanf("%s", entry);
      if (strcmp(documents, entry) == 0)
      {
        mkdir(docent, 0700);
        printf("Chosen directory created.");
      }
      else
      {
        printf("Current working directory: %s\n", getenv("HOME"));
        chdir(getenv("HOME"));
        scanf("%s", entry);
        mkdir(entry, 0700);
        printf("Chosen directory created.");
      }
    }
  }

  free(appendstr);
  free(diarystr);
  free(setpass);
  free(reenter);
  free(diary);
  free(filebuffer);
  return (0);
}