#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void int_to_bytes(unsigned long int n, unsigned char *p, int bit){
        for(int i = 0; i < 4; i++){
           p[i] =  (n << (bit*i)) >> 24;
        }
};
int bytes_to_int(unsigned char* p, int bit){
    unsigned int n = 0;
    for(int i = 0; i < 4; i++){
        n += p[3-i] << (bit*i);
    }
    return n;
};
struct header{
    char marker[3];
    char version[1];
    char sub_version[1];
    char flags[1];
    char size[4];
};
struct frame{
    char marker[4];
    char size[4];
    char flags[2];
    char a[1];
};
int show(FILE* a, struct frame fram, int size_of_header_int){
    while(ftell(a) <= size_of_header_int){
        fread(&fram, sizeof(char), sizeof(fram), a);
        if(bytes_to_int(fram.marker, 8) == 0) break;
        int size_of_fram = bytes_to_int(fram.size, 8);
        printf("Frame marker:%s ", fram.marker);
        for(int i = 1; i < size_of_fram; i++){
            char t = fgetc(a);
            printf("%c", t);
        }
        printf("\n");
    }
    return 0;
}
int get(FILE* a, struct frame fram, int size_of_header_int, char* prop_nam){
    char simbol = 0;
    while(ftell(a) <= size_of_header_int){
        fread(&fram, sizeof(char), sizeof(fram), a);
        int size_of_fram;
        if((bytes_to_int(fram.marker, 8)) == 0) break;
        if(strcmp(fram.marker, prop_nam) == 0){
            size_of_fram = bytes_to_int(fram.size, 8);
            printf("Frame marker:%s ", fram.marker);
            for(int i = 1; i < size_of_fram; i++){
                char t = fgetc(a);
                printf("%c", t);
                }
            printf("\n");
            simbol = 1;
            return 0;
        }
        if(memcmp(fram.marker, prop_nam, sizeof(prop_nam)) != 0){
            size_of_fram = bytes_to_int(fram.size, 8);
            for(int i = 1; i < size_of_fram; i++){
                char t = fgetc(a);
                }
        }
    }
    if(simbol == 0) printf("there is no such frame id");
    return 0;
}
void set(struct header header, struct frame fram, FILE* ne, FILE* a, int size_of_header_int, char* prop_nam, char* prop_value){
    int chek = 0;
    while(ftell(a) <= size_of_header_int){
        fread(&fram, sizeof(char), sizeof(fram), a);
       if(bytes_to_int(fram.marker, 8) == 0) break;
        int cur_frame_size = bytes_to_int(fram.size, 8);
        if(bytes_to_int(fram.marker, 8) != 0  && memcmp(fram.marker, prop_nam, strlen(prop_nam)) == 0){
            chek = 1;
            for(int i = 1; i < cur_frame_size; i++){
            char t = fgetc(a);
        }
            int new_fram_size_in_int = strlen(prop_value) + 1;
            char* nen;
            nen = (char*) calloc(new_fram_size_in_int, 1);
            nen = prop_value;
            int_to_bytes(new_fram_size_in_int, fram.size, 8);
            int new_size_of_header_in_int = size_of_header_int - cur_frame_size + new_fram_size_in_int;
            int_to_bytes(new_size_of_header_in_int, header.size, 7);
            fseek(ne, 0, SEEK_SET);
            fwrite(&header, sizeof(char), sizeof(header), ne);
            fseek(ne, 0, SEEK_END);
            fwrite(&fram, sizeof(char), sizeof(fram), ne);
            fwrite(nen, sizeof(char), new_fram_size_in_int, ne);
            free(nen);
             }
       if(bytes_to_int(fram.marker, 8) != 0  && memcmp(fram.marker, prop_nam, strlen(prop_nam)) != 0){
           int size_of_fram = bytes_to_int(fram.size, 8);
           fwrite(&fram, sizeof(char), sizeof(fram), ne);
            for(int i = 1; i < size_of_fram; i++){
                char t = fgetc(a);
                fputc(t, ne);
                }
       }
    }
    if(chek == 0){
            int new_fram_size_in_int = strlen(prop_value) + 1;
            char* nen;
            nen = (char*) calloc(new_fram_size_in_int, 1);
            nen = prop_value;
            int_to_bytes(new_fram_size_in_int, fram.size, 8);
            fwrite(prop_nam, sizeof(char), sizeof(prop_nam), ne);
            fwrite(fram.size, sizeof(char), sizeof(fram.size), ne);
            fwrite(fram.flags, sizeof(char), sizeof(fram.flags), ne);
            fwrite(fram.a, sizeof(char), sizeof(fram.a), ne);
            fwrite(nen, sizeof(char), new_fram_size_in_int, ne);
    }
}


int main(int argc, char* argv[])
{
   struct header header;
   struct frame frame;
    if(argc < 3) printf("Error number of arguments");
    char* istr;
    istr = strstr(argv[1], "--filepath=");
    if(istr == NULL){
        printf("Error Name of mp3");
        return 1;
    };
    char* name;
    name = strtok(argv[1], "=");
    name = strtok(NULL, "=");
    char* nam;
    nam = (char*)calloc(strlen(name), sizeof(char));
    strcpy(nam, name);
    FILE* mp3;
    if((mp3 = fopen(nam,"rb")) == NULL) printf("can't open file");
    fread(&header, sizeof(char), sizeof(header), mp3);
    int size_of_header_in_int = bytes_to_int(header.size, 7);
    if(memcmp(header.marker, "ID3", 3) != 0) printf("Uncorect format of file");

    if(strcmp(argv[2], "--show") == 0){
        show(mp3, frame, size_of_header_in_int);
    }
    if(memcmp(argv[2], "--get", 5) == 0){
        char* prop_name;
        prop_name = strtok(argv[2], "=");
        prop_name = strtok(NULL, "=");
        get(mp3, frame, size_of_header_in_int, prop_name);

    }
    if(memcmp(argv[2], "--set", 5) == 0){
        FILE* ne = fopen("NE.txt","wb");
        char* prop_name;
        prop_name = strtok(argv[2], "=");
        prop_name = strtok(NULL, "=");
        char* prop_value;
        prop_value = strtok(argv[3], "=");
        prop_value = strtok(NULL, "=");
        set(header, frame, ne, mp3, size_of_header_in_int, prop_name, prop_value);
    }
     fclose(mp3);
     free(nam);
     return 0;
}
