#include "api.h"
#include <stdio.h>

// The testdata only contains the first 100 mails (mail1 ~ mail100)
// and 2000 queries for you to debug.

int n_mails, n_queries;
mail *mails;
query *queries;

struct node{
    char token[20];
    int hash;
    //int use;
    struct node *next;
};
typedef struct node Node;

void similar(int qid, int mid, double threshold);
int bucket_index_calculator(char word[]);
int string_match(Node *n1, Node *n2);
void preprocess();
Node ***bucket;
int *w_q;

int main(void) {
	api.init(&n_mails, &n_queries, &mails, &queries);
    preprocess();
    for(int i = 0; i < n_queries; i++){
        if(queries[i].type == expression_match){
            //api.answer(queries[i].id, NULL, 0);
            //fprintf(stderr, "%d ", mails[i].id);
        }
        if(queries[i].type == find_similar){
            //fprintf(stderr, "1111\n");
            //fprintf(stderr, "thres=%lf\n", queries[i].data.find_similar_data.threshold);
            if(queries[i].data.find_similar_data.threshold < 0.1)
                similar(queries[i].id, queries[i].data.find_similar_data.mid, queries[i].data.find_similar_data.threshold);
                //fprintf(stderr, "%lf\n", queries[i].data.find_similar_data.threshold);
            //if(queries[i].data.find_similar_data.mid==9451)
            //similar(queries[i].id, queries[i].data.find_similar_data.mid, queries[i].data.find_similar_data.threshold);
            //api.answer(queries[i].id, NULL, 0);
        }
    }

  return 0;
}

void preprocess(){
    //fprintf(stderr, "preprocess\n");
    
    bucket = malloc(sizeof(Node **) * n_mails);
    for (int i = 0; i < n_mails; i++) {
        bucket[i] = malloc(sizeof(Node *) * 1296);
    }
    //long long int w_list[n_mails];
    w_q = malloc(n_mails * sizeof(int));
    for(int i = 0; i < n_mails; i++){
        //fprintf(stderr, "%d-th mail\n", i);
        //Node **bucket;
        //bucket = malloc(1296 * sizeof(Node *));
        //fprintf(stderr, "%p\n", bucket);
        Node *current;
        for (int j = 0; j < 1296; j++)
            bucket[i][j]=NULL;
        int index = 0;
        long long int t_m_quant = 0;
        int bucket_index = -1;
        int mod = 100001;
        Node *new = (Node *) malloc(sizeof(Node));
        for (int a = 0; a < 20; a++)
            new->token[a] = '\0';
        new->hash = 0;
        for (int j = 0; j < 256; j++){       //process subject of mid
            if(index == 19 && (mails[i].subject[j] >= '0' && mails[i].subject[j] <= '9' || mails[i].subject[j] >= 'a' && mails[i].subject[j] <= 'z' || mails[i].subject[j] >= 'A' && mails[i].subject[j] <= 'Z'))
                continue;
            if(!(mails[i].subject[j] >= 'A' && mails[i].subject[j] <= 'Z') && !(mails[i].subject[j] >= 'a' && mails[i].subject[j] <= 'z') && !(mails[i].subject[j] >= '0' && mails[i].subject[j] <= '9')){
                if(index == 0)
                    continue;
                bucket_index = bucket_index_calculator(new->token);
                
                int repeat = 0;
                if(bucket[i][bucket_index] != NULL){
                    current = bucket[i][bucket_index];
                    while(current->next != NULL){
                        if (string_match(current, new) == 0)
                            current=current->next;
                        else{
                            free(new);
                            repeat = 1;
                            break;
                        }
                    }
                    if (repeat == 0 && string_match(current, new) == 0)
                        current->next = new;
                    else if(repeat == 0){
                        free(new);
                        repeat = 1;
                    }
                    
                }else
                    bucket[i][bucket_index] = new;
               
                
                //for(int aa = 0; aa < index; aa++)
                    //fprintf(stderr, "%c", new->token[aa]);
                //fprintf(stderr, "\n");
                if(repeat == 0)
                    t_m_quant += 1;
                if(mails[i].subject[j] == '\0')
                    break;
                
                new = (Node *) malloc(sizeof(Node));
                
                for (int a = 0; a < 20; a++)
                    new->token[a] = '\0';
                new->hash = 0;
                index = 0;
            }else{
                if(mails[i].subject[j] >= 'A' && mails[i].subject[j] <= 'Z'){
                    new->token[index] = (char)((int)mails[i].subject[j] + (int)32);
                    new->hash = (new->hash * 36 + (mails[i].subject[j] - 'A') + 10) % mod;
                    index += 1;
                }else if(mails[i].subject[j] >= 'a' && mails[i].subject[j] <= 'z'){
                    new->token[index] = mails[i].subject[j];
                    new->hash = (new->hash * 36 + (mails[i].subject[j] - 'a') + 10) % mod;
                    index += 1;
                }else if(mails[i].subject[j] >= '0' && mails[i].subject[j] <= '9'){
                    new->token[index] = mails[i].subject[j];
                    new->hash = (new->hash * 36 + (mails[i].subject[j] - '0')) % mod;
                    index += 1;
                }
            }
        }
        
        new = (Node *) malloc(sizeof(Node));
        
        for (int a = 0; a < 20; a++)
            new->token[a] = '\0';
        new->hash = 0;
        index = 0;
        
        for (int j = 0; j < 100000; j++){
            if(index == 19 && (mails[i].content[j] >= '0' && mails[i].content[j] <= '9' || mails[i].content[j] >= 'a' && mails[i].content[j] <= 'z' || mails[i].content[j] >= 'A' && mails[i].content[j] <= 'Z'))
                continue;
            if(!(mails[i].content[j] >= 'A' && mails[i].content[j] <= 'Z') && !(mails[i].content[j] >= 'a' && mails[i].content[j] <= 'z') && !(mails[i].content[j] >= '0' && mails[i].content[j] <= '9')){
                if(index == 0)
                    continue;
                bucket_index = bucket_index_calculator(new->token);
                
                int repeat = 0;
                if(bucket[i][bucket_index] != NULL){
                    current = bucket[i][bucket_index];
                    while(current->next != NULL){
                        if(string_match(current, new) == 0)
                            current=current->next;
                        else{
                            free(new);
                            repeat = 1;
                            break;
                        }
                    }
                    if(repeat == 0 && string_match(current, new) == 0)
                        current->next = new;
                    else if(repeat == 0){
                        free(new);
                        repeat = 1;
                    }
                }else
                    bucket[i][bucket_index] = new;
                
                    
                //for(int aa = 0; aa < index; aa++)
                    //fprintf(stderr, "%c", new->token[aa]);
                //fprintf(stderr, "\n");
                if(repeat == 0)
                    t_m_quant += 1;
                if(mails[i].content[j] == '\0')
                    break;
                
                new = (Node *) malloc(sizeof(Node));
                
                for (int a = 0; a < 20; a++)
                    new->token[a] = '\0';
                new->hash = 0;
                index = 0;
            }else{
                if(mails[i].content[j] >= 'A' && mails[i].content[j] <= 'Z'){
                    new->token[index] = (char)((int)mails[i].content[j] + (int)32);
                    new->hash = (new->hash * 36 + (mails[i].content[j] - 'A') + 10) % mod;
                    index += 1;
                }else if(mails[i].content[j] >= 'a' && mails[i].content[j] <= 'z'){
                    new->token[index] = mails[i].content[j];
                    new->hash = (new->hash * 36 + (mails[i].content[j] - 'a') + 10) % mod;
                    index += 1;
                }else if(mails[i].content[j] >= '0' && mails[i].content[j] <= '9'){
                    new->token[index] = mails[i].content[j];
                    new->hash = (new->hash * 36 + (mails[i].content[j] - '0')) % mod;
                    index += 1;
                }
            }
        }
        
        /*if(i==0){
            fprintf(stderr, "%p\n", bucket);
            for(int j=0;j<1296;j++){
                if(bucket[j]!=NULL){
                    for(int k=0;k<20;k++)
                        fprintf(stderr, "%c", bucket[j]->token[k]);
                    fprintf(stderr, "\n");
                }
            }
            fprintf(stderr, "\n");
        }*/
        
        //bucket_list[i] = bucket;
        w_q[i] = t_m_quant;
        //fprintf(stderr, "tm=%d ", t_m_quant);
        //fprintf(stderr, "wq=%d ", w_q[i]);
        /*if(i==9451){
            //fprintf(stderr,"wq0=%d tmquant=%d ", w_q[9451], t_m_quant);
            for(int k=0;k<n_mails;k++)
                fprintf(stderr, "%d ", w_q[k]);
        }*/
            
        
        
        
    }
    /*fprintf(stderr, "%p\n", bucket_list[0]);
    fprintf(stderr, "%p\n", bucket_list[1]);*/
    /*Node *test;
    for(int i=0; i<1296;i++){
        fprintf(stderr, "%d: ", i);
        test = bucket[4622][i];
        if(test!=NULL){
            for(int k=0;k<20;k++)
                fprintf(stderr, "%c", test->token[k]);
            fprintf(stderr, " ");
            test=test->next;
        }
    }
    for(int i=0;i<1296;i++){
        if(bucket[4622][i]==NULL)
            fprintf(stderr, "%d ", i);
    }*/
    return;
}

void similar(int qid, int mid, double threshold){
    //fprintf(stderr, "mid=%d qid=%d threshold=%lf\n", mid, qid, threshold);
    int ans[n_mails];
    int ans_index=0;
    //Node *bucket[1296];
    Node *current;
    //for (int i = 0; i < 1296; i++)
        //bucket[i]=NULL;
    //int t_m_quant=0;
    int index = 0;
    int bucket_index = -1;
    int mod = 100001;
    Node *new = (Node *) malloc(sizeof(Node));
    for (int a = 0; a < 20; a++)
        new->token[a] = '\0';
    new->hash = 0;
    
    //for(int i = 0; i < 100; i++)
        //fprintf(stderr, "1111\n");
    for(int i = 0; i < n_mails; i++){
        //fprintf(stderr ,"%d-th mail\n", i);
        //Node **mid_bucket;
        //mid_bucket = bucket_list[mid];
        int bunion = w_q[mid];
        int intersect = 0;
        if(i != mid){
            //Node **compare_bucket;
            //compare_bucket = bucket_list[i];
            bunion += w_q[i];
            /*if(mid_bucket[772]==NULL)
                fprintf(stderr, "empty");
            else
                fprintf(stderr, "not empty\n");*/
            //fprintf(stderr, "mid wordscount= %d bunion=%d mid=%d\n", w_q[mid], bunion, mid);
            /*if(i==0){
                Node *test;
                for(int q=0;q<1296;q++){
                    if(bucket[9451][q]!=NULL){
                        test = bucket[9451][q];
                        while(test!=NULL){
                            for(int t=0;t<30;t++){
                                if(test->token[t] == '\0')
                                    break;
                                fprintf(stderr,"%c", test->token[t]);
                            }
                            test=test->next;
                            fprintf(stderr, " ");
                        }
                    }
                }
            }*/
            
            /*for(int j = 0; j<1296;j++){
                if(bucket[i][j]==NULL)
                    fprintf(stderr, "%d ", j);
            }*/
            
            for(int j = 0; j < 1296; j++){
                //fprintf(stderr, "j=%d", j);
                Node *compare_current, *mid_current;
                if(bucket[i][j] != NULL && bucket[mid][j] != NULL){
                    compare_current = bucket[i][j];
                    while(compare_current != NULL){
                        //fprintf(stderr, "1111\n");
                        int same = 0;
                        mid_current = bucket[mid][j];
                        while(mid_current != NULL){
                            //fprintf(stderr, "2222\n");
                            same = string_match(compare_current, mid_current);
                            if(same == 0)
                                mid_current = mid_current->next;
                            else if(same == 1){
                                //fprintf(stderr, "3333\n");
                                
                                bunion -= 1;
                                intersect += 1;
                                break;
                            }
                        }
                        compare_current = compare_current->next;
                        //fprintf(stderr, "4444\n");
                    }
                    
                }
                if((double)intersect / (double)bunion > threshold)
                    break;
            }
            
            if((double)intersect / (double)bunion > threshold){
                ans[ans_index] = mails[i].id;
                ans_index += 1;
                //fprintf(stderr, "intersect=%d bunion=%d", intersect, bunion);
                //fprintf(stderr, "add %d ", mails[i].id);
            }
        }
    }
    
    //for(int i=0;i<ans_index;i++)
        //fprintf(stderr, "%d ", ans[i]);
    
    api.answer(qid, ans, ans_index);
    return;
    
}

int bucket_index_calculator(char word[]){
    if(word[1] == '\0'){
        if(word[0] >= '0' && word[0] <= '9')
            return word[0] - '0';
        else if(word[0] >= 'a' && word[0] <= 'z')
            return word[0] - 'a' + 10;
    }else{
        if(word[1] >= '0' && word[1] <= '9'){
            if(word[0] >= '0' && word[0] <= '9')
                return (word[1] - '0') * 36 + word[0] - '0';
            else if(word[0] >= 'a' && word[0] <= 'z')
                return (word[1] - '0') * 36 + word[0] - 'a' + 10;
        }else if(word[1] >= 'a' && word[1] <= 'z'){
            if(word[0] >= '0' && word[0] <= '9')
                return (word[1] - 'a' + 10) * 36 + word[0] - '0';
            else if(word[0] >= 'a' && word[0] <= 'z')
                return (word[1] - 'a' + 10) * 36 + word[0] - 'a' + 10;
        }
    }
    return 0;
}

int string_match(Node *n1, Node *n2){
    if(n1->hash != n2->hash)
        return 0;
    for(int i = 0; i < 20; i++){
        if(n1->token[i] == '\0' && n2->token[i] != '\0' || n1->token[i] != '\0' && n2->token[i] == '\0')
            return 0;
        else if (n1->token[i] != n2->token[i])
            return 0;
        else if(n1->token[i] == '\0' && n2->token[i] == '\0')
            break;
    }
    
    return 1;
}

int count(char article[]){
    int number = 0;
    int word = 0;
    int i = 0;
    while(article[i] != '\0'){
        if(article[i] >= '0' && article[i] <= '9' || article[i] >= 'a' && article[i] <= 'z'|| article[i] >= 'A' && article[i] <= 'Z'){
            number += 1;
        }else{
            if(number > 0)
                word += 1;
            number = 0;
        }
        i += 1;
    }
    if (number > 0)
        word += 1;
    return word;
}
