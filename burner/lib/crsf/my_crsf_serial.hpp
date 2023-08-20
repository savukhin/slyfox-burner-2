#pragma once

#include "crc8.hpp"
#include "crsf_protocol.hpp"
#include "my_crsf_serial_interface.hpp"
#include "crsf_header.hpp"

#include <ctime>
#include <memory>
#include <string.h>
#include <functional>
#include <vector>
#include <stdexcept>
#include <limits>


enum eFailsafeAction { fsaNoPulses, fsaHold };

template <typename Req, typename Len, typename Msg>
class MyCrsfSerial : public IByteHandler
{
private:
    Len max_payload_length_;

    long long max_len_param_;
    long long min_len_param_;
public:
    // Packet timeout where buffer is flushed if no data is received in this time
    static const unsigned int CRSF_PACKET_TIMEOUT_MS = 100;
    static const unsigned int CRSF_FAILSAFE_STAGE1_MS = 300;

    MyCrsfSerial() : _crc(0xd5),
        _lastReceive(0), _lastChannelsPacket(0), _linkIsUp(false),
        _passthroughMode(false) {
            this->max_payload_length_ = (((long long)std::numeric_limits<Len>::max()) + 1) / 8;

            this->max_len_param_ = sizeof(Msg) + this->max_payload_length_ + 1;
            this->min_len_param_ = sizeof(Msg) + 1 + 1;
        }

    IHeader *makePacket(const IMessage &msg, long long req_id) override {
        void* msg_bytes = msg.toBytes();
        auto len = msg.getByteLen();
        auto id = msg.get_id();
        return this->makePacket(req_id, len, msg.get_id(), msg_bytes);
    }

    IHeader *makePacket(Req request_id, Len len, Msg msg_type_id, const void *payload) {
        // if (!_linkIsUp)
        //     throw new std::runtime_error("cannot packet: link is down");
        // if (_passthroughMode)
        //     throw new std::runtime_error("cannot packet: passthough mode is on");

        if (len > this->max_len_param_)
            throw new std::runtime_error("cannot packet: len is bigger that possible");

        IHeader *header = new CrsfHeader<Req, Len, Msg>(request_id, len, msg_type_id, payload, this->_crc);
        return header;
    }

    IHeader* handleByte(uint8_t b) override {
        this->_lastReceive = std::time(0);

        if (_passthroughMode)
            {
                // if (onShiftyByte)
                //     onShiftyByte(b);
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
    void handleBytes(std::vector<uint8_t> sequence) {
        for (size_t i = 0; i < sequence.size(); i++) {
            this->handleByte(sequence[0]);
        }

        checkPacketTimeout();
        checkLinkDown();
    }
    void handleBytes(uint8_t *sequence, size_t len) {
        for (size_t i = 0; i < len; i++) {
            this->handleByte(sequence[i]);
        }

        checkPacketTimeout();
        checkLinkDown();
    }

    // Return current channel value (1-based) in us
    int getChannel(unsigned int ch) const { return _channels[ch - 1]; }
    int* getChannels() { return _channels; }
    const crsfLinkStatistics_t *getLinkStatistics() const { return &_linkStatistics; }
    const crsf_sensor_gps_t *getGpsSensor() const { return &_gpsSensor; }
    bool isLinkUp() const { return _linkIsUp; }
    bool getPassthroughMode() const { return _passthroughMode; }
    void setPassthroughMode(bool val) { _passthroughMode = val; }

    // Event Handlers
    // std::function<void(const crsf_header_t*)> onPacket;

private:
    uint8_t _rxBuf[CRSF_MAX_PACKET_SIZE];
    uint8_t _rxBufPos;
    Crc8 _crc;
    crsfLinkStatistics_t _linkStatistics;
    crsf_sensor_gps_t _gpsSensor;
    uint32_t _lastReceive;
    uint32_t _lastChannelsPacket;
    bool _linkIsUp;
    bool _passthroughMode;
    int _channels[CRSF_NUM_CHANNELS];

    Len* tryExtractLength() {
        if (_rxBufPos < sizeof(Req) + sizeof(Len)) {
            return nullptr;
        }

        uint8_t *buf = new uint8_t[sizeof(Len)];
        memcpy(buf, _rxBuf + sizeof(Req), sizeof(Len));
        Len *len = (Len*)buf;

        return len;
    }
    
    IHeader* handleByteReceived() {
        CrsfHeader<Req, Len, Msg>* result = nullptr;
        bool reprocess;
        do
        {
            reprocess = false;
            if (_rxBufPos > 1)
            {
                // uint8_t len = _rxBuf[1];
                Len *len_ptr = tryExtractLength();
                if (!len_ptr) {
                    continue;
                }

                Len len = *len_ptr;
                

                // Sanity check the declared length isn't outside Type + X{1,CRSF_MAX_PAYLOAD_LEN} + CRC
                // assumes there never will be a CRSF message that just has a type and no data (X)
                if (len < this->min_len_param_ || len > this->max_len_param_)
                {
                    shiftRxBuffer(1);
                    reprocess = true;
                }

                else if (_rxBufPos >= (len + sizeof(Req) + sizeof(Len)))
                {
                    uint8_t inCrc = _rxBuf[sizeof(Req) + sizeof(Len) + len - 1];
                    uint8_t crc = _crc.calc(&_rxBuf[sizeof(Req) + sizeof(Len)], len - 1);

                    if (crc == inCrc)
                    {
                        result = new CrsfHeader<Req, Len, Msg>((void*)_rxBuf);
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
    void shiftRxBuffer(uint8_t cnt) {
        // If removing the whole thing, just set pos to 0
        if (cnt >= _rxBufPos)
        {
            _rxBufPos = 0;
            return;
        }

        // Otherwise do the slow shift down
        uint8_t *src = &_rxBuf[cnt];
        uint8_t *dst = &_rxBuf[0];
        _rxBufPos -= cnt;
        uint8_t left = _rxBufPos;
        while (left--)
            *dst++ = *src++;
    }

    void checkPacketTimeout() {
        if (_rxBufPos > 0 && std::time(0) - _lastReceive > CRSF_PACKET_TIMEOUT_MS)
        while (_rxBufPos)
            shiftRxBuffer(1);
    }
    void checkLinkDown() {
        if (_linkIsUp && std::time(0) - _lastChannelsPacket > CRSF_FAILSAFE_STAGE1_MS)
        {
            _linkIsUp = false;
        }
    }
};
