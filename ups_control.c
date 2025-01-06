#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>   // For sleep function  
#include <hidapi/hidapi.h>  
#include <string.h>   // For string processing  

#define VENDOR_ID  0x0665  
#define PRODUCT_ID 0x5161  
#define TIMEOUT 5000  // Timeout in milliseconds  
#define BUFFER_SIZE 64 // Buffer size for data read  

int main() {  
    int res;  
    hid_device *handle;  
    unsigned char command[] = {0x51, 0x31, 0x0D}; // "Q1" command with CR  
    unsigned char response[BUFFER_SIZE];  
    unsigned char full_response[1024];  
    int full_response_len = 0;  

    // Initialize HIDAPI  
    if (hid_init()) {  
        printf("Failed to initialize HIDAPI.\n");  
        return 1;  
    }  

    // Open the device  
    handle = hid_open(VENDOR_ID, PRODUCT_ID, NULL);  
    if (!handle) {  
        printf("Failed to open UPS device.\n");  
        return 1;  
    }  
    printf("UPS device connected.\n");  

    // Send command  
    res = hid_write(handle, command, sizeof(command));  
    if (res < 0) {  
        printf("Failed to send command.\n");  
    } else {  
        printf("Command sent successfully.\n");  
    }  

    // Wait before reading  
    sleep(1);  

    // Read data  
    while (1) {  
        res = hid_read_timeout(handle, response, sizeof(response), TIMEOUT);  
        if (res > 0) {  
            for (int i = 0; i < res; i++) {  
                full_response[full_response_len++] = response[i];  
            }  
            if (full_response_len > 0 && full_response[full_response_len - 1] == '\0') {  
                break;  
            }  
        } else {  
            printf("Failed to read response or timeout.\n");  
            break;  
        }  
    }  

    // Null-terminate and print full response  
    full_response[full_response_len] = '\0';  
    printf("Full response: %s\n", full_response);  

    // Parse response data  
    float input_voltage, output_voltage, battery_voltage, load, frequency, avr, temperature;  
    char error_code[16] = {0};  

    int parsed_values = sscanf((char *)full_response, "(%f %f %f %f %f %f %f %15s)",  
                               &input_voltage, &output_voltage, &battery_voltage, &load,  
                               &frequency, &avr, &temperature, error_code);  

    if (parsed_values == 8) {  
        printf("Input Voltage: %.1f V\n", input_voltage);  
        printf("Output Voltage: %.1f V\n", output_voltage);  
        printf("Battery Voltage: %.1f V\n", battery_voltage);  
        printf("Load: %.1f %%\n", load);  
        printf("Frequency: %.1f Hz\n", frequency);  
        printf("Battery/Sound/AVR: %.2f\n", avr);  
        printf("Temperature: %.1f C\n", temperature);  
        printf("Error or Status Code: %s\n", error_code);  
    } else {  
        printf("Failed to parse response. Parsed values: %d\n", parsed_values);  
    }  

    // Close device  
    hid_close(handle);  
    hid_exit();  

    return 0;  
} 
