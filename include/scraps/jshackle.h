#pragma once

#include "scraps/config.h"
#include "scraps/Buffer.h"

#include "jshackle/ToJava.h"
#include "jshackle/ToNative.h"

namespace jshackle {

template <typename T> constexpr size_t ElementSize() { return sizeof(T); }
template <> constexpr size_t ElementSize<void>() { return 1; }
template <> constexpr size_t ElementSize<const void>() { return 1; }

template <typename BufferT, template<typename> class B>
struct ToJava < const B<BufferT>, typename std::enable_if < std::is_same<B<BufferT>, scraps::Buffer<BufferT>>::value, void >::type > {
    typedef jobject Type;
    static constexpr const char* Signature(JNIContext& jniContext) { return "Ljava/nio/ByteBuffer;"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, const B<BufferT> buffer) {
        return env->NewDirectByteBuffer(const_cast<void*>(reinterpret_cast<const void*>(buffer.data())), buffer.size() * ElementSize<BufferT>());
    }
};

template <typename BufferT>
struct ToNative<jobject, const scraps::Buffer<BufferT>&> {
    typedef const scraps::Buffer<BufferT> Type;
    static Type Convert(JNIContext& jniContext, JNIEnv* env, jobject obj) {
        return scraps::Buffer<BufferT>(reinterpret_cast<BufferT*>(env->GetDirectBufferAddress(obj)), env->GetDirectBufferCapacity(obj) / ElementSize<BufferT>());
    }
};

} // namespace jshackle
