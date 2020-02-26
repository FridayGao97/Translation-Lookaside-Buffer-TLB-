//the command line: valgrind --tool=lackey --trace-mem=yes ./quicksort 100 2>&1 | ./valtlb379 -p FIFO 4096 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//initial the global var
int tlbsize;
long pgsize;
//count for hit, miss
long hit = 0;
long flash_hit = 0;
long miss = 0;

//the virtual table for TLB
int *TLB;
//used in LRU, store the most recent. its order of index is corresponding to TLB
int *TLBtime;

//First in, First out
int FIFO(int lastupdate, int pgnum){

  int exist = 0;

  //iterate all tlbsize, check whether pgnum exist before
  for(int i =0; i< tlbsize; i++){
    //if page exit before, will no action in TLB, but hit++;
    if(TLB[i] == pgnum){
      //printf("~~~hit !!\n");
      exist = 1;
      hit++;
      flash_hit++;
      break;
    }
  }
  //if not, first in, first out
  //if TLB not full, store to next empty space
  //if full => lasupdate > tlbsize; use mod to retrive back to head
  if(exist == 0){
    //printf("~~~miss !!\n");
    miss++;
    lastupdate = lastupdate +1;
    TLB[lastupdate % tlbsize] = pgnum;
  }
  
  //return lastupdate, always keep tracking the lastpage used
  return lastupdate;
}


//find the smallest element in array
int indexofSmallestElement(int array[], int size){
    int index = 0;
    for(int i = 1; i < size; i++)
    {
        if(array[i] < array[index])
            index = i;              
    }

    return index;
}

//least recent use, to find the smallest val in TLBtime
// the order in TLBtime is same as in TLB, which means the index (position) for page num is same.
void LRU(long count, int pgnum){

  int exist = 0;
  //check exist before
  for(int i =0; i< tlbsize; i++){
    //if exist, hit++, update TLBtime by current count, (count always increment by processing)
    if(TLB[i] == pgnum){
      exist = 1;
      TLBtime[i] = count;
      hit++;
      flash_hit++;
      // printf("~~~HIT !!\n");
      break;
    }
  }

  //if not exist, should insert page number to TLB, and update also in TLBtime
  if(exist == 0){
    miss++;
    //for the TLB table not full
    if((count -hit) <= tlbsize){
      // -1 since the index start from 0
      TLB[count - hit - 1] = pgnum;
      TLBtime[count - hit -1] = count;
    }
    //TLB table is full
    else{
      //if full, must replace least recent use
      // printf("full tlb !!\n");
      int indx = indexofSmallestElement(TLBtime,tlbsize);
      // printf("index: %d | pgnum :%d!!\n",indx,pgnum);
      TLB[indx] = pgnum;
      TLBtime[indx] = count;
    }
  }

}




int main(int argc, char * argv[]) {

  //get the valgrind input,line by line
  char input[100] = {0};
  
  //initial -f, -i
  int flushperiod = -1;
  int iflag = 0;

  //get the page size, and TLB size
  pgsize = atol(argv[argc-2]);
  tlbsize = atoi(argv[argc-1]);

  //get the -p policy requirement
  char policy[5];
  strcpy(policy,argv[argc-3]);

  //set the empty TLB array
  //https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/
  TLB = (int*)malloc((tlbsize) * sizeof(int)); 
  //for LRU
  TLBtime = (int*)malloc((tlbsize) * sizeof(int));

  memset(TLB, -1, sizeof((tlbsize) * sizeof(int)));
  memset(TLBtime,-1,sizeof((tlbsize) * sizeof(int)));

  //when [-i]flag exist
  if(strcmp(argv[1],"-i") == 0){
    //read data with ignoring I
    printf("i: \n");
    iflag =1;
  }

  //check -f flushperiod present or not
  else if(strcmp(argv[1],"-f") == 0){
    flushperiod = atoi(argv[2]);
  }

  if(strcmp(argv[2],"-f") == 0){
    flushperiod = atoi(argv[3]);
  }

  printf("tlbsize, %d\n",tlbsize);
  printf("pgsize, %ld\n",pgsize);
  printf("policy: %s\n",policy);
  printf("iflag, %d\n",iflag);
  printf("flushperiod, %d\n",flushperiod);
  //printf("2: %s\n",argv[1]);
  int lastupdate = -1;
  long countpages = 0;
  long totalpage = 0;
   
  //get valgrind line from stdin
  while(fgets(input, sizeof(input), stdin)){
    
    // printf("input: %s\n",input);
    //check & get the vaild address  
    //if vaild address line
    if(input[0] == 'I' || input[1] =='L'|| input[1] =='S'|| input[1] =='M'){
      //
      //printf("!start getting address\n");
      int pgnum;
      int pgnum2 = -1;
      char adstr[50];
      int i = 3;
      while(input[i] != ','){
        adstr[i-3] = input[i];
        i++;
      }
      adstr[i-3] = '\0';
      char offset[5];

      int j = 0;
      while(input[i+1] != '\0'){
        offset[j] = input[i+1];
        i++;
        j++;
      }
      offset[j] = '\0';
      
    
      //get the memory address
      long address = strtoul(adstr, NULL, 16);
      //get the memory address offset
      long addoff =address + strtoul(offset, NULL, 16);
      //calculate the page number and after adding offset page number
      pgnum = (int) address/pgsize;
      pgnum2 = (int) addoff/pgsize;
      //check if after adding offset, they are still same page
      if(pgnum2 == pgnum){
        pgnum2 = -1;
      }

      //ignore I address,instruction
      if(iflag == 1){
        //printf("!-i flag\n");
        if(input[0] != 'I'){
          //count how many page are handled
          countpages++;
          totalpage ++;

          if(strcmp(policy,"LRU") == 0){

            LRU(countpages,pgnum);
            //if adding address offset going to next page, then also load next page
            if(pgnum2 != -1){
              totalpage++;
              countpages++;
              LRU(countpages,pgnum2);
            }
            
          }
          else if(strcmp(policy,"FIFO") == 0){

            lastupdate = FIFO(lastupdate,pgnum);
            if(pgnum2 != -1){
              //if adding address offset going to next page, then also load next page
              totalpage++;
              countpages++;
              lastupdate = FIFO(lastupdate,pgnum2);
            }

          }
        }
      }
      //count all address
      else{
        //printf("! no -i flag,fetch all.\n");
        //count how many page are handled
        countpages++;
        totalpage ++;

        if(strcmp(policy,"LRU") == 0){
            LRU(countpages,pgnum);
            if(pgnum2 != -1){
              totalpage++;
              countpages++;
              LRU(countpages,pgnum2);
            }
        }
        else if(strcmp(policy,"FIFO") == 0){

          lastupdate = FIFO(lastupdate,pgnum);
          if(pgnum2 != -1){
              totalpage++;
              countpages++;
              lastupdate = FIFO(lastupdate,pgnum2);
            }

        }
      }
      //check the condition for -f , if exist then flush the TLB
      if(countpages == flushperiod){
        memset(TLB, -1, sizeof((tlbsize) * sizeof(int)));
        memset(TLBtime,-1,sizeof((tlbsize) * sizeof(int)));
        countpages = 0;
        hit = 0;
        lastupdate = -1;
      }
      

    }
    
  }
  if(flushperiod != -1){
    printf("flash Hit: %ld\n",flash_hit);
  }
  else{
    printf("Hit: %ld\n",hit);
  }
  
  printf("Miss: %ld\n",miss);
  printf("Total reference: %ld\n",totalpage);
  free(TLBtime);
  free(TLB);
  
  return 0;
}
