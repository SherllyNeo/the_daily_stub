#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 10000
#define INDEX_FILE "index.html"

/* 
 SIMPLE USAGE
 ./poser title text_for_post
 */

int find_line_matching(FILE* fp,const char* line_to_find) {
    fseek(fp, 0, SEEK_SET);
    char ch;
    int count_of_matching = 0;
    int lines = 0;
    while(!feof(fp))
    {
        ch = fgetc(fp);
        if (ch == line_to_find[count_of_matching]) {
            count_of_matching++;
        }
        if (count_of_matching == (int)strlen(line_to_find)) {
            return lines;
        }

        if(ch == '\n')
        {
            lines++;
            count_of_matching = 0;
        }
    }

    return -1;
}

int count_lines_of_file(FILE* fp) {
    fseek(fp, 0, SEEK_SET);
    char ch;
    int lines = 0;
    while(!feof(fp))
    {
        ch = fgetc(fp);
        if(ch == '\n')
        {
            lines++;
        }
    }
    return lines;
}

int get_last_post_id(FILE* fp) {
    char* line_to_find = "<div id=\"post";
    fseek(fp, 0, SEEK_SET);
    char ch;
    int count_of_matching = 0;
    int lines = 0;
    while(!feof(fp))
    {
        ch = fgetc(fp);
        if (ch == line_to_find[count_of_matching]) {
            count_of_matching++;
        }
        if (count_of_matching == (int)strlen(line_to_find)) {
            ch = fgetc(fp);
            return ch - '0';
        }

        if(ch == '\n')
        {
            lines++;
            count_of_matching = 0;
        }
    }

    return -1;
}

int insert_at_line(char* file_name,int line_to_insert,char* text_to_insert) {
    FILE* fp = fopen(file_name,"rb");
    if (fp == NULL) {
        fprintf(stderr,"unable to open %s\n",file_name);
        exit(EXIT_FAILURE);
    }

    FILE* tmp = fopen("./tmp","w");
    if (tmp == NULL) {
        fprintf(stderr,"unable to open tmp\n");
        exit(EXIT_FAILURE);
    }

    int lines = 0;
    char ch;
    while(!feof(fp))
    {
        ch = fgetc(fp);
        if (
                isalpha(ch) || isdigit(ch) || ch == '\n' || ch == '<' || ch == '>' || ch == '=' || ch == ' ' 
                || ch == '\t' || ch == '"' || ch == ',' || ch == ';'
                || ch == '.' || ch == '/'
                ) {
            fprintf(tmp,"%c",ch);
        }
        if (lines == line_to_insert) {
            fprintf(tmp,"\n%s\n",text_to_insert);
            lines++;
        }
        if(ch == '\n')
        {
            lines++;
        }
    }
    fclose(fp);
    fclose(tmp);

    remove(file_name);
    rename("tmp", file_name);

    return 0;
}

int init_file(char* file_name) {
    char init_html[] = 
    "<!DOCTYPE html>\n\t\n<html lang=\"en\">\n<head>\n\t\t<title>The Daily Stub</title> \
    \n\t\t<meta charset=\"UTF-8\"> \
    \n\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \
    \n\t\t<link href=\"css/style.css\" rel=\"stylesheet\"> \
    \n</head> \
    \n\t<body>  \
    \n  \
    \n\t<h1>Welcome to the daily stub</h1>	\
    \n	\
    \n	\
    \n\t<div id=\"post0\" class=\"post\">	\
    \n\t\t<h2>Welcome to a daily stub post</h2>	\
        \n\t\t<p>	\
        \n\t\tI keep stubbing my toe everyday. Here is my story.	\
        \n\t\t</p>	\
    \n\t</div>	\
	\n\
   \n\t</body>	\
   \n</html> ";
    FILE* fp = fopen(file_name,"w");
    if (fp == NULL) {
        fprintf(stderr,"unable to open %s\n",file_name);
        exit(EXIT_FAILURE);
    }

    fprintf(fp,"%s\n",init_html);
    fclose(fp);
    return 0;
}


int main(int argc, char *argv[])
{
    printf("IMPORTANT NOTE: this expects the file to be in a specific format, please do not edit index.html without knowing this format or may make breaking changes\n");

    const char TOP[] = "<h1>Welcome to the daily stub</h1>";

    FILE* fp_to_check = fopen(INDEX_FILE,"r");

    if (fp_to_check == NULL) {
        fprintf(stderr,"unable to open %s\n",INDEX_FILE);
        printf("making new file as current one does not exist\n");
        int result = init_file(INDEX_FILE);
        if (result != 0) {
            fprintf(stderr,"unable to init file: %s\n",INDEX_FILE);
            exit(EXIT_FAILURE);
        }
    }

    FILE* fp = fopen(INDEX_FILE,"r");
    if (fp == NULL) {
        fprintf(stderr,"ERROR: %s pointer is still null after safety checks and potential re-init\n",INDEX_FILE);
        exit(EXIT_FAILURE);
    }


    int new_post_id = get_last_post_id(fp)+1;
    int line_to_insert_new_post = find_line_matching(fp,&TOP[0]) + 1;

    printf("new post: %d to be inserted at line %d\n",new_post_id,line_to_insert_new_post);

    char new_post[MAX];
    char* title = "New post!";
    char* post_text = "My toe nearly broke today, as did my spirit. I live on another day";
    if (argc >= 2) {
        title = argv[1];
        post_text = argv[2];
    }

    sprintf(new_post,"\t<div id=\"post%d\" class=\"post\">\n\t\t<h2>%s</h2>\n\t\t<p>%s</p>\n\t</div>\n\t<div class=\"spacer\"></div>\n",new_post_id,title,post_text);
    int result = insert_at_line(INDEX_FILE, line_to_insert_new_post, new_post);

    fclose(fp);
    if (result == 0) {
        printf("added new post\n");
    }
    else {
        fprintf(stderr,"failed to add new post\n");
    }

    return EXIT_SUCCESS;
}
