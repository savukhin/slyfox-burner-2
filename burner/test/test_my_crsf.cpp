#include <gtest/gtest.h>
#include "my_crsf_serial.hpp"
#include "messages.hpp"

TEST(Crsf, Full)
{
    // config_message_t *msg = new config_message_t{100, 100};
    uint32_t init_msg = 123423;
    uint32_t *init_msg_ptr = &init_msg;

    int32_t message_id = 243;
    int32_t request_id = 233;
    int32_t len = 4;

    MyCrsfSerial<int32_t, int32_t, int32_t> crsf_1;
    auto response = crsf_1.queuePacket(request_id, len, message_id, (void*)init_msg_ptr);

    EXPECT_NE(response, nullptr);
    EXPECT_EQ(response->get_msg_type_id(), message_id);
    EXPECT_EQ(response->get_request_id(), request_id);
    
    void *payload = response->get_payload();
    int *payload_int = (int*)payload;
    EXPECT_EQ(*payload_int, init_msg);

    uint8_t *payload_uint = (uint8_t*)payload;

    uint8_t *header = response->to_bytes();

    // [request_id] [length] [msg_type_id] [payload 4 bytes] [crc]
    int header_length = sizeof(request_id) + sizeof(len) + sizeof(message_id) + 4 + 1;

    for (int i = 0; i < header_length - 1; i++) {
        auto res = crsf_1.handleByte(header[i]);
        EXPECT_EQ(res, nullptr);
    }
    
    auto res = crsf_1.handleByte(header[header_length - 1]);
    EXPECT_NE(res, nullptr);

    // payload = res->get_payload();
    // payload_int = (int*)payload;
    // EXPECT_EQ(*payload_int, init_msg);

    // uint8_t *header2 = response->to_bytes();
    // for (int i = 0; i < header_length; i++) {
    //     EXPECT_EQ(header[i], header2[i]);
    // }
}

#if defined(ARDUINO)
#include <Arduino.h>

void setup()
{
    // should be the same value as for the `test_speed` option in "platformio.ini"
    // default value is test_speed=115200
    Serial.begin(115200);

    ::testing::InitGoogleTest();
    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock();
}

void loop()
{
  // Run tests
  if (RUN_ALL_TESTS())
  ;

  // sleep for 1 sec
  delay(1000);
}

#else
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock(&argc, argv);

    if (RUN_ALL_TESTS())
    ;

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}
#endif