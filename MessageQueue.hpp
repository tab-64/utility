/*
*
*/
#ifndef __MESSAGEQUEUE_HPP__
#define __MESSAGEQUEUE_HPP__

// Import C++ Standard Template Library headers.
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <utility>

namespace FlexChat{

namespace Util {

using MessageID     = int;
using MessageParam  = unsigned long long;
using MessageStr    = std::string;

//
// 消息类型
//
enum class MessageType{
    Signal = 0, // 信号型
    Common = 1, // 普通型
    String = 2, // 字符串型
    Mixed  = 3  // 混合型
};

const int        NO_PARAM = 0;
const MessageStr NO_STR = "";
//
// 抽象类: Message
// 作为访问不同类型消息的接口
// 
class MessageBase{
public:
    virtual const MessageType& type() = 0;  // 消息类型
    virtual const MessageID&   id() = 0;    // 消息标识符
    virtual MessageParam       param() = 0; // 消息参数
    virtual MessageStr         info() = 0;  // 消息信息(字符串)

};

//
// 指向消息对象的指针(采用智能指针)
//
using MessagePtr = std::shared_ptr<MessageBase>;

//
// 类: SignalMessage
// 信号型消息, 只包含一个标识符(id)
//
class SignalMessage : public MessageBase{
public:
    // 禁止使用隐式类型转换
    explicit SignalMessage(const MessageID& id){
        id_ = id;
    }
    const MessageType& type(void){
        return std::move(MessageType::Signal);
    }
    const MessageID& id(void){
        return id_;
    }
    MessageParam param(void){
        return NO_PARAM;
    }
    MessageStr info(void){
        return NO_STR;
    }

protected:
    MessageID id_;

};

//
// 类: CommonMessage
// 普通消息, 包含标识符(id)和参数(param)
//
class CommonMessage : public MessageBase{
public:
    explicit CommonMessage(const MessageID& id, const MessageParam& param){
        id_     = id;
        param_  = param;
    }
    const MessageType& type(void){
        return std::move(MessageType::Common);
    }
    const MessageID& id(void){
        return id_;
    }
    MessageParam param(void){
        return param_;
    }
    MessageStr info(void){
        return NO_STR;
    }

protected:
    MessageID    id_;
    MessageParam param_;
};

//
// 类: StringMessage
// 字符串型消息, 包含标识符(id)和一个字符串(str)
//
class StringMessage : public MessageBase{
public:
    explicit StringMessage(const MessageID& id, 
        const MessageStr& str){

        id_   = id;
        str_  = str;
    }
    const MessageType& type(void){
        return std::move(MessageType::String);
    }
    const MessageID& id(void){
        return id_;
    }
    MessageParam param(void){
        return NO_PARAM;
    }
    MessageStr info(void){
        return str_;
    }

protected:
    MessageID  id_;
    MessageStr str_;
};

//
// 类: MixedMessage
// 混合消息, 包含标识符(id),参数(param),字符串(str)
//
class MixedMessage : public MessageBase{
public:
    explicit MixedMessage(const MessageID& id, 
        const MessageParam& param, const MessageStr& str){
        id_     = id;
        param_  = param;
        str_    = str;
    }
    const MessageType& type(void){
        return std::move(MessageType::Mixed);
    }
    const MessageID& id(void){
        return id_;
    }
    MessageParam param(void){
        return param_;
    }
    MessageStr info(void){
        return str_;
    }

protected:
    MessageID     id_;
    MessageParam  param_;
    MessageStr    str_;
};

// 
// 类: Message
// 用于创建Message实例(使用create方法),
// 实例类型取决于提供的参数类型
//
class Message{
public:
    // Signal.
    static MessagePtr create(const MessageID& id){
        MessagePtr tmp(new SignalMessage(id));
        return tmp;
    }

    // Common.
    static MessagePtr create(const MessageID& id, 
        const MessageParam& param){

        MessagePtr tmp(new CommonMessage(id, param));
        return tmp;
    }

    // String.
    static MessagePtr create(const MessageID& id, 
        const MessageStr& str){

        MessagePtr tmp(new StringMessage(id, str));
        return tmp;
    }

    // Mixed.
    static MessagePtr create(const MessageID& id, 
        const MessageParam& param, const MessageStr& str){

        MessagePtr tmp(new MixedMessage(id, param, str));
        return tmp;
    }
};

//
// 类: MessageQueue
// 消息队列, 包含指向消息实例的指针.
//
class MessageQueue{
public:
    explicit MessageQueue(void){};

    // 加入新消息
    void push(const MessagePtr& msg){
        msg_queue_.push(msg);
    };
    // 移出队首消息
    void pop(void){
        msg_queue_.pop();
    }
    // 获取队首消息(不移出)
    MessagePtr front(void){
        return msg_queue_.front();
    }
    // 获取队尾消息(不移出)
    MessagePtr back(void){
        return msg_queue_.back();
    }
    size_t size(void){
        return msg_queue_.size();
    }
    bool empty(void){
        return msg_queue_.empty();
    }

    MessageQueue& operator<<(const MessagePtr& rhs){
        push(rhs);
        return *this;
    }
    MessageQueue& operator>>(MessagePtr& rhs){
        rhs = front();
        pop();
        return *this;
    }
protected:
    using MsgQueue = std::queue<MessagePtr>;

protected:
    MsgQueue msg_queue_;
};


class MessageQueue_threadsafe : protected MessageQueue{
public:
    explicit MessageQueue_threadsafe(void){}
    void operator=(MessageQueue)=delete;
    void operator=(MessageQueue_threadsafe)=delete;

    // 加入新消息
    void push(const MessagePtr& msg){
        msg_queue_.push(msg);
    };
    // 移出队首消息
    void pop(void){
        msg_queue_.pop();
    }
    // 获取队首消息(不移出)
    MessagePtr front(void){
        return msg_queue_.front();
    }
    // 获取队尾消息(不移出)
    MessagePtr back(void){
        return msg_queue_.back();
    }
    size_t size(void){
        return msg_queue_.size();
    }
    bool empty(void){
        return msg_queue_.empty();
    }

    MessageQueue& operator<<(const MessagePtr& rhs){
        push(rhs);
        return *this;
    }
    MessageQueue& operator>>(MessagePtr& rhs){
        rhs = front();
        pop();
        return *this;
    }
protected:
    std::mutex mtx_;
    std::condition_variable cv_;
};

} // namespace Util

} // namespace FlexChat

#endif // __MESSAGEQUEUE_HPP__