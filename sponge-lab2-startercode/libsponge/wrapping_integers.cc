#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { 
    return WrappingInt32{isn.raw_value() + static_cast<uint32_t>(n)}; 
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    // DUMMY_CODE(n, isn, checkpoint);
    uint32_t ab_seqno_low = n - isn;  // 相减操作已经重载了
    uint64_t ab_seqno = (checkpoint & 0xFFFFFFFF00000000) + ab_seqno_low;

    // temp_low = ab_seqno >= (1ULL << 32) ? ab_seqno - (1ULL << 32) : 0：计算序列号减 2^32，确保不出现负数。
    uint64_t temp_low = ab_seqno >= (1ULL << 32) ? ab_seqno - (1ULL << 32) : 0;
    uint64_t temp_high = ab_seqno + (1ULL << 32);
    uint64_t closest = ab_seqno;

    if (std::llabs(static_cast<int64_t>(temp_high - checkpoint)) <
        std::llabs(static_cast<int64_t>(closest - checkpoint))) {
        closest = temp_high;
    }
    if (ab_seqno >= (1ULL << 32) &&
        std::llabs(static_cast<int64_t>(temp_low - checkpoint)) <
        std::llabs(static_cast<int64_t>(closest - checkpoint))) {
        closest = temp_low;
    }
    return closest;
}
