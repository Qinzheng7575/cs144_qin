#include "byte_stream.hh"

#include <algorithm>
#include <iterator>
#include <stdexcept>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

//template <typename... Targs>
//void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;
//构造函数，用来初始化
ByteStream::ByteStream(const size_t capacity)
    : my_buffer(capacity)
    , _capacity(capacity)
    , _written_num(0) 
    , _read_num(0)
    , buffer_begin(0)
    , buffer_end(0)
    , _input_end_flag(false), _error(false)
    {}

size_t ByteStream::write(const string &data) { 
    size_t n = min(data.size(), remaining_capacity());
    for (size_t i = 0; i < n; i++) {
        buffer_end = (buffer_end + 1) % _capacity;
        my_buffer[buffer_end] = data[i];
    }
    _written_num += n;
    return n;
}

//! \param[in] len bytes will be copied from the output side of the buffer 只做复制的事情，其他不用管
string ByteStream::peek_output(const size_t len) const {
    string out;
    size_t n = min(len, buffer_size());
    // 提前分配好内存以避免多次重新分配
    out.reserve(n);

    // 从 buffer_begin 开始读取 n 个字节
    for (size_t i = 0; i < n; i++) {
        out.push_back(my_buffer[(buffer_begin + i) % _capacity]);  // 去掉 +1，假设_capacity定义为实际容量
    }
    return out;
}

//! \param[in] len bytes will be removed from the output side of the buffer 在这里才改变begin和end
void ByteStream::pop_output(const size_t len) { 
    size_t n = min(len, buffer_size());
    buffer_begin = (buffer_begin + n) % _capacity;
    _read_num += n;
}



void ByteStream::end_input() { _input_end_flag = true; }

bool ByteStream::input_ended() const { return _input_end_flag; }

size_t ByteStream::buffer_size() const { return my_buffer.size(); }

bool ByteStream::buffer_empty() const { return my_buffer.size() == 0; }

bool ByteStream::eof() const { return input_ended() && buffer_empty(); }

size_t ByteStream::bytes_written() const { return _written_num; }

size_t ByteStream::bytes_read() const { return _read_num; }

size_t ByteStream::remaining_capacity() const { return _capacity - my_buffer.size(); }
