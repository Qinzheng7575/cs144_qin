#include "tcp_receiver.hh"
#define _LISTEN_ 0
#define _SYN_RECV_ 1
#define _FIN_RECV_ 2
#define _ERROR_ -1
// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

bool TCPReceiver::segment_received(const TCPSegment &seg) {
    _STATE_ = _LISTEN_;//syn报文还没来
    const TCPHeader &header = seg.header();

    switch (_STATE_) {
        case _LISTEN_:
            if (header.syn) {
                _isn = header.seqno; //是syn报文，那么seqno就是isn了
                _STATE_ = _SYN_RECV_;
            } else {  // 注意 SYN 包之前的数据包必须全部丢弃
                return;
            }
            break;
        default:
            break;
    }
    uint64_t checkpoint = _reassembler.stream_out().bytes_written();
    uint64_t ab_seqno = unwrap(header.seqno, _isn, checkpoint); //seqno是TCP报文头部中使用的WrappingInt32

    uint64_t stream_index = ab_seqno - 1 + (header.syn ? 1 : 0);
    _reassembler.push_substring(seg.payload().copy(), stream_index, header.fin);
    return True;
}

optional<WrappingInt32> TCPReceiver::ackno() const { 
    if (_reassembler.stream_out().input_ended()) {
        _STATE_ = _FIN_RECV_;
    }
    switch (_STATE_) {
        case _LISTEN_:
            return nullopt;
        case _SYN_RECV_:
            uint64_t ackno = _reassembler.stream_out().bytes_written() + 1;
            break;
        case _FIN_RECV_:
            uint64_t ackno = _reassembler.stream_out().bytes_written() + 2;
            break;
        default:
            break;
    }
    return WrappingInt32(_isn) + ackno; 
}

size_t TCPReceiver::window_size() const { return {_capacity - _reassembler.stream_out().buffer_size()}; }
