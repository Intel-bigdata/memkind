#include "trial_generator.h"
#include "check.h"

void TrialGenerator :: generate_trials_incremental(alloc_api_t api){

    size_t size[] = {2, 2*KB, 2*MB, 2*GB};
    int    psize[] = {HBW_PAGESIZE_4KB, HBW_PAGESIZE_2MB, HBW_PAGESIZE_2MB,
		      HBW_PAGESIZE_2MB};

    for (int i = 0; i< 4; i++){
	trial_t ltrial;
	ltrial.api = api;
	ltrial.size = size[i];
	ltrial.alignment = 32;
	ltrial.page_size = psize[i];
	ltrial.free_index = -1;
	trial_vec.push_back(ltrial);
	ltrial.api = FREE;
	ltrial.size = 0;
	ltrial.alignment = 0;
	ltrial.page_size = 0;
	ltrial.free_index = i;    
	trial_vec.push_back(ltrial);
    }
    
}


void TrialGenerator :: print_trial_list(){

    std::vector<trial_t>:: iterator it;

    std::cout << "SIZE PSIZE ALIGN FREE"<<std::endl;

    for (it = trial_vec.begin();
         it != trial_vec.end();
         it++){
	std::cout << it->size <<" "
                  << it->page_size <<" "
                  << it->alignment <<" "
                  << it->free_index<<std::endl;
    }

}


#if 0

void TrialGenerator :: execute_trials(int num_bandwidth, int *bandwidth){

    int num_trial = trial_vec.size();
    int i, ret = 0;
    void **ptr_vec = NULL;
    void **tptr = NULL;
    ptr_vec = (void **) malloc (num_trial *
				sizeof (void *));
    if (NULL == ptr_vec){
	fprintf (stderr, "Error in allocating ptr array\n");
	exit(-1);
    }
    

    for (i = 0; i < num_trial; ++i) {
        switch(trial_vec[i].api) {
            case FREE:
                if (i == num_trial - 1 || trial_vec[i + 1].api != REALLOC) {
                    hbw_free(ptr_vec[trial_vec[i].free_index]);
                    ptr_vec[trial_vec[i].free_index] = NULL;
                    ptr_vec[i] = NULL;
                }
                else {
                    ptr_vec[i] = hbw_realloc(ptr_vec[trial_vec[i].free_index], trial_vec[i + 1].size);
                    ptr_vec[trial_vec[i].free_index] = NULL;
                    ++i;
                }
                break;
            case MALLOC:
                ptr_vec[i] = hbw_malloc(trial_vec[i].size);
                break;
            case CALLOC:
                ptr_vec[i] = hbw_calloc(trial_vec[i].size, 1);
                break;
            case REALLOC:
                ptr_vec[i] = hbw_realloc(NULL, trial_vec[i].size);
                break;
	case MEMALIGN:
	        ret =  hbw_allocate_memalign(&ptr_vec[i], 
					     trial_vec[i].alignment,
					     trial_vec[i].size);
                break;
            case MEMALIGN_PSIZE:
	        ret = hbw_allocate_memalign_psize(&ptr_vec[i],
						  trial_vec[i].alignment,
						  trial_vec[i].size, 
						  trial_vec[i].page_size);
                break;
        }
        if (trial_vec[i].api != FREE) {
            ASSERT_TRUE(ptr_vec[i] != NULL);
            memset(ptr_vec[i], 0, trial_vec[i].size);
            Check check(ptr_vec[i], trial_vec[i].size);
            EXPECT_EQ(0, check.check_node_hbw(num_bandwidth, bandwidth));
            if (trial_vec[i].api == CALLOC) {
                check.check_zero();
            }
            if (trial_vec[i].api == MEMALIGN || trial_vec[i].api == MEMALIGN_PSIZE) {
                check.check_align(trial_vec[i].alignment);
            }
            if (trial_vec[i].api == MEMALIGN_PSIZE) {
                check.check_page_size(trial_vec[i].page_size);
            }
        }
    }
    for (i = 0; i < num_trial; ++i) {
        if (ptr_vec[i]) {
            hbw_free(ptr_vec[i]);
        }
    }
}
#endif