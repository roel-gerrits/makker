//
// Created by roel on 6/17/22.
//

#include "RewindableTokenStream.h"

RewindableTokenStream::RewindableTokenStream(TokenStream &source) :
        source(source),
        buffer(),
        buffer_pos(buffer.begin()),
        buffer_start_index(0),
        buffer_current_index(0) {}

const Token &RewindableTokenStream::peek() {
    if (buffer_pos == buffer.end()) {
        buffer_pos = buffer.emplace(buffer.end(), source.next());
    }
    return *buffer_pos;
}

const Token &RewindableTokenStream::next() {
    if (buffer_pos == buffer.end()) {
        buffer_pos = buffer.emplace(buffer.end(), source.next());
        buffer_current_index++;
    } else {
        buffer_current_index++;
    }

    const Token &t = *buffer_pos;
    buffer_pos++;
    return t;
}

RewindableTokenStream::Snapshot::Snapshot(unsigned int index) :
        index(index) {}

RewindableTokenStream::Snapshot RewindableTokenStream::snapshot() {
    return RewindableTokenStream::Snapshot(buffer_current_index);
}

void RewindableTokenStream::rewind(Snapshot snapshot) {
    buffer_pos = buffer.begin();
    buffer_current_index = snapshot.index;
    buffer_pos = std::next(buffer_pos, buffer_current_index - buffer_start_index);
}

void RewindableTokenStream::print() {
    for (auto it = buffer.begin(); it != buffer.end(); it++) {
        printf(" > %s %s \n", to_str(it->type), (it == buffer_pos) ? "<--" : "");
    }
}
