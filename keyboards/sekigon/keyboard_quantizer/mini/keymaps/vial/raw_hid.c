
void raw_hid_task(void) {
    uint8_t buffer[RAW_EPSIZE];
    size_t  size = 0;
    do {
        size = chnReadTimeout(&drivers.raw_driver.driver, buffer, sizeof(buffer), TIME_IMMEDIATE);
        if (size > 0) {
            raw_hid_receive(buffer, size);
        }
    } while (size > 0);
}