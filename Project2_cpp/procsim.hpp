#ifndef PROCSIM_HPP
#define PROCSIM_HPP

#include <cstdint>
#include <cstdio>

// Default structure
#define DEFAULT_F 4
#define DEFAULT_P 128
#define DEFAULT_J 3
#define DEFAULT_K 1
#define DEFAULT_L 2
#define DEFAULT_R 1
#define DEFAULT_Y 8

// List of opcodes
typedef enum {
    OP_NOP = 1,     // NOP
    OP_ADD = 2,     // ADD
    OP_MUL = 3,     // MUL
    OP_LOAD = 4,    // LOAD
    OP_STORE = 5,   // STORE
    OP_BR = 6       // BRANCH
} opcode_t;

// Simlatiion configuration struct
typedef struct configuration {
    uint64_t f;     // Fetch rate
    uint64_t p;     // Number of PRegs
    uint64_t j;     // Number of ADD/BR units
    uint64_t k;     // Number of MUL units
    uint64_t l;     // Number of LOAD/STORE units
    uint64_t r;     // Number of reservation stations per FU type
    uint64_t y;     // log2(Number of entries in BTB - size of the branch predictor)
} proc_conf_t;

// Instruction struct partially populated by the driver file
typedef struct instruction {

    uint64_t inst_addr;             // instruction address
    int32_t opcode;                 // instruction opcode
    int32_t src_reg[2];             // Source registers
    int32_t dest_reg;               // Destination register
    uint64_t br_target;             // branch target if the instruction is a branch
    bool br_taken;                  // actual behavior of the branch instruction
    uint64_t ld_st_addr;            // Effective address of Load or Store instruction

    // clock fields - You will be filling in these for each instruction
    uint64_t fetch_cycle;           // cycle in which fetched
    uint64_t dispatch_cycle;        // cycle in which dispatched
    uint64_t schedule_cycle;        // cycle in which scheduled
    uint64_t execute_cycle;         // cycle in which executed
    uint64_t state_update_cycle;    // cycle in which retired

    // You may introduce other fields as needed

} inst_t;

// Simulation statistics tracking struct
typedef struct statistics {
    uint64_t instructions_retired;          // Total number of instructions completed/retired

    uint64_t branch_instructions;           // Total branch instructions
    uint64_t correctly_predicted;           // Branches predicted correctly
    double branch_prediction_accuracy;      // Branch prediction accuracy

    uint64_t load_instructions;             // Total load instructions
    uint64_t store_instructions;            // Total store instructions
    uint64_t load_store_buffer_hits;        // Loads that hit in the load/store buffer
    double load_store_buffer_hr;            // Load/Store buffer hit rate

    double average_disp_queue_size;         // Average dispatch queue size
    uint64_t max_disp_queue_size;           // Maximum dispatch queue size
    double average_ipc;                     // Average instructions retired per cycle (IPC)
    uint64_t cycle_count;                   // Total cycle count (runtime in cycles)

} proc_stats_t;

bool read_instruction(inst_t* p_inst);

void setup_proc(const proc_conf_t *config);
void run_proc(proc_stats_t *stats, const proc_conf_t  *conf);
void complete_proc(proc_stats_t *stats);

#endif // PROCSIM_HPP
