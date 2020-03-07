#include <cstdio>
#include <cinttypes>
#include <cstdlib>
#include <cstring>

#include <getopt.h>
#include <unistd.h>

#include "procsim.hpp"

FILE* inFile = stdin;

// Print help message and exit
static void print_help_and_exit(void)
{
    printf("procsim [OPTIONS]\n");
    printf("  -f F\t\tNumber of instructions to fetch\n");
    printf("  -p P\t\tNumber of PRegs\n");
    printf("  -j J\t\tNumber of ADD/BR FUs\n");
    printf("  -k K\t\tNumber of MUL FUs\n");
    printf("  -l L\t\tNumber of LOAD/STORE FUs\n");
    printf("  -r R\t\tNumber of reservation entries per unit FU type\n");
    printf("  -y Y\t\tlog2 number of entries in BTB\n");
    printf("  -i <Path to trace> Input trace file\n");
    printf("  -h\t\tThis helpful output\n");
    exit(EXIT_SUCCESS);
}

// Print configuration
static void print_config(proc_conf_t *config)
{
    printf("Processor Settings\n");
    printf("F: %" PRIu64 "\n", config->f);
    printf("P: %" PRIu64 "\n", config->p);
    printf("J: %" PRIu64 "\n", config->j);
    printf("K: %" PRIu64 "\n", config->k);
    printf("L: %" PRIu64 "\n", config->l);
    printf("R: %" PRIu64 "\n", config->r);
    printf("Y: %" PRIu64 "\n", config->y);
    printf("\n");
}

// Prints statistics at the end of the simulation
static void print_statistics(proc_stats_t* stats) {
    printf("\n\n");
    printf("Processor stats:\n");
    printf("Total instructions retired:     %lu\n", stats->instructions_retired);
    printf("Total Branch instructions:      %lu\n", stats->branch_instructions);
    printf("Correctly predicted branches:   %lu\n", stats->correctly_predicted);
    printf("Branch prediction accuracy:     %f\n", stats->branch_prediction_accuracy);
    printf("Total load instructions:        %lu\n", stats->load_instructions);
    printf("Total store instructions:       %lu\n", stats->store_instructions);
    printf("Load/Store queue hits:          %lu\n", stats->load_store_buffer_hits);
    printf("Load/Store queue hit rate:      %f\n", stats->load_store_buffer_hr);
    printf("Average Dispatch Queue size:    %f\n", stats->average_disp_queue_size);
    printf("Maximum Dispatch Queue size:    %lu\n", stats->max_disp_queue_size);
    printf("Average instructions retired:   %f\n", stats->average_ipc);
    printf("Final Cycle count:              %lu\n", stats->cycle_count);
}

/* Function to read instruction from the input trace. Populates the inst struct
 *
 * returns true if an instruction was read successfully. Returns false at end of trace
 *
 */
bool read_instruction(inst_t* inst)
{
    int ret;

    if (inst == NULL) {
        fprintf(stderr, "Fetch requires a valid pointer to populate\n");
        return false;
    }

    // Don't modify this line. Instruction fetch might break otherwise!
    ret = fscanf(inFile, "%" PRIx64 " %d %d %d %d %" PRIx64 " %" PRIx64 " %d\n", &inst->inst_addr, &inst->opcode, &inst->dest_reg,
                    &inst->src_reg[0], &inst->src_reg[1], &inst->ld_st_addr, &inst->br_target, (int *) &inst->br_taken);

    if (ret != 8) { // Check if something went really wrong
        if (!feof(inFile)) { // Check if end of file has been reached
            fprintf(stderr, "Something went wrong and we could not parse the instruction\n");
        }
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    int opt;

    // Default configuration setup
    proc_conf_t conf = {.f = DEFAULT_F,
                                .p = DEFAULT_P,
                                .j = DEFAULT_J,
                                .k = DEFAULT_K,
                                .l = DEFAULT_L,
                                .r = DEFAULT_R,
                                .y = DEFAULT_Y};

    while(-1 != (opt = getopt(argc, argv, "f:p:j:k:k:r:y:i:h"))) {
        switch(opt) {
        case 'f':
            conf.f = atoi(optarg);
            break;
        case 'p':
            conf.p = atoi(optarg);
            break;
        case 'j':
            conf.j = atoi(optarg);
            break;
        case 'k':
            conf.k = atoi(optarg);
            break;
        case 'l':
            conf.l = atoi(optarg);
            break;
        case 'r':
            conf.r = atoi(optarg);
            break;
        case 'y':
            conf.y = atoi(optarg);
            break;
        case 'i':
            inFile = fopen(optarg, "r");
            if (inFile == NULL) {
                fprintf(stderr, "Failed to open %s for reading\n", optarg);
                print_help_and_exit();
            }
            break;
        case 'h':
        default:
            print_help_and_exit();
            break;
        }
    }

    print_config(&conf); // Print run configuration

    setup_proc(&conf); // Setup the processor

    proc_stats_t stats;
    memset(&stats, 0, sizeof(proc_stats_t));

    run_proc(&stats, &conf); // Run the processor

    complete_proc(&stats); // Finalize statistics and perform cleanup

    fclose(inFile); // release file descriptor memory

    print_statistics(&stats);

    return 0;
}
