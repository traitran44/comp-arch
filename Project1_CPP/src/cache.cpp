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

void cache_init(uint64_t num_of_ways, vector<cache_set> &cache);

void print_cache(vector<cache_set> cache);

void print_cache_dim(vector<cache_set> cache);


int cache_hit(cache_access_info *access_info);

int install_block(cache_access_info *access_info, sim_config_t *sim_conf, sim_stats_t *sim_stats);

int get_victim(cache_set *tag_set, sim_config_t *sim_conf);

int get_first_valid_victim(cache_set *t_set);

void l1_inst_cache_load(cache_access_info *access_info, sim_stats_t *sim_stats, sim_config_t *sim_conf);

void l1_data_cache_load(cache_access_info *access_info, sim_stats_t *sim_stats, sim_config_t *sim_conf);

void l1_data_cache_store(cache_access_info *access_info, sim_stats_t *sim_stats, sim_config_t *sim_conf);

int l2_data_cache_load(cache_access_info *access_info, sim_stats_t *sim_stats, sim_config_t *sim_conf);

void l2_data_cache_store(cache_access_info *access_info, sim_stats_t *sim_stats, sim_config_t *sim_conf);

int l2_inst_cache_load(cache_access_info *access_info, sim_stats_t *sim_stats, sim_config_t *sim_conf);

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


void cache_init(uint64_t num_of_ways, vector<cache_set> &cache) {
    for (uint64_t i = 0; i < cache.size(); i++) { // init set
        cache[i].tags = vector<tag>(num_of_ways);
        cache[i].limit_set_size = num_of_ways;
        cache[i].tag_count = 0;
        for (uint64_t j = 0; j < num_of_ways; j++) { // init tag
            cache[i].tags[j].valid = false;
            cache[i].tags[j].dirty = false;
            cache[i].tags[j].tag_id = 0;
            cache[i].tags[j].time = 0;
            cache[i].tags[j].access_count = 0;
        }
    }
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
     *              tag bits                   index bits           offset bits
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

    uint64_t i_num_sets = (((uint64_t) 1) << i_index_bits);
    uint64_t d_num_sets = (((uint64_t) 1) << d_index_bits);
    uint64_t l2_num_sets = (((uint64_t) 1) << l2_index_bits);

    l1_i_cache.resize(i_num_sets, cache_set());
    l1_d_cache.resize(d_num_sets, cache_set());
    l2_cache.resize(l2_num_sets, cache_set());

    cache_init(1 << sim_conf->l1inst.s, l1_i_cache);
    cache_init(1 << sim_conf->l1data.s, l1_d_cache);
    cache_init(1 << sim_conf->l2unified.s, l2_cache);

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

void print_cache_dim(vector<cache_set> cache) {
    cout << "Number of sets: " << cache.size() << endl;
    cout << "Number of ways: " << cache[0].tags.size() << endl;
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
void cache_access(uint64_t addr, char type, uint64_t line_count,
                  struct sim_stats_t *sim_stats, struct sim_config_t *sim_conf) {
    uint64_t indx_mask_l2 = sim_conf->l2unified.c - sim_conf->l2unified.b - sim_conf->l2unified.s;
    uint64_t offset_mask_l2 = sim_conf->l2unified.b;
    uint64_t tag_l2 = TAG_MASK(addr, sim_conf->l2unified.c, sim_conf->l2unified.s);
    uint64_t indx_l2 = INDX_MASK(addr, offset_mask_l2, indx_mask_l2);
    uint64_t tag_d;
    uint64_t indx_d;
    uint64_t tag_i;
    uint64_t indx_i;
    uint64_t indx_mask_l1;
    uint64_t offset_mask_l1;
    cache_access_info access_info = cache_access_info();
    access_info.new_l2_tag = tag_l2;
    access_info.new_l2_indx = indx_l2;
    access_info.line_count = line_count;
    access_info.addr = addr;
    access_info.update_accesses = true;
    switch (type) {
        case LOAD: // access data cache
            indx_mask_l1 = sim_conf->l1data.c - sim_conf->l1data.b - sim_conf->l1data.s;
            offset_mask_l1 = sim_conf->l1data.b;
            tag_d = TAG_MASK(addr, sim_conf->l1data.c, sim_conf->l1data.s);
            indx_d = INDX_MASK(addr, offset_mask_l1, indx_mask_l1);
            access_info.new_l1_tag = tag_d;
            access_info.new_l1_indx = indx_d;
            access_info.install_lvl = L1D;
            access_info.dirty = false;
            l1_data_cache_load(&access_info, sim_stats, sim_conf);
            break;
        case STORE: // access data cache
            indx_mask_l1 = sim_conf->l1data.c - sim_conf->l1data.b - sim_conf->l1data.s;
            offset_mask_l1 = sim_conf->l1data.b;
            tag_d = TAG_MASK(addr, sim_conf->l1data.c, sim_conf->l1data.s);
            indx_d = INDX_MASK(addr, offset_mask_l1, indx_mask_l1);
            access_info.new_l1_tag = tag_d;
            access_info.new_l1_indx = indx_d;
            access_info.install_lvl = L1D;
            access_info.dirty = true;
            l1_data_cache_store(&access_info, sim_stats, sim_conf);
            break;
        case INST: // access instruction cache
            indx_mask_l1 = sim_conf->l1inst.c - sim_conf->l1inst.b - sim_conf->l1inst.s;
            offset_mask_l1 = sim_conf->l1inst.b;
            tag_i = TAG_MASK(addr, sim_conf->l1inst.c, sim_conf->l1inst.s);
            indx_i = INDX_MASK(addr, offset_mask_l1, indx_mask_l1);
            access_info.new_l1_tag = tag_i;
            access_info.new_l1_indx = indx_i;
            access_info.install_lvl = L1I;
            access_info.dirty = false;
            l1_inst_cache_load(&access_info, sim_stats, sim_conf);
            break;
        default:
            cout << "Something not right in cache access!" << endl;
            break;
    }
}

void l1_inst_cache_load(cache_access_info *access_info, sim_stats_t *sim_stats, sim_config_t *sim_conf) {
    sim_stats->l1inst_num_accesses++;
    access_info->install_lvl = L1I;
    int l1_hit_indx;
    l1_hit_indx = cache_hit(access_info);
    if (l1_hit_indx < 0) { // L1 Cache Miss
        sim_stats->l1inst_num_misses++;
        l2_inst_cache_load(access_info, sim_stats, sim_conf);
    } else { // L1 Cache Hit
        l1_i_cache[access_info->new_l1_indx].tags[l1_hit_indx].access_count++;
        l1_i_cache[access_info->new_l1_indx].tags[l1_hit_indx].time = access_info->line_count;
    }
}

int l2_inst_cache_load(cache_access_info *access_info, sim_stats_t *sim_stats, sim_config_t *sim_conf) {
    int l2_hit_indx;
    access_info->install_lvl = L2;
    cache_set *t_set = &l2_cache[access_info->new_l2_indx];
    sim_stats->l2unified_num_accesses++;
    sim_stats->l2unified_num_accesses_insts++;
    l2_hit_indx = cache_hit(access_info);
    if (l2_hit_indx < 0) { // L2 Cache Miss
        sim_stats->l2unified_num_misses++;
        sim_stats->l2unified_num_misses_insts++;
        install_block(access_info, sim_conf, sim_stats); // Install instructions to L2
        sim_stats->l2unified_num_bytes_transferred += BLK_SZ(sim_conf->l2unified.b); // read instruction to L2
    } else { // L2 Cache Hit
        t_set->tags[l2_hit_indx].access_count++;
        t_set->tags[l2_hit_indx].time = access_info->line_count;
    }
    access_info->install_lvl = L1I;
    install_block(access_info, sim_conf, sim_stats); // Install instructions to L1
    return l2_hit_indx;
}


void l1_data_cache_load(cache_access_info *access_info, sim_stats_t *sim_stats, sim_config_t *sim_conf) {
    sim_stats->l1data_num_accesses++;
    sim_stats->l1data_num_accesses_loads++;
    access_info->install_lvl = L1D;
    int l1_hit_indx;
    l1_hit_indx = cache_hit(access_info);
    if (l1_hit_indx < 0) { // L1 Cache Miss
        sim_stats->l1data_num_misses++;
        sim_stats->l1data_num_misses_loads++;
        l2_data_cache_load(access_info, sim_stats, sim_conf);
    } else { // L1 Cache Hit
        l1_d_cache[access_info->new_l1_indx].tags[l1_hit_indx].access_count++;
        l1_d_cache[access_info->new_l1_indx].tags[l1_hit_indx].time = access_info->line_count;
    }
}

int l2_data_cache_load(cache_access_info *access_info, sim_stats_t *sim_stats, sim_config_t *sim_conf) {
    int l2_hit_indx;
    sim_stats->l2unified_num_accesses++;
    sim_stats->l2unified_num_accesses_loads++;
    access_info->install_lvl = L2;
    cache_set *t_set = &l2_cache[access_info->new_l2_indx];

    l2_hit_indx = cache_hit(access_info);
    if (l2_hit_indx < 0) { // L2 Cache Miss
        sim_stats->l2unified_num_misses++;
        sim_stats->l2unified_num_misses_loads++;
        // read new blk from memory
        sim_stats->l2unified_num_bytes_transferred += BLK_SZ(sim_conf->l2unified.b);
        install_block(access_info, sim_conf, sim_stats); // Install L2
    } else { // L2 Cache Hit
        t_set->tags[l2_hit_indx].access_count++;
        t_set->tags[l2_hit_indx].time = access_info->line_count;
    }

    access_info->install_lvl = L1D;
    install_block(access_info, sim_conf, sim_stats); // Install L1

    return l2_hit_indx;
}

void l1_data_cache_store(cache_access_info *access_info, sim_stats_t *sim_stats, sim_config_t *sim_conf) {
    sim_stats->l1data_num_accesses++;
    sim_stats->l1data_num_accesses_stores++;
    cache_set *l1_set = &l1_d_cache[access_info->new_l1_indx];
    int l1_hit_indx;
    switch (sim_conf->wp) {
        case WBWA: // Write back Write Alloc
            l1_hit_indx = cache_hit(access_info);
            if (l1_hit_indx < 0) { // L1 Write miss
                sim_stats->l1data_num_misses++;
                sim_stats->l1data_num_misses_stores++;
                l2_data_cache_store(access_info, sim_stats, sim_conf);
            } else { // L1 Write Hit
                l1_set->tags[l1_hit_indx].valid = true;
                l1_set->tags[l1_hit_indx].dirty = true;
                l1_set->tags[l1_hit_indx].time = access_info->line_count;
                l1_set->tags[l1_hit_indx].access_count++;
            }
            break;
        case WTWNA: // Write Through Write No Alloc
            l1_hit_indx = cache_hit(access_info);
            if (l1_hit_indx >= 0) { // Write Hit
                l1_set->tags[l1_hit_indx].valid = true;
                l1_set->tags[l1_hit_indx].dirty = true;
                l1_set->tags[l1_hit_indx].access_count++;
                l1_set->tags[l1_hit_indx].time = access_info->line_count;
            } else { // Write Miss
                access_info->install_lvl = L2;
                l2_data_cache_store(access_info, sim_stats, sim_conf);
                access_info->install_lvl = L1D;
            }
            // Transfer to memory anyway
            sim_stats->l2unified_num_bytes_transferred += BLK_SZ(sim_conf->l2unified.b);
            break;
        default:
            break;
    }
}

void l2_data_cache_store(cache_access_info *access_info, sim_stats_t *sim_stats, sim_config_t *sim_conf) {
    access_info->install_lvl = L2;
    sim_stats->l2unified_num_accesses++;
    sim_stats->l2unified_num_accesses_stores++;
    cache_set *t_set = &l2_cache[access_info->new_l2_indx];
    int l2_hit_indx;
    switch (sim_conf->wp) {
        case WBWA: // Write back Write Alloc
            l2_hit_indx = cache_hit(access_info);
            if (l2_hit_indx < 0) { // L2 Write miss
                sim_stats->l2unified_num_misses++;
                sim_stats->l2unified_num_misses_stores++;
//                install_block(access_info, sim_conf, sim_stats); // fetch from memory to L2

                // Transfer block from Mem to L2 Cache on miss
                sim_stats->l2unified_num_bytes_transferred += BLK_SZ(sim_conf->l2unified.b);
                access_info->dirty = false;
                install_block(access_info, sim_conf, sim_stats);
            } else { // L2 Write Hit
                t_set->tags[l2_hit_indx].valid = true;
                t_set->tags[l2_hit_indx].dirty = false;
                t_set->tags[l2_hit_indx].access_count++;
                t_set->tags[l2_hit_indx].time = access_info->line_count;
            }
            access_info->install_lvl = L1D;
            access_info->dirty = true;
            install_block(access_info, sim_conf, sim_stats);
            break;
        case WTWNA: // Write Through Write No Alloc
            break;
        default:
            break;
    }
}

int install_block(cache_access_info *access_info, sim_config_t *sim_conf, sim_stats_t *sim_stats) {
    int victim_indx = -1;
    uint64_t new_tag;
    cache_set *t_set;

    switch (access_info->install_lvl) {
        case L1D:
            t_set = &l1_d_cache[access_info->new_l1_indx];
            new_tag = access_info->new_l1_tag;
            break;
        case L1I:
            t_set = &l1_i_cache[access_info->new_l1_indx];
            new_tag = access_info->new_l1_tag;
            break;
        case L2:
            t_set = &l2_cache[access_info->new_l2_indx];
            new_tag = access_info->new_l2_tag;
            break;
        default:
            t_set = NULL;
            new_tag = 0;
            break;
    }

    if (t_set->tag_count < t_set->tags.size()) { // Compulsory miss
        victim_indx = t_set->tag_count;
        t_set->replace_q.push(victim_indx);
        t_set->tag_count++;
    } else { // Capacity Miss - Find eviction victim
        victim_indx = get_victim(t_set, sim_conf);
        tag victim_tag = t_set->tags[victim_indx];
        if (victim_indx >= 0) {
            switch (access_info->install_lvl) {
                case L1D:
                    if (victim_tag.valid) sim_stats->l1data_num_evictions++;
                    // if evicted blk is dirty and write policy is WBWA then install to L2
                    if (sim_conf->wp == WBWA && victim_tag.dirty && victim_tag.valid) {
                        /**
                         Writeback the L1D dirty victim to L2
                         */
                        cache_access_info victim_access_info = cache_access_info();
                        victim_access_info.dirty = victim_tag.dirty;
                        victim_access_info.new_l1_tag = TAG_MASK(victim_tag.addr,
                                                                 sim_conf->l1data.c,
                                                                 sim_conf->l1data.s);
                        victim_access_info.new_l1_indx = INDX_MASK(victim_tag.addr,
                                                                   sim_conf->l1data.b,
                                                                   sim_conf->l1data.c - sim_conf->l1data.b - sim_conf->l1data.s);
                        victim_access_info.new_l2_tag = TAG_MASK(victim_tag.addr,
                                                           sim_conf->l2unified.c,
                                                           sim_conf->l2unified.s);
                        victim_access_info.new_l2_indx = INDX_MASK(victim_tag.addr,
                                                             sim_conf->l2unified.b,
                                                             sim_conf->l2unified.c - sim_conf->l2unified.b -
                                                             sim_conf->l2unified.s);
                        victim_access_info.dirty = true;
                        victim_access_info.line_count = victim_tag.time;
                        victim_access_info.install_lvl = L2;

                        install_block(&victim_access_info, sim_conf, sim_stats);
                    }
                    break;
                case L1I:
                    if (victim_tag.valid) sim_stats->l1inst_num_evictions++;
                    break;
                case L2:
                    if (victim_tag.valid) sim_stats->l2unified_num_evictions++;
                    if (sim_conf->wp == WBWA && victim_tag.dirty && victim_tag.valid) {
                        sim_stats->l2unified_num_write_backs++;
                        // write dirty evicted blk to memory for WBWA
                        sim_stats->l2unified_num_bytes_transferred += BLK_SZ(sim_conf->l2unified.b);
                    }
                    break;
            }

        }
    }
    if (victim_indx >= 0) {
        t_set->tags[victim_indx].valid = true;
        t_set->tags[victim_indx].dirty = access_info->dirty;
        t_set->tags[victim_indx].access_count = 1;
        t_set->tags[victim_indx].time = access_info->line_count;
        t_set->tags[victim_indx].tag_id = new_tag;
        t_set->tags[victim_indx].addr = access_info->addr;
    } else {
        cout << "Cache Read Replace Indx < 0!" << endl;
    }
    return victim_indx;
}

int get_victim(cache_set *tag_set, sim_config_t *sim_conf) {
    int victim = -1;
    uint64_t min_count;
    uint64_t min_tag;
    switch (sim_conf->rp) {
        case LRU:
            victim = get_first_valid_victim(tag_set);
            min_count = tag_set->tags[victim].time;
            for (uint64_t i = victim; i < tag_set->tags.size(); ++i) {
                if (tag_set->tags[i].time < min_count && tag_set->tags[i].valid) {
                    min_count = tag_set->tags[i].time;
                    victim = i;
                }
            }
            break;
        case LFU:
            victim = get_first_valid_victim(tag_set);
            min_count = tag_set->tags[victim].access_count;
            min_tag = tag_set->tags[victim].tag_id;

            for (uint64_t i = 0; i < tag_set->tags.size(); ++i) {
                if (tag_set->tags[i].access_count < min_count ||
                    (tag_set->tags[i].access_count == min_count &&
                     tag_set->tags[i].tag_id < min_tag)) {
                    min_count = tag_set->tags[i].access_count;
                    victim = i;
                    min_tag = tag_set->tags[i].tag_id;
                }
            }
            break;
        case FIFO:
            victim = tag_set->replace_q.front();
            tag_set->replace_q.pop();
            tag_set->replace_q.push(victim);
            break;
        default:
            break;
    }
    if (victim < 0) cout << "get_victim shouldn't return -1" << endl;
    return victim;
}

int get_first_valid_victim(cache_set *t_set) {
    for (uint64_t i = 0; i < t_set->tags.size(); ++i) {
        if (t_set->tags[i].valid) {
            return i;
        }
    }
    return -1;
}

int cache_hit(cache_access_info *access_info) {
    int match_indx = -1;
    uint64_t match_tag = -1;
    cache_set *t_set;
    switch (access_info->install_lvl) {
        case L1D:
            t_set = &l1_d_cache[access_info->new_l1_indx];
            match_tag = access_info->new_l1_tag;
            break;
        case L1I:
            t_set = &l1_i_cache[access_info->new_l1_indx];
            match_tag = access_info->new_l1_tag;
            break;
        case L2:
            t_set = &l2_cache[access_info->new_l2_indx];
            match_tag = access_info->new_l2_tag;
            break;
        default:
            t_set = NULL;
            break;
    }
    for (uint64_t i = 0; i < t_set->tags.size(); ++i) {
        if (t_set->tags[i].tag_id == match_tag &&
            t_set->tags[i].valid) {
            match_indx = i;
            break;
        }
    }
    return match_indx;
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
