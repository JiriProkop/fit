/*
    Author: Jiří Prokop
    Date: 29.4.2024

    Disclaimer:
        Works only on machines where BYTE = 8 bits and where little-endian is used
*/

#include <bit>
#include <bitset>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#define BLOCK_BIT_SIZE (512)
#define MIN_BITS_FOR_PADDING (448)

/**
 * Constants for SHA-256
 */
const u_int32_t constants[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be,
    0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa,
    0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85,
    0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
    0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

/**
 * Structure for working variables defined in SHA-256 specification
 */
typedef struct {
    u_int32_t a;
    u_int32_t b;
    u_int32_t c;
    u_int32_t d;
    u_int32_t e;
    u_int32_t f;
    u_int32_t g;
    u_int32_t h;
} work_vars_t;

/**
 * Structure for arguments
 */
typedef struct {
    bool c;
    bool s;
    bool v;
    bool e;
    std::string k;
    std::string m;
    unsigned long n;
    std::string a;
} args_t;

/**
 * @brief Convert a 32-bit unsigned integer to big-endian
 *
 * @param value 32-bit unsigned integer
 * @return uint32_t 32-bit unsigned integer in big-endian
 */
uint32_t to_big_endian(uint32_t value) {
    return ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) | ((value & 0x000000FF) << 24);
}

/**
 * @brief Add length after padding
 *
 * @param ptr Pointer to the block
 * @param length Length of the message
 */
void add_length_after_padding(char *ptr, u_int64_t length) {
    ptr[63] = length;
    ptr[62] = length >> 8;
    ptr[61] = length >> 16;
    ptr[60] = length >> 24;
    ptr[59] = length >> 32;
    ptr[58] = length >> 40;
    ptr[57] = length >> 48;
    ptr[56] = length >> 56;
}

/**
 * @brief Add padding to the block
 *
 * @param block_pos Position in the block
 * @param ptr Pointer to the block
 * @param length Length of the message
 * @param first_time True if it is the first time, false otherwise
 * @return true Padding is done, false otherwise
 */
bool padd(unsigned *block_pos, char *ptr, u_int64_t length, bool first_time) {
    unsigned bits_in_block = (*block_pos) * CHAR_BIT;

    if (bits_in_block == MIN_BITS_FOR_PADDING) {
        add_length_after_padding(ptr, length);
        return true;
    }

    if (first_time == true) {
        ptr[(*block_pos)++] = 0x80;
        bits_in_block += CHAR_BIT;
        first_time = false;
    }

    if (bits_in_block > MIN_BITS_FOR_PADDING) {
        while (bits_in_block != BLOCK_BIT_SIZE) {
            ptr[(*block_pos)++] = 0x00;
            bits_in_block += CHAR_BIT;
        }
        *block_pos = 0;
        return false;
    } else {
        while (bits_in_block != MIN_BITS_FOR_PADDING) {
            ptr[(*block_pos)++] = 0b00000000;
            bits_in_block += CHAR_BIT;
        }
        add_length_after_padding(ptr, length);
        return true;
    }
}

/**
 * @brief Initialize hash values
 *
 * @param hash Array for hash values
 */
void init_hash(u_int32_t hash[]) {
    const u_int32_t h_0[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
    };

    for (int i = 0; i < 8; i++) {
        hash[i] = h_0[i];
    }
}

/**
 * Functions for SHA-256, specified in the standard

 *///////////////////////////////////////////////////////////////////////////////////////////////
u_int32_t ch(u_int32_t x, u_int32_t y, u_int32_t z) { return (x & y) ^ (~x & z); }

u_int32_t maj(u_int32_t x, u_int32_t y, u_int32_t z) { return (x & y) ^ (x & z) ^ (y & z); }

u_int32_t sum0(u_int32_t w) { return std::__rotr(w, 2) ^ std::__rotr(w, 13) ^ std::__rotr(w, 22); }

u_int32_t sum1(u_int32_t w) { return std::__rotr(w, 6) ^ std::__rotr(w, 11) ^ std::__rotr(w, 25); }

u_int32_t sigma0(u_int32_t w) { return std::__rotr(w, 7) ^ std::__rotr(w, 18) ^ (w >> 3); }

u_int32_t sigma1(u_int32_t w) { return std::__rotr(w, 17) ^ std::__rotr(w, 19) ^ (w >> 10); }
////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Prepare message schedule
 *
 * @param msg_schedule Array for message schedule
 * @param block Array for block
 */
void prepare_msg_schedule(u_int32_t *msg_schedule, u_int32_t *block) {
    int i;
    for (i = 0; i < 16; i++) {
        msg_schedule[i] = block[i];
    }

    for (; i < 64; i++) {
        msg_schedule[i] = sigma1(msg_schedule[i - 2]) + msg_schedule[i - 7] + sigma0(msg_schedule[i - 15]) + msg_schedule[i - 16];
    }
}

/**
 * @brief Initialize working variables
 *
 * @param hash Array for hash values
 * @param vars Structure for working variables
 */
void init_working_vars(u_int32_t hash[], work_vars_t *vars) {
    vars->a = hash[0];
    vars->b = hash[1];
    vars->c = hash[2];
    vars->d = hash[3];
    vars->e = hash[4];
    vars->f = hash[5];
    vars->g = hash[6];
    vars->h = hash[7];
}

/**
 * @brief Convert block to big-endian
 *
 * @param block Array for block
 */
void block_to_big_endian(u_int32_t block[]) {
    for (int i = 0; i < 16; i++) {
        block[i] = to_big_endian(block[i]);
    }
}

/**
 * @brief Perform SHA-256 hash
 *
 * @param block Array for block
 * @param hash Array for hash values
 */
void do_hash(u_int32_t block[], u_int32_t hash[]) {
    block_to_big_endian(block);

    u_int32_t msg_schedule[64];
    prepare_msg_schedule(msg_schedule, block);

    work_vars_t vars;
    init_working_vars(hash, &vars);

    u_int32_t t1, t2;
    for (int i = 0; i < 64; i++) {
        t1 = vars.h + sum1(vars.e) + ch(vars.e, vars.f, vars.g) + constants[i] + msg_schedule[i];
        t2 = sum0(vars.a) + maj(vars.a, vars.b, vars.c);
        vars.h = vars.g;
        vars.g = vars.f;
        vars.f = vars.e;
        vars.e = vars.d + t1;
        vars.d = vars.c;
        vars.c = vars.b;
        vars.b = vars.a;
        vars.a = t1 + t2;
    }

    hash[0] = vars.a + hash[0];
    hash[1] = vars.b + hash[1];
    hash[2] = vars.c + hash[2];
    hash[3] = vars.d + hash[3];
    hash[4] = vars.e + hash[4];
    hash[5] = vars.f + hash[5];
    hash[6] = vars.g + hash[6];
    hash[7] = vars.h + hash[7];
}

/**
 * @brief Convert hash values to hex
 *
 * @param hash Array with hash values
 */
std::string hash_to_hex(u_int32_t hash[]) {
    std::stringstream ss;
    for (int i = 0; i < 8; i++) {
        ss << std::hex << std::setw(8) << std::setfill('0') << hash[i];
    }
    return ss.str();
}

/**
 * @brief Compute SHA-256 hash
 *
 * @return std::string SHA-256 hash
 */
std::string sha256() {
    u_int32_t hash[8];
    init_hash(hash);

    u_int64_t length = 0;
    unsigned block_pos = 0;
    u_int32_t block[16];
    char *ptr = (char *)block;
    int c;
    while ((c = std::getchar()) != EOF) {
        ptr[block_pos++] = c;
        if (block_pos == BLOCK_BIT_SIZE / CHAR_BIT) {
            do_hash(block, hash);
            block_pos = 0;
        }
        length += 8;
    }

    if (!padd(&block_pos, ptr, length, true)) {
        do_hash(block, hash);
        padd(&block_pos, ptr, length, false);
    }

    do_hash(block, hash);
    return hash_to_hex(hash);
}

/**
 * @brief Compute MAC
 *
 * @param args Arguments
 * @return std::string MAC
 */
std::string mac(args_t *args) {
    u_int32_t hash[8];
    init_hash(hash);

    u_int64_t length = 0;
    unsigned block_pos = 0;
    u_int32_t block[16];
    char *ptr = (char *)block;

    for (unsigned long i = 0; i < args->k.length(); i++) {
        ptr[block_pos++] = args->k[i];
        if (block_pos == BLOCK_BIT_SIZE / CHAR_BIT) {
            do_hash(block, hash);
            block_pos = 0;
        }
        length += 8;
    }

    int c;
    while ((c = std::getchar()) != EOF) {
        ptr[block_pos++] = c;
        if (block_pos == BLOCK_BIT_SIZE / CHAR_BIT) {
            do_hash(block, hash);
            block_pos = 0;
        }
        length += 8;
    }

    if (!padd(&block_pos, ptr, length, true)) {
        do_hash(block, hash);
        padd(&block_pos, ptr, length, false);
    }

    do_hash(block, hash);
    return hash_to_hex(hash);
}

/**
 * @brief Initialize hash values for length extension attack
 *
 * @param hash Array for hash values
 * @param mac MAC
 */
void init_hash_attack(u_int32_t hash[], std::string mac) {
    for (int i = 0; i < 8; ++i) {
        std::string byte = mac.substr(i * 8, 8);
        hash[i] = std::stoul(byte, nullptr, 16);
    }
}

/**
 * @brief Print block padding in hex
 *
 * @param ptr Pointer to the block
 * @param start Start position
 * @return std::string Block padding in hex
 */
std::string print_block_padding_hex(char *ptr, unsigned start) {
    std::stringstream ss;
    ss << "\\x" << std::hex << 0x80;
    size_t i;
    for (i = start + 1; i < BLOCK_BIT_SIZE / CHAR_BIT; ++i) {
        ss << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ptr[i]);
    }

    return ss.str();
}

/**
 * @brief Do a length extension attack
 *
 * @param args Arguments
 */
void length_extension_attack(args_t *args) {
    // compute the new hash with extension:
    u_int32_t hash[8];
    init_hash_attack(hash, args->m);

    u_int64_t length = BLOCK_BIT_SIZE + args->a.length() * 8;
    unsigned block_pos = 0;
    u_int32_t block[16];
    char *ptr = (char *)block;

    for (unsigned long i = 0; i < args->a.length(); i++) {
        ptr[block_pos++] = args->a[i];
        if (block_pos == BLOCK_BIT_SIZE / CHAR_BIT) {
            do_hash(block, hash);
            block_pos = 0;
        }
    }

    if (!padd(&block_pos, ptr, length, true)) {
        do_hash(block, hash);
        padd(&block_pos, ptr, length, false);
    }

    do_hash(block, hash);
    std::cout << hash_to_hex(hash) << std::endl;

    // now come up with the result string with extension:
    block_pos = args->n;
    length = args->n * 8;
    std::string changed_msg = "";
    int c;
    while ((c = std::getchar()) != EOF) {
        if (++block_pos == BLOCK_BIT_SIZE / CHAR_BIT) {
            block_pos = 0;
        }
        length += 8;
        changed_msg += c;
    }
    unsigned block_pos_before_padd = block_pos;
    if (!padd(&block_pos, ptr, length, true)) {
        changed_msg += print_block_padding_hex(ptr, block_pos_before_padd);
        block_pos_before_padd = 0;
        padd(&block_pos, ptr, length, false);
    }
    changed_msg += print_block_padding_hex(ptr, block_pos_before_padd);
    changed_msg += args->a;
    std::cout << changed_msg << std::endl;
}

/**
 * @brief Print help
 */
void print_help() {
    std::cout << "Usage: ./kry [OPTION]..." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -c              Compute hash and print it" << std::endl;
    std::cout << "  -s              Compute MAC and print it" << std::endl;
    std::cout << "  -v              Verify MAC" << std::endl;
    std::cout << "  -e              Do a length extension attack" << std::endl;
    std::cout << "  -k              Key for MAC computation" << std::endl;
    std::cout << "  -m              Given MAC for verification" << std::endl;
    std::cout << "  -n              Length of the key" << std::endl;
    std::cout << "  -a              String to append for length extension attack" << std::endl;
}

/**
 * @brief Parse arguments
 *
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @param args Arguments structure where the parsed arguments will be stored
 */
void parse_args(int argc, char *argv[], args_t *args) {
    if (argc == 1) {
        print_help();
        exit(1);
    } else if (argc == 2 && (!strcmp(argv[1], "--help"))) {
        print_help();
        exit(EXIT_SUCCESS);
    }

    int c;
    while ((c = getopt(argc, argv, "hcsvek:m:n:a:")) != -1) {
        switch (c) {
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
                break;
            case 'c':
                args->c = true;
                break;
            case 's':
                args->s = true;
                break;
            case 'v':
                args->v = true;
                break;
            case 'e':
                args->e = true;
                break;
            case 'k':
                args->k = optarg;
                break;
            case 'm':
                args->m = optarg;
                break;
            case 'n':
                args->n = std::stoul(optarg);
                break;
            case 'a':
                args->a = optarg;
                break;
            case '?':
                exit(1);
            default:
                break;
        }
    }
    int check = 0;
    if (args->c) {
        check++;
        if (!args->k.empty() || !args->m.empty() || args->n != 0 || !args->a.empty()) {
            std::cerr << "No other arguments are allowed with -c!" << std::endl;
            exit(1);
        }
    } else if (args->s) {
        if (args->k.empty()) {
            std::cerr << "Key is required for MAC computation." << std::endl;
            exit(1);
        } else if (!args->m.empty() || args->n != 0 || !args->a.empty()) {
            std::cerr << "No other arguments are allowed with -s!" << std::endl;
            exit(1);
        }
        check++;
    } else if (args->v) {
        if (args->k.empty() || args->m.empty()) {
            std::cerr << "Key and MAC are required for MAC verification." << std::endl;
            exit(1);
        } else if (args->n != 0 || !args->a.empty()) {
            std::cerr << "No other arguments are allowed with -v!" << std::endl;
            exit(1);
        }
        check++;
    } else if (args->e) {
        if (args->m.empty() || args->n == 0 || args->a.empty()) {
            std::cerr << "Key, length of the key and string to append are required for length extension attack." << std::endl;
            exit(1);
        } else if (!args->k.empty()) {
            std::cerr << "No other arguments are allowed with -e!" << std::endl;
            exit(1);
        }
        check++;
    }
    if (check != 1) {
        std::cerr << "Exactly one of -c, -s, -v, -e is required!" << std::endl;
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    args_t args = {.c = false, .s = false, .v = false, .e = false, .k = "", .m = "", .n = 0, .a = ""};
    parse_args(argc, argv, &args);

    if (args.c) {
        std::cout << sha256() << std::endl;
    } else if (args.s) {
        std::cout << mac(&args) << std::endl;
    } else if (args.v) {
        std::string computed_mac = mac(&args);
        if (computed_mac == args.m) {
            return 0;
        } else {
            return 1;
        }
    } else if (args.e) {
        length_extension_attack(&args);
    }

    return 0;
}
