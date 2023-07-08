#include "crsf_protocol.hpp"
#include "my_crsf_serial.hpp"

#include <vector>
#include <ctime>
#include <memory>
#include <string.h>

MyCrsfSerial::MyCrsfSerial() :
    _crc(0xd5),
    _lastReceive(0), _lastChannelsPacket(0), _linkIsUp(false),
    _passthroughMode(false)
{
}

void MyCrsfSerial::handleBytes(uint8_t *sequence, size_t len)
{
    for (size_t i = 0; i < len; i++) {
		this->handleByte(sequence[i]);
	}

    checkPacketTimeout();
    checkLinkDown();
}

void MyCrsfSerial::handleBytes(std::vector<uint8_t> sequence)
{
    for (size_t i = 0; i < sequence.size(); i++) {
		this->handleByte(sequence[0]);
	}

    checkPacketTimeout();
    checkLinkDown();
}

crsf_header_t* MyCrsfSerial::handleByte(uint8_t b)
{
    this->_lastReceive = std::time(0);

	if (_passthroughMode)
        {
            if (onShiftyByte)
                onShiftyByte(b);
            return nullptr;
        }

	_rxBuf[_rxBufPos++] = b;
    auto result = handleByteReceived();

    if (_rxBufPos == (sizeof(_rxBuf)/sizeof(_rxBuf[0])))
    {
        // Packet buffer filled and no valid packet found, dump the whole thing
        _rxBufPos = 0;
    }

    return result;
}


crsf_header_t* MyCrsfSerial::handleByteReceived()
{
    crsf_header_t* result = nullptr;
    bool reprocess;
    do
    {
        reprocess = false;
        if (_rxBufPos > 1)
        {
            uint8_t len = _rxBuf[1];
            // Sanity check the declared length isn't outside Type + X{1,CRSF_MAX_PAYLOAD_LEN} + CRC
            // assumes there never will be a CRSF message that just has a type and no data (X)
            if (len < 4 || len > (CRSF_MAX_PAYLOAD_LEN + 3))
            {
                shiftRxBuffer(1);
                reprocess = true;
            }

            else if (_rxBufPos >= (len + 2))
            {
                uint8_t inCrc = _rxBuf[2 + len - 1];
                uint8_t crc = _crc.calc(&_rxBuf[2], len - 1);
                if (crc == inCrc)
                {
                    result = (crsf_header_t *)_rxBuf;
                    // processPacketIn(len);
                    shiftRxBuffer(len + 2);
                    reprocess = true;
                }
                else
                {
                    shiftRxBuffer(1);
                    reprocess = true;
                }
            }  // if complete packet
        } // if pos > 1
    } while (reprocess);

    return result;
}

void MyCrsfSerial::checkPacketTimeout()
{
    // If we haven't received data in a long time, flush the buffer a byte at a time (to trigger shiftyByte)
    if (_rxBufPos > 0 && std::time(0) - _lastReceive > CRSF_PACKET_TIMEOUT_MS)
        while (_rxBufPos)
            shiftRxBuffer(1);
}

void MyCrsfSerial::checkLinkDown()
{
    if (_linkIsUp && std::time(0) - _lastChannelsPacket > CRSF_FAILSAFE_STAGE1_MS)
    {
        if (onLinkDown)
            onLinkDown();
        _linkIsUp = false;
    }
}

void MyCrsfSerial::processPacketIn(uint8_t len)
{
    len = len;
    const crsf_header_t *hdr = (crsf_header_t *)_rxBuf;
    // if (hdr->device_addr == CRSF_ADDRESS_FLIGHT_CONTROLLER)
    // {
    onPacket(hdr);
    // } // CRSF_ADDRESS_FLIGHT_CONTROLLER
}

// Shift the bytes in the RxBuf down by cnt bytes
void MyCrsfSerial::shiftRxBuffer(uint8_t cnt)
{
    // If removing the whole thing, just set pos to 0
    if (cnt >= _rxBufPos)
    {
        _rxBufPos = 0;
        return;
    }

    if (cnt == 1 && onShiftyByte)
        onShiftyByte(_rxBuf[0]);

    // Otherwise do the slow shift down
    uint8_t *src = &_rxBuf[cnt];
    uint8_t *dst = &_rxBuf[0];
    _rxBufPos -= cnt;
    uint8_t left = _rxBufPos;
    while (left--)
        *dst++ = *src++;
}

void MyCrsfSerial::packetChannelsPacked(const crsf_header_t *p)
{
    crsf_channels_t *ch = (crsf_channels_t *)&p->data;
    _channels[0] = ch->ch0;
    _channels[1] = ch->ch1;
    _channels[2] = ch->ch2;
    _channels[3] = ch->ch3;
    _channels[4] = ch->ch4;
    _channels[5] = ch->ch5;
    _channels[6] = ch->ch6;
    _channels[7] = ch->ch7;
    _channels[8] = ch->ch8;
    _channels[9] = ch->ch9;
    _channels[10] = ch->ch10;
    _channels[11] = ch->ch11;
    _channels[12] = ch->ch12;
    _channels[13] = ch->ch13;
    _channels[14] = ch->ch14;
    _channels[15] = ch->ch15;

    for (unsigned int i=0; i<CRSF_NUM_CHANNELS; ++i)
        // _channels[i] = map(_channels[i], CRSF_CHANNEL_VALUE_1000, CRSF_CHANNEL_VALUE_2000, 1000, 2000);
        _channels[i] = ((_channels[i] - CRSF_CHANNEL_VALUE_1000) / (CRSF_CHANNEL_VALUE_2000 - CRSF_CHANNEL_VALUE_1000)) * (2000 - 1000) + 1000;

    if (!_linkIsUp && onLinkUp)
        onLinkUp();
    _linkIsUp = true;
    _lastChannelsPacket = std::time(0);

    if (onPacketChannels)
        onPacketChannels();
}

void MyCrsfSerial::packetLinkStatistics(const crsf_header_t *p)
{
    const crsfLinkStatistics_t *link = (crsfLinkStatistics_t *)p->data;
    memcpy(&_linkStatistics, link, sizeof(_linkStatistics));

    if (onPacketLinkStatistics)
        onPacketLinkStatistics(&_linkStatistics);
}

void MyCrsfSerial::packetGps(const crsf_header_t *p)
{
    const crsf_sensor_gps_t *gps = (crsf_sensor_gps_t *)p->data;
    _gpsSensor.latitude = be32toh(gps->latitude);
    _gpsSensor.longitude = be32toh(gps->longitude);
    _gpsSensor.groundspeed = be16toh(gps->groundspeed);
    _gpsSensor.heading = be16toh(gps->heading);
    _gpsSensor.altitude = be16toh(gps->altitude);
    _gpsSensor.satellites = gps->satellites;

    if (this->onPacketGps)
        this->onPacketGps(&_gpsSensor);
}

// uint8_t* MyCrsfSerial::queuePacket(uint8_t addr, uint8_t type, const void *payload, uint8_t len)
std::pair<uint8_t*, int> MyCrsfSerial::queuePacket(uint8_t msg_type_id, request_id_type request_id, const void *payload, uint8_t len)
{
    if (!_linkIsUp)
        return {};
    if (_passthroughMode)
        return {};
    if (len > CRSF_MAX_PAYLOAD_LEN)
        return {};

    crsf_header_t *package = new crsf_header_t();
    package->msg_type_id = msg_type_id;
    package->frame_size = len + 3;
    package->request_id = request_id;
    package->data = new uint8_t[len + 2];

    uint8_t* buf = (uint8_t*) package;

    buf[len+3] = _crc.calc(&buf[3], len + 2);



    // uint8_t buf[CRSF_MAX_PACKET_SIZE];
    // buf[0] = addr;
    // buf[1] = len + 2; // type + payload + crc
    // buf[2] = type;
    // memcpy(&buf[3], payload, len);
    // buf[len+3] = _crc.calc(&buf[2], len + 1);

    // Busywait until the serial port seems free
    //while (millis() - _lastReceive < 2)
    //    loop();

    // return buf;
    // return std::pair<uint8_t[CRSF_MAX_PACKET_SIZE], int>(buf, len + 4);
    return { buf, len + 4 };
}

void MyCrsfSerial::setPassthroughMode(bool val)
{
    _passthroughMode = val;
}
