#pragma once

#include <string.h>

#include "../crsf/my_crsf_serial_interface.hpp"
#include "crc8.hpp"
#define PACKED __attribute__((packed))

template <typename Req, typename Len, typename Msg>
struct crsf_header_t {
    Req request_id;        // from crsf_frame_type_e
    Len frame_size;  // counts size after this byte, so it must be the payload size + (sizeof msg and crc)
    Msg msg_type_id;      // from crsf_addr_e
    uint8_t *data;
    uint8_t crc;
} PACKED;

template <typename Req, typename Len, typename Msg>
class CrsfHeader : public IHeader
{
private:
    crsf_header_t<Req, Len, Msg> *header;
    long long total_length_;
    long long payload_length_;

public:
    CrsfHeader(Req request_id, Len len, Msg msg_type_id, const void* payload, Crc8 &crc8) {
        header = new crsf_header_t<Req, Len, Msg>();
        header->msg_type_id = msg_type_id;
        header->frame_size = len + sizeof(Msg) + 1;
        header->request_id = request_id;
        header->data = new uint8_t[len];
        memcpy(header->data, payload, len);

        uint8_t* buf = new uint8_t[len + sizeof(Msg)];
        
        Msg *msg = &msg_type_id;
        memcpy(buf, msg, sizeof(Msg));
        memcpy(buf + sizeof(Msg), payload, len);
        
        auto sum = crc8.calc(buf, len + sizeof(Msg));
        delete[] buf;

        header->crc = sum;

        // [req] [len] [msg] [payload] [crc]
        this->total_length_ = sizeof(Req) + sizeof(Len) + sizeof(Msg) + len + 1;
        this->payload_length_ = len;
    }

    CrsfHeader(void* bytes) {
        uint8_t *bytes_casted = reinterpret_cast<uint8_t *>(bytes);
        
        this->header = new crsf_header_t<Req, Len, Msg>();
        Req *req = new Req;
        Len *len = new Len;
        Msg *msg = new Msg;
        uint8_t *crc = new uint8_t;

        size_t i = 0;
        memcpy(req, bytes_casted + i, sizeof(Req));
        i += sizeof(Req);

        memcpy(len, bytes_casted + i, sizeof(Len));
        i += sizeof(Len);

        memcpy(msg, bytes_casted + i, sizeof(Msg));
        i += sizeof(Msg);

        uint8_t *payload = new uint8_t[*len + sizeof(Msg) + 1];

        memcpy(payload, bytes_casted + i, *len);
        i += *len;

        memcpy(crc, bytes_casted + i, 1);

        this->header->request_id = *req;
        this->header->frame_size = *len;
        this->header->msg_type_id = *msg;
        this->header->data = payload;
        this->header->crc = *crc;

        // free(req);
        // free(len);
        // free(msg);
        // free(crc);

        delete req;
        delete len;
        delete msg;
        delete crc;
    }

    int get_msg_type_id() override { return header->msg_type_id; };
    long long get_request_id() override { return header->request_id; };
    int get_frame_length() override { return header->frame_size; };
    int get_total_length() override { return this->total_length_; };
    int get_sum() override {return header->crc; }

    void* get_payload() override { return (void*)header->data; }

    // uint8_t* get_header() override { return (uint8_t*)header; }
    uint8_t* to_bytes() override {
        uint8_t *buf = new uint8_t[this->total_length_];

        int i = 0;
        memcpy(buf + i, &this->header->request_id, sizeof(Req));
        i += sizeof(Req);
        memcpy(buf + i, &this->header->frame_size, sizeof(Len));
        i += sizeof(Len);
        memcpy(buf + i, &this->header->msg_type_id, sizeof(Msg));
        i += sizeof(Msg);

        memcpy(buf + i, header->data, this->payload_length_);
        i += this->payload_length_;
        memcpy(buf + i, &this->header->crc, 1);

        return buf;
    }
} PACKED;
