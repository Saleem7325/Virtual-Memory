#include "my_vm.h"

/*
 * Number of entries we can fit in a Page Table/Directory
*/
#define NUM_ENTRIES (PGSIZE / sizeof(pde_t))

/*
 * Number of bits in address that specify index into page
*/
#define OFFSET_BITS ((unsigned long)log2f(PGSIZE))

/*
 * Number of bits in address that specify index into page table
*/
#define PT_BITS  (((sizeof(pde_t) * 8) - OFFSET_BITS) / 2)

/*
 * Number of bits in address that specify index into page directory
*/
#define PD_BITS (((sizeof(pde_t) * 8) - OFFSET_BITS) % 2) + PT_BITS

/*
 * Number of virtual address bits
*/
#define VIRT_BITS (PT_BITS + PD_BITS)

/*
 * Number of bits in virtual bitmap
*/
#define VBMAP_SIZE ((unsigned long)pow(2, VIRT_BITS))

/*
 * Number of chars to store in virtual bitmap
*/
#define VBMAP_BYTES ((VBMAP_SIZE % 8) == 0 ? (VBMAP_SIZE / 8) : ((VBMAP_SIZE / 8) + 1))

/*
 * Number of chars to store in physical bitmap
*/
#define PBMAP_SIZE ((MEMSIZE / PGSIZE) / 8)

/*
 * Physical memory
*/
char *phys_mem = NULL;

/*
 * Bitmap representing all pages in phys_mem
 * 1 = allocated, 0 = free
*/
char page_bitmap[PBMAP_SIZE];

/*
 * Bitmap representing all possible virtual addresses
*/
char *virt_bitmap;

/*
 * Number of Pages tables/directories in memory
*/
int table_count;

/*
 * Number of user allocated pages
*/
unsigned long int page_count;

/*
 * Pointer to Page Directory in phys_mem (points to start of phys_mem).
 * First page in memory is allocated to page directory.
*/
pde_t *page_directory;

void *get_next_avail(int num_pages);

/*__________________ HELPER FUNCTIONS __________________*/

void free_all(){
    free(phys_mem);
    free(virt_bitmap);

    phys_mem = NULL;
    virt_bitmap = NULL;
}

/*
 * For validating/checking page_bitmap
*/
int get_bit_at_index(char *bitmap, int index){
    int byte_index = index / 8;
    int bit_index = index % 8;

    char byte = bitmap[byte_index];
    if(byte & (1 << bit_index)){
        return 1;
    }else{
        return 0;
    }
}

int set_bit_at_index(char *bitmap, int index){
    int byte_index = index / 8;
    int bit_index = index % 8;
    char byte = bitmap[byte_index];
    
    // Set the bit at 'bit_index' to 1
    byte |= (1 << bit_index);
    
    // Store the modified byte back in the bitmap
    bitmap[byte_index] = byte;
}

int clear_bit_at_index(char *bitmap, int index){
    int byte_index = index / 8;
    int bit_index = index % 8;
    char byte = bitmap[byte_index];
    
    // Clear the bit at 'bit_index' to 0
    byte &= ~(1 << bit_index);
    
    // Store the modified byte back in the bitmap
    bitmap[byte_index] = byte;
}

int page_directory_index(void *va){
    long unsigned int vaddr = (long unsigned int)va;
    return (vaddr >> (PT_BITS + OFFSET_BITS));
}

int page_table_index(void *va){
    int mask = ((1 << PT_BITS) - 1);
    return (((long unsigned int)va >> (OFFSET_BITS)) & mask);
}

int offset(void *va){
    int mask = ((1 << OFFSET_BITS) - 1);
    return ((int)va & mask);
}

void *virtual_address(int pages){
    unsigned long free_pages = 0;
    unsigned long start_page = -1;
    
    for(int i = 0; i < VBMAP_SIZE; ++i){
        if (get_bit_at_index(virt_bitmap, i) == 0){
            // Page is free
            if(start_page == -1){
                start_page = i;  // Mark start of free block
            }
            
            if(++free_pages == pages){
                break;  // Found a block of required size
            }
        }else{
            // Page is allocated, reset the count
            free_pages = 0;
            start_page = -1;
        }
    }

    // Not enough continuous pages available
    if(free_pages < pages){
        return NULL;
    }
    
    // Mark the found pages as allocated in the physical bitmap
    for(int i = start_page; i < start_page + pages; ++i){
        set_bit_at_index(virt_bitmap, i);
    }

    // int ofset = offset(pa);
    return (void *)(start_page <<  OFFSET_BITS);
}

/*__________________ VMEM FUNCTIONS __________________*/

/*
 * Function responsible for allocating and setting your physical memory 
*/
void set_physical_mem() {
    // Create physical memory 
    // Initialize page bitmap 
    phys_mem = (char *)malloc(MEMSIZE);
    memset(page_bitmap, '\0', PBMAP_SIZE);

    // Assign Page directory first page in memory and initialize
    page_directory = (pde_t *)phys_mem;
    memset(page_directory, '\0', PGSIZE);

    // Create and initialize virtual bitmap
    virt_bitmap = (char *)malloc(VBMAP_BYTES);
    memset(virt_bitmap, '\0', VBMAP_BYTES);
    set_bit_at_index(virt_bitmap, 0);

    // Set first bit in bitmap since that is where page directory is being stored
    set_bit_at_index(page_bitmap, 0);
    table_count++; 
    // char *byte = &page_bitmap[0];
    // *byte = *byte | 1;

    // Print config macros for debugging
    printf("Number of entries: %d\n", NUM_ENTRIES);
    printf("Offset bits: %ld\n", OFFSET_BITS);
    printf("Page table bits: %ld\n", PT_BITS);
    printf("Page directory bits: %ld\n", PD_BITS);
    printf("Virtual bits: %ld\n", VIRT_BITS);
    printf("Virtual bitmap bits: %ld\n", VBMAP_SIZE);
    printf("Virtual bitmap bytes: %ld\n", VBMAP_SIZE);
    printf("Page bitmap size: %d\n", PBMAP_SIZE);
    printf("______________________________________\n");
}


/*
 * Part 2: Add a virtual to physical page translation to the TLB.
 * Feel free to extend the function arguments or return type.
 */
int add_TLB(void *va, void *pa) {

    /*Part 2 HINT: Add a virtual to physical page translation to the TLB */

    return -1;
}


/*
 * Part 2: Check TLB for a valid translation.
 * Returns the physical page address.
 * Feel free to extend this function and change the return type.
 */
pte_t *check_TLB(void *va) {

    /* Part 2: TLB lookup code here */



   /*This function should return a pte_t pointer*/
}


/*
 * Part 2: Print TLB miss rate.
 * Feel free to extend the function arguments or return type.
 */
void print_TLB_missrate() {
    double miss_rate = 0;	

    /*Part 2 Code here to calculate and print the TLB miss rate*/




    fprintf(stderr, "TLB miss rate %lf \n", miss_rate);
}

/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
pte_t *translate(pde_t *pgdir, void *va) {
    /* Part 1 HINT: Get the Page directory index (1st level) Then get the
    * 2nd-level-page table index using the virtual address.  Using the page
    * directory index and page table index get the physical address.
    *
    * Part 2 HINT: Check the TLB before performing the translation. If
    * translation exists, then you can return physical address from the TLB.
    */
    int pd_index = page_directory_index(va);
    int pt_index = page_table_index(va);
    pde_t *page_table = (pde_t *)pgdir[pd_index];

    if(page_table != NULL){
        // pte_t *entry = (pte_t *)page_table[pt_index];
        pte_t *entry = (pte_t *)&page_table[pt_index];

        if(entry != NULL){
            return entry;
        }
    }

    //If translation not successful, then return NULL
    return NULL; 
}


/*
The function takes a page directory address, virtual address, physical address
as an argument, and sets a page table entry. This function will walk the page
directory to see if there is an existing mapping for a virtual address. If the
virtual address is not present, then a new entry will be added
*/
int page_map(pde_t *pgdir, void *va, void *pa) {
    int pd_index = page_directory_index(va);
    int pt_index = page_table_index(va);
    pte_t *page_table = (pte_t *)pgdir[pd_index];

    if(!page_table){
        void *pt = get_next_avail(1);
        if(!pt){
            return -1;
        }
        memset(pt, '\0', PGSIZE);
        table_count++;

        pgdir[pd_index] = (pde_t)pt;
        page_table = (pte_t *)pgdir[pd_index];
    }

    pte_t *entry = &page_table[pt_index];
    if(entry && *entry == 0){
        *entry = (pte_t)pa;
    }else{
        // Entry is present
        return -1;
    }

    return 0;
}


/*Function that gets the next available page
*/
void *get_next_avail(int num_pages) {
    //Use virtual address bitmap to find the next free page
    int free_pages = 0;
    int start_page = -1;
    
    for(int i = 0; i < PBMAP_SIZE * 8; ++i){
        if (get_bit_at_index(page_bitmap, i) == 0){
            // Page is free
            if(start_page == -1){
                start_page = i;  // Mark start of free block
            }
            
            if(++free_pages == num_pages){
                break;  // Found a block of required size
            }
        }else{
            // Page is allocated, reset the count
            free_pages = 0;
            start_page = -1;
        }
    }

    if(free_pages < num_pages){
        // Not enough continuous pages available
        return NULL;
    }
    
    // Mark the found pages as allocated in the physical bitmap
    for(int i = start_page; i < start_page + num_pages; ++i){
        set_bit_at_index(page_bitmap, i);
    }

    // Calculate and return the starting address of the block
    return (void *)(phys_mem + (start_page * PGSIZE));
}


/* Function responsible for allocating pages
and used by the benchmark
*/
void *t_malloc(unsigned int num_bytes) {
    if(!phys_mem){
        set_physical_mem();
    }

    int pages = 1;
    if(num_bytes > PGSIZE){
        pages = (num_bytes % PGSIZE) == 0 ? (num_bytes / PGSIZE) : ((num_bytes / PGSIZE) + 1);
    }
    printf("\nNum pages: %d\n", pages);

    unsigned long int total_pages = table_count + page_count;
    if(total_pages + pages > MEMSIZE){
        fprintf(stderr, "Error: Not enough free memory\n");
        return NULL;
    }

    void *vptr = virtual_address(pages);
    void *vpage = vptr;
    for(int i = 0; i < pages; i++, vpage += PGSIZE){
        void *page = get_next_avail(1);
        if(!page){
            fprintf(stderr, "Error: Could not allocate page\n");
            return NULL;
        }

        page_map(page_directory, vpage, page);
        page_count++;
        printf("Mapping: (va : pa) %p : %p\n", vpage, page);
    }

    return vptr;
}

/* Responsible for releasing one or more memory pages using virtual address (va)
*/
void t_free(void *va, int size) {
    if(!phys_mem){
        fprintf(stderr, "\nError: Attempting to free an unallocated page at %p\n", va);
        return;
    }

    int num_pages = 1;
    if(size > PGSIZE){
       num_pages = ((size % PGSIZE) == 0 ? (size / PGSIZE) : ((size / PGSIZE) + 1));
    }
    printf("\nNum pages: %d\n", num_pages);

    void *current_va = va;
    int vpage_index = ((unsigned long)current_va >> OFFSET_BITS);
    for(int i = 0; i < num_pages; ++i){
        // Translate to get physical address
        pte_t *pte = translate(page_directory, current_va);
        if(pte == NULL){
            fprintf(stderr, "Address: %p not found\n", current_va);
            return;
        }
        
        if(*pte){
            // Get index of physical page
            int page_index = ((char *)*pte - phys_mem) / PGSIZE;
            printf("Freeing (va : pa) %p : %p\n", current_va, (void *)*pte);
            *pte = 0;

            clear_bit_at_index(page_bitmap, page_index);
            clear_bit_at_index(virt_bitmap, vpage_index++);
            page_count--;

            current_va = (void *)((char *)current_va + PGSIZE);
        } else {
            // Handle error: Trying to free an unallocated page
            fprintf(stderr, "Error: Attempting to free an unallocated page at %p\n", current_va);
            break;
        }
    }

    if(page_count == 0){
        free_all();
    }
    
}


/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
 * The function returns 0 if the put is successfull and -1 otherwise.
*/
int put_value(void *va, void *val, int size) {

    /* HINT: Using the virtual address and translate(), find the physical page. Copy
     * the contents of "val" to a physical page. NOTE: The "size" value can be larger 
     * than one page. Therefore, you may have to find multiple pages using translate()
     * function.
     */


    /*return -1 if put_value failed and 0 if put is successfull*/

}


/*Given a virtual address, this function copies the contents of the page to val*/
void get_value(void *va, void *val, int size) {

    /* HINT: put the values pointed to by "va" inside the physical memory at given
    * "val" address. Assume you can access "val" directly by derefencing them.
    */


}



/*
This function receives two matrices mat1 and mat2 as an argument with size
argument representing the number of rows and columns. After performing matrix
multiplication, copy the result to answer.
*/
void mat_mult(void *mat1, void *mat2, int size, void *answer) {

    /* Hint: You will index as [i * size + j] where  "i, j" are the indices of the
     * matrix accessed. Similar to the code in test.c, you will use get_value() to
     * load each element and perform multiplication. Take a look at test.c! In addition to 
     * getting the values from two matrices, you will perform multiplication and 
     * store the result to the "answer array"
     */
    int x, y, val_size = sizeof(int);
    int i, j, k;
    for (i = 0; i < size; i++) {
        for(j = 0; j < size; j++) {
            unsigned int a, b, c = 0;
            for (k = 0; k < size; k++) {
                int address_a = (unsigned int)mat1 + ((i * size * sizeof(int))) + (k * sizeof(int));
                int address_b = (unsigned int)mat2 + ((k * size * sizeof(int))) + (j * sizeof(int));
                get_value( (void *)address_a, &a, sizeof(int));
                get_value( (void *)address_b, &b, sizeof(int));
                // printf("Values at the index: %d, %d, %d, %d, %d\n", 
                //     a, b, size, (i * size + k), (k * size + j));
                c += (a * b);
            }
            int address_c = (unsigned int)answer + ((i * size * sizeof(int))) + (j * sizeof(int));
            // printf("This is the c: %d, address: %x!\n", c, address_c);
            put_value((void *)address_c, (void *)&c, sizeof(int));
        }
    }
}