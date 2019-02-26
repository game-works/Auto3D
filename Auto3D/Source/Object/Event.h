#pragma once

#include "../Base/AutoPtr.h"
#include "../Base/Ptr.h"
#include "../IO/JSONValue.h"

namespace Auto3D
{

class Event;

/// Internal helper class for invoking _event handler functions.
class AUTO_API EventHandler
{
public:
    /// Construct with receiver object pointer.
    EventHandler(RefCounted* receiver);
    /// Destruct.
    virtual ~EventHandler();

    /// Invoke the handler function. Implemented by subclasses.
    virtual void Invoke(Event& event) = 0;

    /// Return the receiver object.
    RefCounted* Receiver() const { return _receiver.Get(); }

protected:
    /// Receiver object.
    WeakPtr<RefCounted> _receiver;
};

/// Template implementation of the _event handler invoke helper, stores a function pointer of specific class.
template <class _Ty, class U> class EventHandlerImpl : public EventHandler
{
public:
    typedef void (_Ty::*HandlerFunctionPtr)(U&);

    /// Construct with receiver and function pointers.
    EventHandlerImpl(RefCounted* receiver_, HandlerFunctionPtr function_) :
        EventHandler(receiver_),
        _function(function_)
    {
        assert(_function);
    }

    /// Invoke the handler function.
    void Invoke(Event& event) override
    {
        _Ty* typedReceiver = static_cast<_Ty*>(_receiver.Get());
        U& typedEvent = static_cast<U&>(event);
        (typedReceiver->*_function)(typedEvent);
    }

private:
    /// Pointer to the _event handler function.
    HandlerFunctionPtr _function;
};

/// Notification and data passing mechanism, to which objects can subscribe by specifying a handler function. Subclass to include _event-specific data.
class AUTO_API Event
{
public:
    /// Construct.
    Event();
    /// Destruct.
    virtual ~Event();
    
    /// Send the _event.
    void Send(RefCounted* sender);
    /// Subscribe to the _event. The _event takes ownership of the handler data. If there is already handler data for the same receiver, it is overwritten.
    void Subscribe(EventHandler* handler);
    /// Unsubscribe from the _event.
    void Unsubscribe(RefCounted* receiver);

    /// Return whether has at least one _valid receiver.
    bool HasReceivers() const;
    /// Return whether has a specific receiver.
    bool HasReceiver(const RefCounted* receiver) const;
    /// Return current sender.
    RefCounted* Sender() const { return _currentSender; }
    
private:
    /// Prevent copy construction.
    Event(const Event& rhs);
    /// Prevent assignment.
    Event& operator = (const Event& rhs);
    
    /// Event handlers.
    Vector<AutoPtr<EventHandler> > _handlers;
    /// Current sender.
    WeakPtr<RefCounted> _currentSender;
};

}
