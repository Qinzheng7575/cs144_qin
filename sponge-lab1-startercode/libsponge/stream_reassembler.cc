#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : 
    _output(capacity), 
    _capacity(capacity),
    _temp_stream(capacity),
    _cur_index(0),//当前已经顺序写入到输出流的数据的索引
    _eof_index(),//??????????????
    _unassembled_bytes(0){}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) { 
    size_t _start = max(index, _cur_index);
    /*如果 index 小于当前已经组装过的字节数（即 _cur_index），则跳过这一部分。
    st = max(index, _cur_index)表示插入的数据在 _cur_index 之后的部分才会真正放入缓冲区。*/
    size_t _end = min(index + data.size() , _cur_index + _capacity - _output.buffer_size());//这里看那个图最好理解
    
    if (eof) {
        // 由于数据可能乱序到达，并且 eof 可能出现在不同的数据段中，因此需要不断更新 _eof_index 来确保它指向最早的流结束位置。
        _eof_index = min(_eof_index, index + data.size());
    }
    
    //写将data按需填充到缓冲区的部分
    //j这样写，是因为由于之前的部分已经被组装过了，所以需要从data子串的中间位置开始插入
    for (size_t i = _start, j = _start - index; i < _end; i++, j++) {
        // 获取 _stream 的当前插入位置，确保循环访问
        auto &stream_slot = _temp_stream[i % _capacity];//就当它是i吧

        // 如果该位置已有数据，确保与即将插入的数据一致
        if (stream_slot.second == true) {  //<pair>中的第二个bool变量,是否有有效数据
            if (stream_slot.first != data[j]) {
                throw runtime_error("Inconsistent data at the same position in stream.");
            }
        }
        // 如果该位置没有数据，则将新数据插入该位置
        else {
            stream_slot = std::make_pair(data[j], true);
            ++_unassembled_bytes;
        }
    }

    string str;
    //写将缓冲区写入输出流_output的部分,_cur_index要在eof之前并且内容有效（后面一点很重要）
    while (_cur_index < _eof_index && _temp_stream[_cur_index % _capacity].second == true) {
        str.push_back(_temp_stream[_cur_index % _capacity].first);
        _temp_stream[_cur_index % _capacity] = {0, false};//循环队列，所以要这样写
        _unassembled_bytes--;
        _cur_index++;
    }

    _output.write(str);

    if (_cur_index == _eof_index) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_bytes; }

bool StreamReassembler::empty() const { return _unassembled_bytes == 0; }
