int text;

void main(){
    int loop;
    text = 0;
    loop = 10;

    while(loop > 1){
        loop = loop - 1;
        if(loop==0){
            print loop;
        }
        else{
            print text;
        }
    }
    do{
        loop = loop + 1;
    } while(loop < 10);
    print loop;
}