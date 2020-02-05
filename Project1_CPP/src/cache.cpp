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

#include "cache.hpp"
#include <vector>

void cache_init(uint64_t, uint64_t, uint64_t, uint64_t);

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
   * |--------48 bits - [(C-B-S) + B]--------|-------(C - B - S)------|-------B-------|
   *                tag bits                        index bits           offset bits
   *
   * Tag bits: size of a tag
   * index bits: number of rows
   * Offset bits: Ignored
   * sim_conf.s : Number of ways or Number of columns
   */
  uint64_t i_index_bits = sim_conf->l1inst.c - sim_conf->l1inst.b - sim_conf->l1inst.s;
  uint64_t d_index_bits = sim_conf->l1data.c - sim_conf->l1data.b - sim_conf->l1data.s;
  uint64_t l2_index_bits = sim_conf->l2unified.c - sim_conf->l2unified.b - sim_conf->l2unified.s;

  uint64_t i_offset_bits = sim_conf->l1inst.b;
  uint64_t d_offset_bits = sim_conf->l1data.b;
  uint64_t l2_offset_bits = sim_conf->l2unified.b;

  uint64_t i_tag_bits = NUM_ADDR_BITS - (i_index_bits + i_offset_bits);
  uint64_t d_tag_bits = NUM_ADDR_BITS - (d_index_bits + d_offset_bits);
  uint64_t l2_tag_bits = NUM_ADDR_BITS - (l2_index_bits + l2_offset_bits);

  cache_init(i_tag_bits, i_index_bits, i_offset_bits, sim_conf->l1data.s);
  cache_init(d_tag_bits, d_index_bits, d_offset_bits, sim_conf->l1inst.s);
  cache_init(l2_tag_bits, l2_index_bits, l2_offset_bits, sim_conf->l2unified.s);
}

void cache_init(uint64_t tag_bits, uint64_t index_bits, uint64_t offset_bits, uint64_t num_of_ways) {

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
  // TODO
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
