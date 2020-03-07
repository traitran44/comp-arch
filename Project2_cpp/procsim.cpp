
#include <cstdio>
#include <cinttypes>
#include <iostream>

#include "procsim.hpp"

// You may define any global variables here



/**
 * Subroutine for initializing the processor. You many add and initialize any global or heap
 * variables as needed.
 *
 * @param config Pointer to the configuration. Read only in this function
 */
void setup_proc(const proc_conf_t *config)
{

}

/**
 * Subroutine that simulates the processor. The processor should fetch instructions as
 * appropriate, until all instructions have executed
 *
 * @param p_stats Pointer to the statistics structure
 * @param config Pointer to the configuration. Read only in this function
 */
void run_proc(proc_stats_t* p_stats, const proc_conf_t  *config)
{
    // Note: use the "read_instruction" function to fetch the next instruction from the memory trace
}

/**
 * Subroutine for cleaning up any outstanding instructions and calculating overall statistics
 * such as average IPC, average fire rate etc.
 *
 * @param p_stats Pointer to the statistics structure
 */
void complete_proc(proc_stats_t *p_stats)
{

}
