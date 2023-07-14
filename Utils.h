#ifndef UTILS_H
#define UTILS_H

#include<iostream>
#include<vector>
#include <stdio.h>
#include <cstring>
#include <map>
#include <bits/stdc++.h>

class Utils {

public:
    std::vector<uint8_t> intToByteArr(int)
    {
        std::vector<uint8_t> byte_arr;
        unsigned long n = 1500;
        uint8_t byte_x;

        byte_x = (n >> 0) & 0xFF;
        byte_arr.push_back(byte_x);
        byte_x = (n >> 8) & 0xFF;
        byte_arr.push_back(byte_x);
        byte_x = (n >> 16) & 0xFF;
        byte_arr.push_back(byte_x);
        byte_x = (n >> 24) & 0xFF;
        byte_arr.push_back(byte_x);

        return byte_arr;
    }

    int byteArrayToInt(std::vector<uint8_t> byte_arr)
    {
        int value;
        std::memcpy(&value, byte_arr.data(), sizeof(int));

        return value;
    }


    /*Adresleri küçükten büyüğe doğru sıralayan fonksiyon*/
    static bool custom_compare(const std::pair<int,  std::pair<int,int>> & p1, const std::pair<int, std::pair<int,int>> & p2) {
        return p1.second.first < p2.second.first;
    }

    auto sortfn(std::map<int, std::pair<int,int>>& m) {

        /*değerleri tutmak için vektör tanımlandı. */
        std::vector< std::pair<int, std::pair<int,int>> > vec;

        /*vektör dolduruldu. */
        for (auto it = m.begin(); it != m.end(); it++) {
            vec.push_back(make_pair(it->first, it->second));
        }

        sort(vec.begin(), vec.end(), custom_compare);

        return vec;
    }
};
#endif // UTILS_H

