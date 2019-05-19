/*************************************************************************
  > File Name: SignalSlot.hpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年05月19日 星期日 10时23分54秒
 ************************************************************************/

#ifndef PALLETTE_SIGNALSLOT_HPP
#define PALLETTE_SIGNALSLOT_HPP

#include "noncopyable.h"

#include <memory>
#include <vector>
#include <mutex>

#include <assert.h>

//信号和槽机制的实现
namespace pallette
{

    namespace detail
    {

        template<typename Callback>
        struct SlotImpl;

        template<typename Callback>
        struct SignalImpl : noncopyable
        {
            typedef std::vector<std::weak_ptr<SlotImpl<Callback> > > SlotList;

            SignalImpl()
                : slots_(new SlotList)
            {
            }

            void copyOnWrite()//写时拷贝，调用此函数外部必须加锁
            {
                //mutex_.assertLocked();
                if (!slots_.unique())
                {
                    slots_.reset(new SlotList(*slots_));
                }
                assert(slots_.unique());
            }

            void clean()
            {
                std::unique_lock<std::mutex> lock(mutex_);
                copyOnWrite();
                SlotList& list(*slots_);
                typename SlotList::iterator it(list.begin());
                while (it != list.end())
                {
                    if (it->expired())
                    {
                        it = list.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }

            std::mutex mutex_;
            std::shared_ptr<SlotList> slots_;//SignalImpl持有SlotImpl的shared_ptr
        };

        template<typename Callback>
        struct SlotImpl : noncopyable
        {
            typedef SignalImpl<Callback> Data;
            SlotImpl(const std::shared_ptr<Data>& data, Callback&& cb)
                : data_(data), cb_(cb), tie_(), tied_(false)
            {
            }

            SlotImpl(const std::shared_ptr<Data>& data, Callback&& cb,
                const std::shared_ptr<void>& tie)
                : data_(data), cb_(cb), tie_(tie), tied_(true)
            {
            }

            ~SlotImpl()
            {
                std::shared_ptr<Data> data(data_.lock());
                if (data)
                {
                    data->clean();
                }
            }

            std::weak_ptr<Data> data_; // SlotImpl持有SignalImpl的weak_ptr
            Callback cb_;
            std::weak_ptr<void> tie_;
            bool tied_;
        };

    }

    /// This is the handle for a slot
    ///
    /// The slot will remain connected to the signal fot the life time of the
    /// returned Slot object (and its copies).
    typedef std::shared_ptr<void> Slot;

    template<typename Signature>
    class Signal;

    template <typename RET, typename... ARGS>
    class Signal<RET(ARGS...)> : noncopyable
    {
    public:
        typedef std::function<RET(ARGS...)> Callback;
        typedef detail::SignalImpl<Callback> SignalImpl;
        typedef detail::SlotImpl<Callback> SlotImpl;

        Signal()
            : impl_(new SignalImpl)
        {
        }

        ~Signal()
        {
        }

        Slot connect(Callback&& func)
        {
            std::shared_ptr<SlotImpl> slotImpl(
                new SlotImpl(impl_, std::forward<Callback>(func)));
            add(slotImpl);
            return slotImpl;
        }

        Slot connect(Callback&& func, const std::shared_ptr<void>& tie)
        {
            std::shared_ptr<SlotImpl> slotImpl(new SlotImpl(impl_, func, tie));
            add(slotImpl);
            return slotImpl;
        }

        void call(ARGS&&... args)
        {
            SignalImpl& impl(*impl_);
            std::shared_ptr<typename SignalImpl::SlotList> slots;
            {
                std::unique_lock<std::mutex> lock(impl_->mutex_);
                slots = impl.slots_;
            }
            typename SignalImpl::SlotList& s(*slots);
            for (typename SignalImpl::SlotList::const_iterator it = s.begin(); it != s.end(); ++it)
            {
                std::shared_ptr<SlotImpl> slotImpl = it->lock();
                if (slotImpl)
                {
                    std::shared_ptr<void> guard;
                    if (slotImpl->tied_)
                    {
                        guard = slotImpl->tie_.lock();
                        if (guard)
                        {
                            slotImpl->cb_(args...);
                        }
                    }
                    else
                    {
                        slotImpl->cb_(args...);
                    }
                }
            }
        }

    private:
        void add(const std::shared_ptr<SlotImpl>& slot)
        {
            SignalImpl& impl(*impl_);
            {
                std::unique_lock<std::mutex> lock(impl_->mutex_);
                impl.copyOnWrite();
                impl.slots_->push_back(slot);
            }
        }

        const std::shared_ptr<SignalImpl> impl_;
    };

}

#endif
