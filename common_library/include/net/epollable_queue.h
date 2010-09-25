/**
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: eyjian@qq.com or eyjian@gmail.com
 */
#ifndef EPOLLABLE_QUEUE_H
#define EPOLLABLE_QUEUE_H
#include "sys/lock.h"
#include "net/epollable.h"
NET_NAMESPACE_BEGIN

/** 可以放入Epoll监控的队列
  * RawQueueClass为原始队列类名，如util::CArrayQueue
  * 为线程安全类
  */
template <class RawQueueClass>
class CEpollableQueue: public CEpollable
{
    typedef typename RawQueueClass::_DataType DataType;
    
public:
    /** 构造一个可Epoll的队列，注意只可监控读事件，也就是队列中是否有数据
      * @queue_max: 队列最大可容纳的元素个数
      * @exception: 如果出错，则抛出CSyscallException异常
      */
    CEpollableQueue(uint32_t queue_max)
        :_raw_queue(queue_max)
    {
        _notify[0] = 'X';
        if (-1 == pipe(_pipefd)) throw sys::CSyscallException(errno, __FILE__, __LINE__);
        set_fd(_pipefd[0]);
    }
    
    ~CEpollableQueue()
    {
        close();
    }

    /** 关闭队列 */
    virtual void close()
    {
        sys::CLockHelper<sys::CLock> lock_helper(_lock);
        close_fd(_pipefd[0]);
        close_fd(_pipefd[1]);
        _pipefd[0] = -1;
        _pipefd[1] = -1;
    }

    /** 判断队列是否已满 */
    bool is_full() const 
	{
        sys::CLockHelper<sys::CLock> lock_helper(_lock);
        return _raw_queue.is_full();
    }
    
    /** 判断队列是否为空 */
    bool is_empty() const 
	{
        sys::CLockHelper<sys::CLock> lock_helper(_lock);
        return _raw_queue.is_empty();
    }

    /***
      * 取队首元素
      * @elem: 存储取到的队首元素
      * @return: 如果队列为空，则返回false，否则返回true
      */
    bool front(DataType& elem) const 
	{
        sys::CLockHelper<sys::CLock> lock_helper(_lock);
        if (_raw_queue.is_empty()) return false;

        elem = _raw_queue.front();
        return true;
    }
    
	/***
      * 弹出队首元素
      * @elem: 存储弹出的队首元素
      * @return: 如果队列为空，则返回false，否则取到元素并返回true
      * @exception: 如果出错，则抛出CSyscallException异常
      */
    bool pop_front(DataType& elem) 
	{
        {        
            sys::CLockHelper<sys::CLock> lock_helper(_lock);
            if (_raw_queue.is_empty()) return false; // 没有数据，也不阻塞
        }
        
        // read还有相当于CEvent::wait的作用
        while (-1 == read(_pipefd[0], _notify, sizeof(_notify)))
        {
            if (errno != EINTR)
                throw sys::CSyscallException(errno, __FILE__, __LINE__);
        }

        elem = _raw_queue.pop_front();
        return true;
    }
    
	/***
      * 向队尾插入一元素
      * @elem: 待插入到队尾的元素
      * @return: 如果队列已经满，则返回false，否则插入成功并返回true
      * @exception: 如果出错，则抛出CSyscallException异常
      */
    bool push_back(DataType elem) 
	{
        {                    
            sys::CLockHelper<sys::CLock> lock_helper(_lock);
            if (_raw_queue.is_full()) return false;
        }

        _raw_queue.push_back(elem);
        // write还有相当于signal的作用
        while (-1 == write(_pipefd[1], _notify, sizeof(_notify)))
        {
            if (errno != EINTR)
                throw sys::CSyscallException(errno, __FILE__, __LINE__);
        }

        return true;
    }

    /** 得到队列中当前存储的元素个数 */
    uint32_t size() const 
	{ 
        sys::CLockHelper<sys::CLock> lock_helper(_lock);
        return _raw_queue.size(); 
	}

private:
    int _pipefd[2]; /** 管道句柄 */
    char _notify[1];
    mutable sys::CLock _lock;
    RawQueueClass _raw_queue; /** 普通队列实例 */
};

NET_NAMESPACE_END
#endif // EPOLLABLE_QUEUE_H
