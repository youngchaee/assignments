#include <stdio.h>
#include <string.h>

int find(char* alphabet, char c,int num){
    for (int i=0;i<num;i++){
        if(c==*(alphabet+i)||*(alphabet+i)=='\0'){
            return i;
        }
    }
    return -1;
}


int main(int argc, char *argv[]){
    FILE* fp;
    fp=fopen(argv[1],"r");
    if (fp==NULL){
        printf("file not opened");
        return 0;
    }
    char cipher[20000];
    
    int length=0, IC=0;
    fgets(cipher,20000,fp);
    for (int i=0;i<20000;i++){
        if(cipher[i]=='\0'){
            break;
        }
        length++;
    }
    int keylength;
    for (keylength=1;keylength<16;keylength++){
        char alphabet[95]={0,};
        int count[95]={0,};
        double IC=0;
        for (int i=0;i<length;i+=keylength){
            int j=find(alphabet,cipher[i],95);
            if(j==-1){
                break;
            }
            alphabet[j]=cipher[i];
            count[j]++;
        }
        for (int i=0;i<95;i++){
            if (count[i]==0){
                break;
            }
            
            IC+=count[i]*(count[i]-1);
        }
        if(length/keylength==0){
            continue;
        }
        IC/=(length/keylength)*(length/keylength-1);
        
        if (IC>=0.060){
            break;
        }
    } //keylength finding ends
    
    char key[16]={0,};
    for (int i=0;i<keylength;i++){
        int max[5]={1,2,3,4,5},e[5]={0,};
        int count[95]={0,};
        char alphabet[95]={0,};
        char frequent_char[7]={'e',' ','t','a','o','i','n'};
        for (int j=i;j<length;j+=keylength){
            int k=find(alphabet,cipher[j],95);
            if(k==-1){
                break;
            }
            alphabet[k]=cipher[j];
            count[k]++;
            
        }
        for (int j=0;j<95;j++){
            int min=max[0],min_index=0;
            for(int k=0;k<5;k++){
                if(max[k]<min){
                    min=max[k];
                    min_index=k;
                }   
            }
            if(max[min_index]<count[j]){
                max[min_index]=count[j]; // the number of the five most frequent letters
                e[min_index]=alphabet[j]; // the five most frequent letters correspond to above line
            }
        }
        
        for (int j=0;j<5;j++){
            int num=0;
            key[i]='e'^e[j];
            for (int k=0;k<5;k++){
                if (find(frequent_char,key[i]^e[k],7)!=-1){
                    num++;
                }
            }
            if (num>4){
                break;
            }
        }
    }
    for (int i=0;i<keylength;i++){
        printf("0x%x ",key[i]);
    }
    printf("\n");
    for (int i=0;i<length;i++){
        printf("%c",cipher[i]^key[i%keylength]);
    }
}