#include <iostream>
#include <libusb-1.0\libusb.h>

void ReadDeviceConfig(libusb_device* device);
void ChatWithDevice(libusb_device_handle* handle);

int main()
{
    // Initialize libusb and set up a session
    libusb_context* ctx = nullptr;
    int result = libusb_init(&ctx);
    if (result < 0) {
        // Handle error
        return result;
    }

    // Discover and open the USB device you want to communicate with:
    libusb_device** deviceList = nullptr;
    ssize_t count = libusb_get_device_list(ctx, &deviceList);
    if (count < 0) {
        // Handle error
        return count;
    }

    for (size_t i = 0; i < count; i++)
    {
        libusb_device* device = deviceList[i];

        struct libusb_device_descriptor descriptor;
        int result = libusb_get_device_descriptor(device, &descriptor);
        if (result != LIBUSB_SUCCESS) {
            // Handle error
            continue;
        }

        ReadDeviceConfig(device);

        // Access device information
        uint16_t vendorId = descriptor.idVendor;
        uint16_t productId = descriptor.idProduct;

        // Print or process the device information
        std::cout << "Device " << i << ": Vendor ID: 0x" << std::hex << vendorId << ", Product ID: 0x" << productId << std::dec << std::endl;

        libusb_device_handle* handle = libusb_open_device_with_vid_pid(ctx, vendorId, productId);
        if (!handle) {
            // Handle error
            return -1;
        }

        ChatWithDevice(handle);

        libusb_close(handle);

        break;
    }

    libusb_free_device_list(deviceList, 1);
    libusb_exit(ctx);

    //// Iterate through the list of devices and find the one you're interested in
    //libusb_device_handle* handle = libusb_open_device_with_vid_pid(ctx, vendorId, productId);
    //if (!handle) {
    //    // Handle error
    //    return -1;
    //}

    //// Free the device list when you're done with it
    //libusb_free_device_list(deviceList, 1);
}


void ReadDeviceConfig(libusb_device* device)
{
    libusb_config_descriptor* config;
    libusb_get_active_config_descriptor(device, &config);

    for (int i = 0; i < config->bNumInterfaces; i++)
    {
        const libusb_interface* interface = &config->interface[i];

        for (int j = 0; j < interface->num_altsetting; j++)
        {
            const libusb_interface_descriptor* interfaceDesc = &interface->altsetting[j];

            for (int k = 0; k < interfaceDesc->bNumEndpoints; k++)
            {
                const libusb_endpoint_descriptor* endpointDesc = &interfaceDesc->endpoint[k];

                // Access endpoint information
                uint8_t endpointAddress = endpointDesc->bEndpointAddress;
                uint8_t endpointNumber = endpointAddress & 0x0F;
                bool isInEndpoint = endpointAddress & 0x80;
            }
        }
    }

    libusb_free_config_descriptor(config);
}

void ChatWithDevice(libusb_device_handle* handle)
{
    unsigned char buffer[64];
    int transferred = 0;

    int result = libusb_bulk_transfer(handle, 0x02, buffer, sizeof(buffer), &transferred, 5000);
    if (result == LIBUSB_SUCCESS)
    {

    }
}
