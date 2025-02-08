#include <bits/stdc++.h>
using namespace std;

int fifo=0, lru=0;

void FIFO() {
    int n,size;
    FILE *f;
    f = fopen("inp.txt", "r");
    fscanf(f,"%d\n%d",&n,&size);

    vector<int> frames(n,-1), queue;
    
    int add;
    while(fscanf(f,"%d\n",&add) != EOF) {
        bool present=false;
        // check if the address is already present
        for(int i=0; i<n; i++) {
            if(frames[i]==add) {
                present = true;
                break;
            }
        }
        if(present) continue;
        // address is not already present in the page table
        queue.push_back(add);
        // page fault occurs
        fifo++;
        // check for empty entries
        for(int i=0; i<n; i++) {
            if(frames[i] == -1) {
                frames[i] = add;
                present = true;
                break;
            }
        }
        if(present) continue;;
        // no empty entry was present
        // remove the address that came first
        int first = queue.front();
        queue.erase(queue.begin());

        for(int i=0; i<n; i++) {
            if(frames[i] == first) {
                frames[i] = add;
                break;
            }
        }
    }
    fclose(f);
}

void LRU() {
    int n,size;
    FILE *f;
    f = fopen("inp.txt", "r");
    fscanf(f,"%d\n%d",&n,&size);

    vector<int> frames(n,-1), queue;
    
    int add;
    while(fscanf(f,"%d\n",&add) != EOF) {
        bool present=false;
        // check if the address is already present
        for(int i=0; i<n; i++) {
            if(frames[i]==add) {

                // this frame is most recently used, so push to the back of the queue
                queue.erase(queue.begin() + i);
                queue.push_back(add);

                present = true;
                break;
            }
        }
        if(present) continue;
        // address is not already present in the page table
        queue.push_back(add);
        // page fault occurs
        lru++;
        // check for empty entries
        for(int i=0; i<n; i++) {
            if(frames[i] == -1) {
                frames[i] = add;
                present = true;
                break;
            }
        }
        if(present) continue;;
        // no empty entry was present
        // remove the least recently used address, present at front of the queue
        int first = queue.front();
        queue.erase(queue.begin());

        for(int i=0; i<n; i++) {
            if(frames[i] == first) {
                frames[i] = add;
                break;
            }
        }
    }

    fclose(f);
}

int main() {

    FIFO();
    LRU();

    FILE* f_out = fopen("Out.txt","w");
    fprintf(f_out,"Number of page faults with FIFO: %d\n",fifo);
    fprintf(f_out,"Number of page faults with LRU: %d\n",lru);
    fclose(f_out);

    return 0;
}