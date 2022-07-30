//
// Created by Pierluigi Carlino on 30/07/22.
//

#include "customTransf.hpp"

TransfBuff* TransfBuff::instance(){
    static TransfBuff *instance;
    instance = new TransfBuff();

    return instance;
}

ProjectBuffer* ProjectBuffer::instance(
        float w = 1.0f,
        float n = 1.0f,
        float f = 1.0f
){
    static ProjectBuffer *instance;
    instance = new ProjectBuffer(w,n,f);

    return instance;
}
