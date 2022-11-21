// SPN implementation (SPN_keysched, encrypt, subst, transp)
// was taken from prof. Ing. Pavol Zajac, PhD.

#include <stdio.h>
#include <stdint.h>
#include <cstring>
#include <chrono>
#include <bits/stdc++.h>
#include <thread>
#include <vector>
#include <mutex>
using namespace std;
#define NumOfRounds  4

int SPN_keysched(uint32_t key, uint16_t key_exp[]);
uint16_t encrypt(uint16_t pt, uint16_t key_exp[]);
void ddt();

constexpr bool aes_mode = true;

// table of multiplication of two elements in GF(2^4) modulo x4 + x + 1
uint8_t GF_2_mult_tab[16][16] = {
        { 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0},
        { 0x0 ,0x1 ,0x2 ,0x3 ,0x4 ,0x5 ,0x6 ,0x7 ,0x8 ,0x9 ,0xA ,0xB ,0xC ,0xD ,0xE ,0xF},
        { 0x0 ,0x2 ,0x4 ,0x6 ,0x8 ,0xA ,0xC ,0xE ,0x3 ,0x1 ,0x7 ,0x5 ,0xB ,0x9 ,0xF ,0xD},
        { 0x0 ,0x3 ,0x6 ,0x5 ,0xC ,0xF ,0xA ,0x9 ,0xB ,0x8 ,0xD ,0xE ,0x7 ,0x4 ,0x1 ,0x2},
        { 0x0 ,0x4 ,0x8 ,0xC ,0x3 ,0x7 ,0xB ,0xF ,0x6 ,0x2 ,0xE ,0xA ,0x5 ,0x1 ,0xD ,0x9},
        { 0x0 ,0x5 ,0xA ,0xF ,0x7 ,0x2 ,0xD ,0x8 ,0xE ,0xB ,0x4 ,0x1 ,0x9 ,0xC ,0x3 ,0x6},
        { 0x0 ,0x6 ,0xC ,0xA ,0xB ,0xD ,0x7 ,0x1 ,0x5 ,0x3 ,0x9 ,0xF ,0xE ,0x8 ,0x2 ,0x4},
        { 0x0 ,0x7 ,0xE ,0x9 ,0xF ,0x8 ,0x1 ,0x6 ,0xD ,0xA ,0x3 ,0x4 ,0x2 ,0x5 ,0xC ,0xB},
        { 0x0 ,0x8 ,0x3 ,0xB ,0x6 ,0xE ,0x5 ,0xD ,0xC ,0x4 ,0xF ,0x7 ,0xA ,0x2 ,0x9 ,0x1},
        { 0x0 ,0x9 ,0x1 ,0x8 ,0x2 ,0xB ,0x3 ,0xA ,0x4 ,0xD ,0x5 ,0xC ,0x6 ,0xF ,0x7 ,0xE},
        { 0x0 ,0xA ,0x7 ,0xD ,0xE ,0x4 ,0x9 ,0x3 ,0xF ,0x5 ,0x8 ,0x2 ,0x1 ,0xB ,0x6 ,0xC},
        { 0x0 ,0xB ,0x5 ,0xE ,0xA ,0x1 ,0xF ,0x4 ,0x7 ,0xC ,0x2 ,0x9 ,0xD ,0x6 ,0x8 ,0x3},
        { 0x0 ,0xC ,0xB ,0x7 ,0x5 ,0x9 ,0xE ,0x2 ,0xA ,0x6 ,0x1 ,0xD ,0xF ,0x3 ,0x4 ,0x8},
        { 0x0 ,0xD ,0x9 ,0x4 ,0x1 ,0xC ,0x8 ,0x5 ,0x2 ,0xF ,0xB ,0x6 ,0x3 ,0xE ,0xA ,0x7},
        { 0x0 ,0xE ,0xF ,0x1 ,0xD ,0x3 ,0x2 ,0xC ,0x9 ,0x7 ,0x6 ,0x8 ,0x4 ,0xA ,0xB ,0x5},
        { 0x0 ,0xF ,0xD ,0x2 ,0x9 ,0x6 ,0x4 ,0x8 ,0x1 ,0xE ,0xC ,0x3 ,0x8 ,0x7 ,0x5 ,0xA}
};

uint8_t aes_matrix[4][4] = {
        {2,3,1,1},
        {1,2,3,1},
        {1,1,2,3},
        {3,1,1,2}
};


int main()
{
    ddt();
}


int SPN_keysched(uint32_t key, uint16_t key_exp[])
{
    int i;
    for ( i = 0; i <= NumOfRounds; i++ )
    {
        key_exp[i] = (uint16_t) (key & 0xffff);
        key = (key >> 4) ^ (key << (32-4));
    }

    return 0;
}

uint16_t subst(uint16_t x)
{
    /*static uint16_t SB[] = { 5, 6, 2, 0, 7, 11,
                             3, 15, 12, 1, 4,
                             8, 9, 13, 10, 14};*/

    //golden s-boxes
    /*static uint16_t SB[] = { 0x0, 0x3, 0x5, 0x8,
                             0x6, 0x9, 0xC, 0x7,
                             0xD,0xA, 0xE, 0x4,
                             0x1, 0xF, 0xB, 0x2};*/

    /*static uint16_t SB[] = { 0x0, 0x3, 0x5, 0x8,
                             0x6, 0xC, 0xB, 0x7,
                             0x9,0xE, 0xA, 0xD,
                             0xF, 0x2, 0x1, 0x4};*/

    /*static uint16_t SB[] = { 0x0, 0x3, 0x5, 0x8,
                             0x6, 0xA, 0xF, 0x4,
                             0xE,0xD,0x9, 0x2,
                             0x1,0x7, 0xC,0xB};*/

    static uint16_t SB[] = { 0x0, 0x3, 0x5, 0x8,
                             0x6, 0xC, 0xB, 0x7,
                             0xA,0x4, 0x9,0xE,
                             0xF, 0x1,0x2,0xD};


    uint16_t y = 0;

    y  =    SB[   x       & 0xf ];
    y ^=    SB[  (x >> 4) & 0xf ] << 4;
    y ^=    SB[  (x >> 8) & 0xf ] << 8;
    y ^=    SB[  (x >>12) & 0xf ] <<12;

    return y;
}

uint16_t transp(uint16_t x)
{
    uint16_t y = 0;

    y ^= ((x) & 0x8421);
    y ^= ((x) & 0x0842) << 3;
    y ^= ((x) & 0x0084) << 6;
    y ^= ((x) & 0x0008) << 9;
    y ^= ((x) & 0x1000) >> 9;
    y ^= ((x) & 0x2100) >> 6;
    y ^= ((x) & 0x4210) >> 3;

    return y;
}

uint16_t transp_aes(uint16_t x)
{
    uint16_t y = 0;
    uint8_t a0 = (x & 0xf000) >> 12;
    uint8_t a1 = (x & 0x0f00) >> 8;
    uint8_t a2 = (x & 0x00f0) >> 4;
    uint8_t a3 = (x & 0x000f) >> 0;

    uint8_t b0 = GF_2_mult_tab[a0][aes_matrix[0][0]] + GF_2_mult_tab[a1][aes_matrix[0][1]] + GF_2_mult_tab[a2][aes_matrix[0][2]] + GF_2_mult_tab[a3][aes_matrix[0][3]];
    uint8_t b1 = GF_2_mult_tab[a0][aes_matrix[1][0]] + GF_2_mult_tab[a1][aes_matrix[1][1]] + GF_2_mult_tab[a2][aes_matrix[1][2]] + GF_2_mult_tab[a3][aes_matrix[1][3]];
    uint8_t b2 = GF_2_mult_tab[a0][aes_matrix[2][0]] + GF_2_mult_tab[a1][aes_matrix[2][1]] + GF_2_mult_tab[a2][aes_matrix[2][2]] + GF_2_mult_tab[a3][aes_matrix[2][3]];
    uint8_t b3 = GF_2_mult_tab[a0][aes_matrix[3][0]] + GF_2_mult_tab[a1][aes_matrix[3][1]] + GF_2_mult_tab[a2][aes_matrix[3][2]] + GF_2_mult_tab[a3][aes_matrix[3][3]];

    y |= (b0 & 0xf) << 12;
    y |= (b1 & 0xf) << 8;
    y |= (b2 & 0xf) << 4;
    y |= (b3 & 0xf) << 0;

    return y;
}

uint16_t encrypt(uint16_t pt, uint16_t key_exp[])
{
    uint16_t x;
    int i;

    x = pt ^ key_exp[0];

    for (i = 1; i < NumOfRounds; i++)
    {
        x = subst(x);
        if(aes_mode)
        {
            x = transp_aes(x);
        } else
        {
            x = transp(x);
        }
        x = x ^ key_exp[i];
    }

    x = subst(x);
    x = x ^ key_exp[NumOfRounds];

    return x;
}

//differential distribution table
void ddt() {
    int k = 0xc001ca75;
    uint16_t subkeys[NumOfRounds + 1];
    SPN_keysched(k, subkeys);

    auto start = std::chrono::system_clock::now();
    ////////////////////////////////////
    const uint16_t MAX = 0xffff;
    const uint16_t THREADS = 12;

    uint16_t **table = (uint16_t **) malloc(MAX * sizeof(uint16_t *));

    for (int i = 0; i < MAX; i++) {
        table[i] = (uint16_t *) malloc(MAX * sizeof(uint16_t));
    }

    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++){
            table[i][j] = 0;
        }
    }

    std::cout << "allocated" << std::endl;
    std::vector<std::thread> threads;
    int current_a = -1;
    std::mutex a_mutex;

    for (int i = 0; i < THREADS; i++) {
        threads.push_back(std::thread([&]() {
            int local_a = 0;
            while (true) {
                {
                    auto lock = std::lock_guard<std::mutex>(a_mutex);
                    current_a += 1;
                    local_a = current_a;

                }
                if (current_a >= MAX) {
                    break;
                }
                for (uint16_t x = 0; x < MAX; x++) {
                    uint16_t i = encrypt(x, subkeys);
                    uint16_t j = encrypt(x ^ local_a, subkeys);
                    uint16_t b = i ^ j;

                    table[local_a][b] += 1;
                }
            }
        }));
    }

    for (int i = 0; i < THREADS; i++) {
        threads[i].join();
    }

    std::cout << "DONE" << std::endl;

    int max = 0;
    uint64_t numOfZeros = 0;

    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            if (i != 0 && j != 0 && table[i][j] > max) {
                max = table[i][j];
            }
            if (table[i][j] == 0) {
                numOfZeros += 1;
            }
        }
    }

    auto end = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    std::cout << "MAX: " << max << " number of zeros: " << numOfZeros << " TIME: " << diff.count() << " s" << std::endl;
}