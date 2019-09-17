#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <iomanip>

using namespace std;

constexpr int MBT_size = 1024;
constexpr int OS_reserve = 32;
static int PID;

/**
 * Information for MBT object
 *
 */
struct MBT{
    int free_blocks_available = MBT_size;
    bool blocks[MBT_size];
};

/**
 * Information for page table object
 *
 */
struct page_table{
    int *array_ptr = nullptr;
};

/**
 * Information for PCB object
 *
 */
struct PCB{
    int PID = 0;
    int ptable_size = 0;
    page_table *ptable_ptr = nullptr;
    PCB *next_pcb_ptr = nullptr;
};

/**
 * Information for Ready Queue object
 *
 */
struct rdy_Q{
    PCB *head_ptr = nullptr;
};

/**
 * Instantiates a MBT object
 *
 */
MBT init_MBT(){
    MBT mbt = {MBT_size - OS_reserve};
    for (int i = 0; i < OS_reserve; i++){
        mbt.blocks[i] = false;
    }
    for (int i = OS_reserve; i < MBT_size; i++){
        mbt.blocks[i] = true;
    }
    return mbt;
}

/**
 * Instantiates a Ready Queue object
 *
 */
rdy_Q init_Q (){
    rdy_Q queue = {.head_ptr = nullptr};
    return queue;
}

/**
 * Instantiates a Page Table object to hold the indexes of used blocks from MBT
 *
 * @param process_size The size of the random process
 * @return ptable The page table object
 */
page_table create_ptable(int process_size){
    page_table ptable = {new int [process_size]};
    return ptable;
}

/**
 * Instantiates a PCB object to hold the information of a process and its Page Table
 *
 * @param ptable The associated Page Table object
 * @param PID The ID for the specified process
 * @param ptable_size The size of the ptable
 * @return pcb The PCB object
 */
PCB create_PCB(page_table &ptable, int PID, int ptable_size){
    PCB pcb = {
            .PID = PID,
            .ptable_size = ptable_size,
            .ptable_ptr = &ptable,
            .next_pcb_ptr = nullptr
    };
    return pcb;
}

/**
 * Creates a random number using the given seed within the program
 *
 * @return size A random number from 10 - 250
 */
int generate_process_size(){
    int size = rand() % 250 + 10;
    return size;
}

/**
 * Creates a random but unique PID based on previously created PID's
 *
 * @return PID A unique ID for a specific process
 */
int generate_PID(){
    if(PID == 0){
        int num = rand() % 900000 + 100000;
        printf("PID: %d\n", num);
        return num;
    }else {
        PID += 1;
        printf("PID: %d\n", PID);
        return PID;
    }

}

/**
 * Loops through the MBT in order to find the first free block to use
 *
 * @param block_array a boolean array used to see which blocks in the MBT are being used
 * @return i The index of the first available block
 */
int first_available_block (bool *block_array) {
    for (int i = OS_reserve; i < MBT_size; i++) {
        if (block_array[i] == true) {
            return i;
        }
    }
    return 0;
}

/**
 * Loops through instantiated Page Table and dynamically assigns the free blocks of MBT to it's integer array
 *
 * @param ptable The associated Page Table object
 * @param first_block The currently first free block in the MBT
 * @param process_size The size of the specified process
 * @param mbt The Memory Block Table object
 */
void allocate_ptable(page_table &ptable, int first_block, int process_size, MBT &mbt) {
    for (int i = 0; i < process_size; i++) {
        for (int j = first_block; j < MBT_size; j++) {
            if (mbt.blocks[j] == true) {
                ptable.array_ptr[i] = j;
                mbt.blocks[j] = false; //allocate mbt
                break;
            }
        }
    }
    mbt.free_blocks_available -= process_size; //update free space available
    printf("Free blocks available: %d\n", mbt.free_blocks_available);
}

/**
 * Inserts a PCB into the Ready Queue object
 *
 * @param pcb The current PCB object
 * @param queue The Ready Queue Object
 */
void insert_to_Q(PCB &pcb, rdy_Q &queue){
    if(queue.head_ptr == nullptr){ //if queue is empty
        queue.head_ptr = &pcb;
    }else if(queue.head_ptr->next_pcb_ptr == nullptr){ // if only 1 pcb in queue
        queue.head_ptr->next_pcb_ptr = &pcb;
    }else{ // more than 2 pcb in queue
        PCB *temp = queue.head_ptr;
        while(temp->next_pcb_ptr != nullptr){
            temp = temp->next_pcb_ptr;
        }
        temp->next_pcb_ptr = &pcb;

    }
    puts("Successfully added to queue!");
}

/**
 * Initiates the entire process of creating a random process, Page Table, and PCB, associating each
 * Page Table to its PCB, and inserting to the queue
 *
 * @param mbt The MBT Object
 * @param queue The Ready Queue Object
 */
void init_process(MBT &mbt, rdy_Q &queue){
    puts("\nCreating new process...");
    int size = generate_process_size(); //generate random process
    printf("Process size: %d\n", size);

    if(mbt.free_blocks_available >= size){ //check sufficient space
        PID = generate_PID(); //generate unique PID
        page_table *ptable = new page_table(create_ptable(size)); //create page table
        allocate_ptable(*ptable, first_available_block(mbt.blocks), size, mbt); //allocate p_table
        PCB *pcb = new PCB(create_PCB(*ptable, PID, size)); //create PCB and save page_table
        insert_to_Q(*pcb, queue); //insert PCB to rdy_q
    }else{ // insufficient
        puts("Error: Not enough blocks available");
        printf("Free Blocks Available: %d\n",  mbt.free_blocks_available);
    }
}

/**
 * Displays the menu options to the user
 */
void display_menu(){
    puts("\n1. Initiate Process\n2. Print System State\n3. Terminate Process\n4. Exit");

}

/**
 * Displays the current state of the MBT to the user
 */
void display_MBT(MBT mbt){
    printf("\nMEMORY BLOCK TABLE\n");
    for(int i = 0; i < MBT_size; i++) {
        if(i % 32 != 0){
            continue;
        }else{
            cout << "[" << setw(4) << setfill('0') << i << ":" << setw(4) << setfill('0') << i + 31 << ']';
            for(int j = i; j < i+32; j++) {
                printf("[%c]", mbt.blocks[j] ? ' ':'x');
            }
            cout << endl;
        }
    }
    printf("\nIn Use: %d\n", MBT_size - mbt.free_blocks_available);
    printf("Free: %d\n", mbt.free_blocks_available);
}

/**
 * Displays the current state of the Ready Queue to the user
 */
bool display_Q(rdy_Q queue){
        PCB *temp = queue.head_ptr;
        puts("\nPROCESS IN QUEUE");
        if(temp == nullptr){
            puts("Queue is empty");
            return false;
        }else{
            while(temp != nullptr){
                printf("\nPID: %d\nSize: %d\n", temp->PID, temp->ptable_size);
                for(int i = 0; i < temp->ptable_size; i++){
                    printf("[%d:%d]\n", i, temp->ptable_ptr->array_ptr[i]);
                }
                temp = temp->next_pcb_ptr;
            }
            return true;
        }
}

/**
 * Takes a user's input and checks it against its correct type and range
 *
 * @param user_input The number given by the user
 * @param min The minimum valid number
 * @param max The maximum valid number
 * @return user_input The validated user's input
 */
int validate_user_input(int &user_input, int min, int max){
    while(true){
        if(!cin.fail()){ //user_input is a integer
            if(user_input >= min && user_input <= max){ //user_input within range
                return user_input;
            }else{ //user_input not within range
                printf("Please enter %d-%d\n", min, max);
                cin >> user_input;
            }
        }else{ //user choice not int
            puts("Please enter an integer");
            cin.clear();
            cin.ignore(std::numeric_limits<int>::max(),'\n');
            cin >> user_input;
        }
    }
}

/**
 * Asks the user for an input
 *
 * @param min The minimum valid number
 * @param max The maximum valid number
 * @return validate_user_input(user_input, min, max) The validated user's input
 */
int get_input(int min, int max){
    int user_input;
    cin >> user_input;
    return validate_user_input(user_input, min, max);
}

/**
 * Checks to see if the user selected process is in the Ready Queue
 *
 * @param queue The Ready Queue object
 * @param user_selected_PID The process selected by the user
 * @return true if process exists in the queue, false otherwise
 */
bool process_in_Q(rdy_Q &queue, int user_selected_PID){
    PCB *temp = queue.head_ptr;
    while(temp != nullptr){ //search through PCB until PID found
        if(user_selected_PID == temp->PID){
            return true;
        }
        temp = temp->next_pcb_ptr;
    }
    //PID does not exist
    return false;
}

/**
 * Frees up the specified memory blocks in the MBT associated with the Page Table
 *
 * @param pcb The PCB object
 * @param mbt The MBT object
 */
void dealloc_MBT(PCB &pcb, MBT &mbt){
    for(int i = 0; i < pcb.ptable_size; i++){
        mbt.blocks[pcb.ptable_ptr->array_ptr[i]] = "free";
    }
    mbt.free_blocks_available += pcb.ptable_size;
}

/**
 * Checks to see if the specified process is the first process in the queue
 *
 * @param queue The Ready Queue object
 * @param user_selected_PID The process specified by the user
 * @return true if first, false otherwise
 */
bool is_first(rdy_Q &queue, int user_selected_PID){
    if(queue.head_ptr->PID == user_selected_PID){
        return true;
    }else{
        return false;
    }
}

/**
 * Frees up dynamically allocated Page Table memory
 *
 * @param pcb The PCB object
 * @param mbt The MBT object
 */
void delete_ptable(PCB &pcb, MBT &mbt){
    dealloc_MBT(pcb, mbt); //free up blocks
    //release memory
    delete[] pcb.ptable_ptr->array_ptr;
    delete[] pcb.ptable_ptr;
    //pointers back to null
    pcb.ptable_ptr->array_ptr = nullptr;
    pcb.ptable_ptr = nullptr;
}

/**
 * Frees up dynamically allocated PCB memory. Called during specified termination
 *
 * @param queue The Ready Queue object
 * @param user_selected_PID The process specified by the user
 * @param mbt The MBT object
 */
void delete_PCB(rdy_Q &queue, int user_selected_PID, MBT &mbt){
    //process always exists if this function is called
    if(is_first(queue, user_selected_PID)){ //if deleting first pcb in queue
        PCB *to_del = queue.head_ptr; //assign to to_del variable used for deletion
        printf("Deleting process: %d\n", to_del->PID);
        delete_ptable(*queue.head_ptr, mbt);
        queue.head_ptr = queue.head_ptr->next_pcb_ptr; //assign head to next pcb
        delete to_del;
        to_del->next_pcb_ptr = nullptr;
    }else{ //if deleting after first pcb in queue
        PCB *temp = queue.head_ptr;
        while(temp != nullptr){ //search through PCB until nullptr
            if(temp->next_pcb_ptr->PID == user_selected_PID){ //if the PID matches
                PCB *to_del = temp->next_pcb_ptr; //assign next pcb to to_del variable for deletion
                printf("Deleting process: %d\n", to_del->PID);
                temp->next_pcb_ptr = to_del->next_pcb_ptr; //assign current temp's next pcb to the deleted pcb's next pcb
                delete_ptable(*to_del, mbt);
                delete to_del;
            }
            temp = temp->next_pcb_ptr; //iterate pcb
        }
    }
}

/**
 * Displays a list of current PIDs in Ready Queue
 *
 * @param queue The Ready Queue object
 */
void display_PID(rdy_Q &queue){
    PCB *temp = queue.head_ptr;
    while(temp != nullptr){
        printf("Process in queue: %d\n", temp->PID);
        temp = temp->next_pcb_ptr;
    }
}

/**
 * Terminates the process specified by the user
 *
 * @param queue The Ready Queue object
 * @param mbt The MBT object
 */
void terminate_process(rdy_Q &queue, MBT &mbt){
    bool exit = false;
    display_PID(queue);
    if(queue.head_ptr != nullptr){ //non-empty queue
        while(!exit){
            puts("\nType in the ID you wish to delete: ");
            int user_input = get_input(100000, 999999);
            if(process_in_Q(queue, user_input)){ //PID exists
                delete_PCB(queue, user_input, mbt);
                exit = true;
            }else{ //PID does not exist
                puts("Process not in queue. Please enter a different PID");
            }
        }
    }else{ //empty queue
        puts("Queue is empty");
    }
}

/**
 * Frees up dynamically allocated PCB memory. Called during mass termination
 *
 * @param queue The Ready Queue object
 * @param mbt The MBT object
 */
void delete_PCB(PCB &pcb, MBT &mbt){
    pcb.next_pcb_ptr = nullptr;
    printf("Deleting Process: %d\n", pcb.PID);
    delete_ptable(pcb, mbt);
}

/**
 * Terminates every process in Ready Queue
 *
 * @param queue The Ready Queue object
 * @param user_selected_PID The process specified by the user
 * @param mbt The MBT object
 */
void terminate_all(rdy_Q &queue, MBT &mbt){
    //always delete from head
    while(true){
        if(queue.head_ptr == nullptr){ //empty queue
            break;
        }else{ //non-empty queue
            PCB *temp = queue.head_ptr;
            queue.head_ptr = queue.head_ptr->next_pcb_ptr; //assign head to next pcb every time head is deleted
            delete_PCB(*temp, mbt);
        }
    }
}

/**
 * Asks the user to confirm termination of all processes
 *
 * @param queue The Ready Queue object
 * @param mbt The MBT object
 */
bool exit_program(rdy_Q &queue, MBT &mbt){
    if(queue.head_ptr != nullptr){ //non-empty queue
        display_PID(queue);
        puts("Do you wish to close all processes?\n1: yes\n2: no\n");
        int user_input = get_input(1, 2);
        switch(user_input){
            case 1:
                terminate_all(queue, mbt);
                return true;
            case 2:
                return false;
            default:
                break;
        }
    }else{ //empty queue
        puts("Queue is empty");
        return true;
    }
    return false;
}

/**
 * Main function to run the entire program
 */
int main() {
    //initialization
    srand(time(nullptr)); //seed for random number generator
    MBT mbt = init_MBT();
    rdy_Q queue = init_Q();

    //menu
    bool exit = false;
    while(!exit){
        display_menu();
        int user_input = get_input(1, 4);
        switch (user_input){
            case 1:
                init_process(mbt, queue);
                break;
            case 2:
                display_MBT(mbt);
                display_Q(queue);
                break;
            case 3:
                terminate_process(queue, mbt);
                break;
            case 4:
                if(exit_program(queue, mbt)){ //exit confirmed
                    exit = true;
                    puts("Exiting Program...");
                    break;
                }else{ //cancel exit
                    break;
                }
            default:
                break;
        }
    }
    return 0;
}
