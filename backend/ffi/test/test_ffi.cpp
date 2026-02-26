/**
 * @file test_ffi.cpp
 * @brief Simple test for FFI functions
 */

#include "../include/shanks_ffi.hpp"
#include <iostream>
#include <cstring>

int main() {
    std::cout << "=== Testing Shanks FFI ===" << std::endl;
    
    // Test 1: List series
    std::cout << "\n1. Testing shanks_list_series()..." << std::endl;
    char* series_list = shanks_list_series();
    if (series_list) {
        std::cout << "Series list (first 200 chars): " << std::endl;
        std::string s(series_list);
        std::cout << s.substr(0, 200) << "..." << std::endl;
        shanks_free_string(series_list);
    } else {
        std::cout << "FAILED: series_list is null" << std::endl;
        return 1;
    }
    
    // Test 2: List algorithms
    std::cout << "\n2. Testing shanks_list_accels()..." << std::endl;
    char* accel_list = shanks_list_accels();
    if (accel_list) {
        std::cout << "Algorithm list (first 200 chars): " << std::endl;
        std::string s(accel_list);
        std::cout << s.substr(0, 200) << "..." << std::endl;
        shanks_free_string(accel_list);
    } else {
        std::cout << "FAILED: accel_list is null" << std::endl;
        return 1;
    }
    
    // Test 3: Get series info
    std::cout << "\n3. Testing shanks_get_series_info()..." << std::endl;
    char* series_info = shanks_get_series_info("Ln2Series");
    if (series_info) {
        std::cout << "Ln2Series info: " << series_info << std::endl;
        shanks_free_string(series_info);
    } else {
        std::cout << "FAILED: series_info is null" << std::endl;
        return 1;
    }
    
    // Test 4: Create a series
    std::cout << "\n4. Testing shanks_series_create()..." << std::endl;
    ShanksSeriesHandle series = shanks_series_create("Ln2Series", "F64", "1.0", "{}");
    if (series) {
        std::cout << "Series created successfully" << std::endl;
        
        // Test 5: Generate series
        std::cout << "\n5. Testing shanks_series_generate()..." << std::endl;
        FFISeriesResult* result = shanks_series_generate(series, 10);
        if (result) {
            std::cout << "Series generic result nodes: " << result->sn.lines[0].len << std::endl;
            shanks_series_result_free(result);
        } else {
            std::cout << "FAILED: generate result is null" << std::endl;
            const char* err = shanks_last_error();
            if (err) {
                std::cout << "Error: " << err << std::endl;
            }
        }
        
        // Test 6: Get sum
        std::cout << "\n6. Testing shanks_series_get_sum()..." << std::endl;
        char* sum = shanks_series_get_sum(series);
        if (sum) {
            std::cout << "Sum: " << sum << std::endl;
            shanks_free_string(sum);
        }
        
        // Destroy series
        shanks_series_destroy(series);
    } else {
        std::cout << "FAILED: series is null" << std::endl;
        const char* err = shanks_last_error();
        if (err) {
            std::cout << "Error: " << err << std::endl;
        }
        return 1;
    }
    
    // Test 7: Create an algorithm
    std::cout << "\n7. Testing shanks_accel_create()..." << std::endl;
    ShanksAccelHandle accel = shanks_accel_create("Shanks Transformation", "F64", "{}");
    if (accel) {
        std::cout << "Algorithm created successfully" << std::endl;
        shanks_accel_destroy(accel);
    } else {
        std::cout << "FAILED: accel is null" << std::endl;
        const char* err = shanks_last_error();
        if (err) {
            std::cout << "Error: " << err << std::endl;
        }
    }
    
    std::cout << "\n=== All tests passed ===" << std::endl;
    return 0;
}
