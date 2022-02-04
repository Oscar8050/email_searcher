#include "api.h"
#include <stdio.h>

int n_mails, n_queries;
mail *mails;
query *queries;

int main(){
    //printf("at the begging\n");
    api.init(&n_mails, &n_queries, &mails, &queries);
    for (int i=0;i<n_mails;i++)
        printf("%d ", mails[i].id);
    
    
    return 0;
}
