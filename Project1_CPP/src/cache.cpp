/**
 * @file cache.c
 * @brief CS{4/6}290 / ECE{4/6}100 Spring 2019 Project 1 cache simulator
 *
 * Fill in the functions to complete the cache simulator
 *
 * @author <Your name goes here>
 */

#include <cstdarg>
#include <cinttypes>
#include <cstdio>
#include <cstdbool>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "cache.hpp"
#include <vector>

using namespace std;

void cache_init(uint64_t, uint64_t, uint64_t, uint64_t, vector <vector<tag>> &);

void print_cache(vector <vector<tag>> cache);

void print_cache_dim(vector <vector<tag>> cache);

// Use this for printing errors while debugging your code
// Most compilers support the __LINE__ argument with a %d argument type
// Example: print_error_exit("Error: Could not allocate memory %d", __LINE__);
static inline void print_error_exit(const char *msg, ...) {
    va_list argptr;
    va_start(argptr, msg);
    fprintf(stderr, msg, argptr);
    va_end(argptr);
    exit(EXIT_FAILURE);
}

// Define data structures and globals you might need for simulating the cache hierarchy below

// TODO


void cache_init(uint64_t tag_bits, uint64_t index_bits, uint64_t offset_bits, uint64_t num_of_ways,
                vector <vector<tag>> &cache) {
    uint64_t num_sets = 1 << index_bits;
    vector <vector<tag>> tmp_cache(num_sets, vector<tag>(num_of_ways));
    cache = tmp_cache;
}

/**
 * Function to initialize any data structures you might need for simulating the cache hierarchy. Use
 * the sim_conf structure for initializing dynamically allocated memory.
 *
 * @param sim_conf Pointer to simulation configuration structure
 *
 */
void sim_init(struct sim_config_t *sim_conf) {
    /**
     * I 0x7f67aae6d090
     * |--------48 bits - (C-S)--------|-------(C - B - S)------|-------B-------|
     *                tag bits                        index bits           offset bits
     *
     * Tag bits: size of a tag
     * index bits: number of rows
     * Offset bits: Ignored
     * sim_conf.s : Number of ways or Number of columns
     */
    i_index_bits = sim_conf->l1inst.c - sim_conf->l1inst.b - sim_conf->l1inst.s;
    d_index_bits = sim_conf->l1data.c - sim_conf->l1data.b - sim_conf->l1data.s;
    l2_index_bits = sim_conf->l2unified.c - sim_conf->l2unified.b - sim_conf->l2unified.s;

    i_offset_bits = sim_conf->l1inst.b;
    d_offset_bits = sim_conf->l1data.b;
    l2_offset_bits = sim_conf->l2unified.b;

    i_tag_bits = NUM_ADDR_BITS - (i_index_bits + i_offset_bits);
    d_tag_bits = NUM_ADDR_BITS - (d_index_bits + d_offset_bits);
    l2_tag_bits = NUM_ADDR_BITS - (l2_index_bits + l2_offset_bits);

    cache_init(i_tag_bits, i_index_bits, i_offset_bits, sim_conf->l1inst.s, l1_i_cache);
    cache_init(d_tag_bits, d_index_bits, d_offset_bits, sim_conf->l1data.s, l1_d_cache);
    cache_init(l2_tag_bits, l2_index_bits, l2_offset_bits, sim_conf->l2unified.s, l2_cache);

    cout << "L1 Inst Cache: " << endl;
    print_cache_dim(l1_i_cache);
    cout << "L1 Data Cache: " << endl;
    print_cache_dim(l1_d_cache);
    cout << "L2 Cache: " << endl;
    print_cache_dim(l2_cache);
    cout << "L1 Inst Tag: " << i_tag_bits << " bits, Index: " << i_index_bits << " bits, Offset: " << i_offset_bits
         << " bits" << endl;
    cout << "L1 Data Tag: " << d_tag_bits << " bits, Index: " << d_index_bits << " bits, Offset: " << d_offset_bits
         << " bits" << endl;
    cout << "L2 Tag: " << l2_tag_bits << " bits, Index: " << l2_index_bits << " bits, Offset: " << l2_offset_bits
         << " bits" << endl;
    cout << "-------------------------" << endl;
}

void print_cache(vector <vector<tag>> cache) {
    for (int i = 0; i < cache.size(); i++) {
        for (int j = 0; j < cache[i].size(); j++) {
            cout << " " << cache[i][j].valid;
        }
        cout << endl;
    }
}

void print_cache_dim(vector <vector<tag>> cache) {
    cout << "Number of sets: " << cache.size() << endl;
    cout << "Number of ways: " << cache[0].size() << endl;
}

/**
 * Function to perform cache accesses, one access at a time. The print_debug function should be called
 * if the debug flag is true
 *
 * @param addr The address being accessed by the processor
 * @param type The type of access - Load (L), Store (S) or Instruction (I)
 * @param sim_stats Pointer to simulation statistics structure - Should be populated here
 * @param sim_conf Pointer to the simulation configuration structure - Don't modify it in this function
 */
void cache_access(uint64_t addr, char type, struct sim_stats_t *sim_stats, struct sim_config_t *sim_conf) {
    uint64_t tag_l2 = TAG_MASK(addr, sim_conf->l2unified.c, sim_conf->l2unified.s);
    uint64_t indx_l2 = INDX_MASK(addr, sim_conf->l2unified.b);
    switch (type) {
        case LOAD: // access data cache
            uint64_t tag_d = TAG_MASK(addr, sim_conf->l1data.c, sim_conf->l1data.s);
            uint64_t indx_d = INDX_MASK(addr, sim_conf->l1data.b);
            cache_data(l1_d_cache[indx_d], tag_d, l2_cache[indx_l2], tag_l2, sim_stats, sim_conf);

            sim_stats->l1data_num_accesses++;
            sim_stats->l1data_num_accesses_loads++;

            if (cache_miss(l1_d_cache[indx_d], tag_d)) {
                sim_stats->l1data_num_misses++;
                sim_stats->l1data_num_misses_loads++;
                if (cache_miss(l2_cache[indx_l2], tag_l2)) {
                    sim_stats->l2unified_num_misses++;
                    sim_stats->l2unified_num_misses_loads++;
                    cache_replace(l2_cache[indx_l2], tag_l2, sim_conf->l2unified, sim_stats);
                }
                cache_replace(l1_d_cache[indx_d], tag_d, sim_conf->l1data, sim_statsll);
            }
            break;
        case STORE: // access data cache
            /**
             * Store: Write
             * Write Back Write Allocate:
             *
             * Write Through Write No Allocate:
             */
            uint64_t tag_d = TAG_MASK(addr, sim_conf->l1data.c, sim_conf->l1data.s);
            uint64_t indx_d = INDX_MASK(addr, sim_conf->l1data.b);
            cache_write(l1_d_cache[indx_d], tag_d, l2_cache[indx_l2], tag_l2, sim_stats, sim_conf);
            break;
        case INST: // access instruction cache
            uint64_t tag_i = TAG_MASK(addr, sim_conf->l1inst.c, sim_conf->l1inst.s);
            uint64_t indx_i = INDX_MASK(addr, sim_conf->l1inst.b);
            cache_instr(l1_i_cache[indx_i], tag_i, l2_cache[indx_l2], tag_l2, sim_stats, sim_conf);
            break;
        default:
            break;
    }
}


/**
 * Function to cleanup dynamically allocated simulation memory, and perform any calculations
 * that might be required
 *
 * @param stats Pointer to the simulation structure - Final calculations should be performed here
 */
void sim_cleanup(struct sim_stats_t *sim_stats, struct sim_config_t *sim_conf) {
    // TODO
}
