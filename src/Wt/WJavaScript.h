// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2008 Emweb bv, Herent, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef WJAVASCRIPT_H_
#define WJAVASCRIPT_H_

#include <Wt/WSignal.h>
#include <Wt/WEvent.h>
#include <Wt/WException.h>
#include <Wt/WJavaScriptSlot.h>
#include <Wt/WLogger.h>
#include <Wt/WString.h>

namespace Wt {

/*! \class JSignal Wt/WJavaScript.h Wt/WJavaScript.h
 *  \brief A signal to relay JavaScript to C++ calls.
 *
 * A JSignal, like an EventSignal, provides communicates events from
 * JavaScript to C++ code. However, it not tied to a built-in
 * event. Instead, it can be emitted from within custom JavaScript
 * code using the JavaScript Wt.emit() function.
 *
 * The signal is identified by a unique name within the scope of a
 * WObject, or a unique global name (when declaring the signal in your
 * WApplication).
 * 
 * The signal supports up to 6 arguments. Values for these arguments
 * may be specified in the JavaScript Wt.emit().
 *
 * Example code:
 * \code
 * class MyWidget : public WCompositeWidget
 * {
 * public:
 *   MyWidget()
 *     : doSome_(this, "doSome")
 *   {
 *     ...
 *   }
 *
 *   JSignal<std::string, int>& doSome() { return doSome_; }
 *
 * private:
 *   JSignal<std::string, int> doSome_;
 *
 *   ...
 * };
 *
 * \endcode
 *
 * The following JavaScript statement will emit the signal for a DOM
 * element <i>element</i> that corresponds to a widget of class
 * <tt>MyWidget</tt>:
 *
 * \code
 * Wt.emit(element, 'dosome', 'foo', 42);
 * \endcode
 *
 * The <i>element</i> can be a \link Wt::WWidget::jsRef() DOM
 * element\endlink, or the \link Wt::WObject::id() object ID\endlink of a
 * WObject, or the constant <tt>Wt</tt> which refers to the Wt::WApplication
 * instance. The conversion between the
 * JavaScript arguments (ax) and the C++ type Ax uses stream operators.
 *
 * You can use the methods createCall() to let the signal itself
 * generate this JavaScript call for you:
 * \code
 * doSome_.createCall("'foo'", "42");
 * \endcode
 *
 * The JavaScript generated by createCall() is possibly affected by every
 * connect or disconnect to the signal. In practice, you will use JSignal
 * internally within a widget and call createCall() only after you connected
 * internal slots to signal.
 *
 * It is also possible to propagate an original JavaScript event as a
 * last argument, of type WMouseEvent, WKeyEvent or WTouchEvent. In that case, the
 * second argument in <tt>Wt.emit()</tt> must be an object which
 * indicates also the JavaScript event and event target.
 *
 * Consider a signal declaration:
 * \code
 * JSignal<std::string, int, WMouseEvent> doSome();
 * \endcode
 *
 * Then, the following would be a suitable JavaScript call:
 *
 * \code
 * Wt.emit(Wt, {name: 'dosome', event: event, eventObject: object}, 'foo', 42);
 * \endcode
 *
 * The method createEventCall() may be used this variation for the JavaScript
 * method call.
 *
 * Since the conversion from JavaScript to C++ uses stream operators,
 * you may provide support for custom types by implementing the c++
 * input stream operator <tt>std::istream& operator>> (std::istream&,
 * T& t)</tt> for your type.
 *
 * \sa WWidget::jsRef(), WObject::id()
 *
 * \ingroup signalslot
 */
#ifndef WT_CNOR
template <typename... A>
#else // WT_CNOR
template <typename A1 = NoClass, typename A2 = NoClass,
	  typename A3 = NoClass, typename A4 = NoClass,
	  typename A5 = NoClass, typename A6 = NoClass>
#endif // WT_CNOR
class JSignal : public EventSignalBase
{
public:
  /*! \brief Construct a signal for the given object, and name.
   *
   * The given \p name must be unique for all user signals
   * specified for the object \p object. Ownership of the signal
   * is not transferred to the object.
   *
   * If \p collectSlotJavaScript is \c true, then javascript specified
   * for connected slots (using JSlot) or learned by stateless slot
   * learning, is collected to client-side JavaScript.
   *
   * Use the utility methods createCall() or createEventCall() to
   * create the appropriate JavaScript statements that invoke the
   * signal, which take into account possible other client-side
   * JavaScript handling associated with the signal.
   *
   * \sa \link WObject::implementStateless() stateless slot learning\endlink
   */
  JSignal(WObject *object, const std::string& name,
	  bool collectSlotJavaScript = false);

  /*! \brief Destructor.
   */
  ~JSignal();

  /*! \brief Returns the signal name.
   */
  const std::string& name() const { return name_; }

  virtual const std::string encodeCmd() const override;

  /*! \brief Returns a JavaScript call that triggers the signal.
   *
   * This is:
   * \code
     Wt.emit([element], [name], arg1, ...);
   * \endcode
   *
   * When the signal was constructed with \p collectSlotJavaScript ==
   * \c true, the inline JavaScript from slots defined as JavaScript
   * or from learned stateless slots, or directly connected to the
   * signal, is included as well.
   *
   * \note The method only takes into account JavaScript from slot
   *       connections that have been connected so far, and any
   *       subsequent connections are ignored.
   *
   * \sa createEventCall()
   */
#ifndef WT_TARGET_JAVA
  const std::string createCall(std::initializer_list<std::string> args) const;
#else // WT_TARGET_JAVA
  const std::string createCall(const std::string& arg1 = std::string(),
			       const std::string& arg2 = std::string(),
			       const std::string& arg3 = std::string(),
			       const std::string& arg4 = std::string(),
			       const std::string& arg5 = std::string(),
			       const std::string& arg6 = std::string()) const;
#endif // WT_TARGET_JAVA

  /*! \brief Returns a JavaScript call that triggers the signal, passing
   *         the original event too.
   *
   * Similar to createCall(), the following JavaScript is returned:
   * \code
     Wt.emit([element], { name: [name], eventObject: [jsObject], event: [jsEvent]},
             arg1, ...);
   * \endcode
   *
   * In addition to information identifying the signal (\p element
   * and \p name) and the arguments, also information on the original
   * JavaScript event is transferred. In this way, you can propagate the
   * corresponding event class (WMouseEvent, WKeyEvent or WTouchEvent) as an additional
   * last argument in the slot.
   *
   * \note The method only takes into account JavaScript from slot
   *       connections that have been connected so far, and any
   *       subsequent connections are ignored.
   *
   */
#ifndef WT_TARGET_JAVA
  const std::string createEventCall(const std::string& jsObject,
				    const std::string& jsEvent,
				    std::initializer_list<std::string> args)
    const;
#else // WT_TARGET_JAVA
  const std::string createEventCall(const std::string& jsObject,
				    const std::string& jsEvent,
				    const std::string& arg1 = std::string(),
				    const std::string& arg2 = std::string(),
				    const std::string& arg3 = std::string(),
				    const std::string& arg4 = std::string(),
				    const std::string& arg5 = std::string())
    const;
#endif // WT_TARGET_JAVA

  /*! \brief Returns whether the signal is connected to at least one slot.
   */
  virtual bool isConnected() const override;

  /*! \brief Connect to a function.
   *
   * This variant of the overloaded connect() method supports a
   * template function object (which supports operator ()).
   *
   * When the receiver function is an object method, the signal will
   * automatically be disconnected when the object is deleted, as long as the
   * object inherits from WObject (or Wt::Signals::trackable).
   */
  template<class F> Wt::Signals::connection connect(F function);
  template<class F> Wt::Signals::connection connect(const WObject *target,
						    F function);

  /*! \brief Connect a slot that takes no arguments.
   */
#ifndef WT_CNOR
  template<class T, class V, class... B>
    Wt::Signals::connection connect(T *target, void (V::*method)(B...));
#else // WT_CNOR
  template<class T, class V>
    Wt::Signals::connection connect(T *target, void (V::*method)());
  template<class T, class V>
    Wt::Signals::connection connect(T *target, void (V::*method)(A1));

  template<class T, class V>
    Wt::Signals::connection connect(T *target, void (V::*method)(const A1&));
  template<class T, class V>
    Wt::Signals::connection connect(T *target, void (V::*method)(A1, A2));
  template<class T, class V>
    Wt::Signals::connection connect(T *target,
				       void (V::*method)(A1,A2,A3));
  template<class T, class V>
    Wt::Signals::connection connect(T *target,
				       void (V::*method)(A1,A2,A3,A4));
  template<class T, class V>
    Wt::Signals::connection connect(T *target,
				       void (V::*method)(A1,A2,A3,A4,A5));
  template<class T, class V>
    Wt::Signals::connection connect(T *target,
				       void (V::*method)(A1,A2,A3,A4,A5,A6));
#endif // WT_CNOR

  /*! \brief Connects a JavaScript function.
   *
   * This will provide a client-side connection between the event and
   * a JavaScript function. The argument must be a JavaScript function
   * which optionally accepts up to two arguments (object and event):
   *
   * \code
   * function(object, event) {
   *   ...
   * }
   * \endcode
   *
   * Unlike a JSlot, there is no automatic connection management: the
   * connection cannot be removed. If you need automatic connection
   * management, you should use connect(JSlot&) instead.
   */
  void connect(const std::string& function);
  void connect(const char * function);

  /*! \brief Connect a slot that is specified as JavaScript only.
   *
   * This will provide a client-side connection between the event and
   * some JavaScript code as implemented by the slot. Unlike other
   * connects, this does not cause the event to propagated to the
   * application, and thus the state changes induced by the
   * \p slot are invisible to the server-side.
   */
  void connect(JSlot& slot);

  /*! \brief Emit the signal.
   *
   * The arguments must exactly match the arguments of the target
   * function.
   *
   * This will cause all connected slots to be triggered, with the given
   * arguments.
   */
#ifndef WT_CNOR
  void emit(A... args) const;
#else // WT_CNOR
  void emit(A1 a1 = NoClass::none, A2 a2 = NoClass::none,
	    A3 a3 = NoClass::none, A4 a4 = NoClass::none,
	    A5 a5 = NoClass::none, A6 a6 = NoClass::none);
#endif // WT_CNOR

  /*! \brief Emit the signal.
   *
   * This is equivalent to emit().
   *
   * \sa emit
   */
#ifndef WT_CNOR
  void operator()(A... args) const;
#else // WT_CNOR
  void operator()(A1 a1 = NoClass::none, A2 a2 = NoClass::none,
		  A3 a3 = NoClass::none, A4 a4 = NoClass::none,
		  A5 a5 = NoClass::none, A6 a6 = NoClass::none);
#endif // WT_CNOR

  virtual Wt::Signals::connection connect(WObject *target,
					  void (WObject::*method)()) override;

protected:
  virtual int argumentCount() const override;

private:
#ifndef WT_CNOR
  typedef Signals::Signal<A...> SignalType;
#endif // WT_CNOR

  std::string name_;
#ifndef WT_CNOR
  SignalType impl_;
#endif // WT_CNOR

  virtual void processDynamic(const JavaScriptEvent& e) const override;
};

#ifdef WT_CNOR

class WT_API JSignal0 : public JSignal<NoClass>
{
public:
  JSignal0(WObject *object, const std::string& name,
	   bool collectSlotJavaScript = false);

  template<class T, class V>
    Wt::Signals::connection connect(T *target, void (V::*method)());
  template<class F> Wt::Signals::connection connect(F f);
  template<class F> Wt::Signals::connection connect(const WObject *target,
						    F function);
  
  void connect(const std::string& function);
  void connect(const char * function);

  void connect(JSlot& slot);

  void emit() const;
};

#endif // WT_CNOR

#ifndef WT_CNOR

void WT_API addSignalToWidget(WObject* o, EventSignalBase* signal);
std::string WT_API senderId(WObject *sender);

template <typename... A>
JSignal<A...>::JSignal(WObject *object,
		       const std::string& name,
		       bool collectSlotJavaScript)
  : EventSignalBase(nullptr, object, collectSlotJavaScript),
    name_(name)
{
  addSignalToWidget(object, dynamic_cast<EventSignalBase*>(this));
}

template <typename... A>
JSignal<A...>::~JSignal()
{
  prepareDestruct();
}

template <typename... A>
const std::string JSignal<A...>::encodeCmd() const
{
  return senderId(owner()) + "." + name_;
}

template <typename... A>
const std::string JSignal<A...>
::createCall(std::initializer_list<std::string> args) const
{
  return EventSignalBase::createUserEventCall(std::string(), std::string(),
					      name_, args);
}

template <typename... A>
const std::string JSignal<A...>
::createEventCall(const std::string& jsObject, const std::string& jsEvent,
		  std::initializer_list<std::string> args) const
{
  return EventSignalBase::createUserEventCall(jsObject, jsEvent,
					      name_, args);
}

template <typename... A>
template <class F>
Wt::Signals::connection JSignal<A...>::connect(F function)
{
  exposeSignal();
  return Signals::Impl::connectFunction<F, A...>(impl_, std::move(function), nullptr);
}

template <typename... A>
template <class F>
Wt::Signals::connection JSignal<A...>::connect(const WObject *target,
					       F function)
{
  exposeSignal();
  return Signals::Impl::connectFunction<F, A...>(impl_, std::move(function), target);
}

template <class... A>
template <class T, class V, class... B>
Wt::Signals::connection JSignal<A...>
::connect(T *target, void (V::*method)(B...))
{
  exposeSignal();

  return Signals::Impl::ConnectHelper<sizeof...(B), A...>
    ::connect(impl_, target, method);
}

/* Oops how to do that?
template <typename A1, typename A2, typename A3,
	  typename A4, typename A5, typename A6>
template <class T, class V>
Wt::Signals::connection JSignal<A1, A2, A3, A4, A5, A6>
::connect(T *target, void (V::*method)())
{
  exposeSignal();
  WObject *o = dynamic_cast<WObject *>(dynamic_cast<V *>(target));
  assert(o);

  WStatelessSlot *s = o->isStateless(static_cast<WObject::Method>(method));

  if (canAutoLearn() && s)
    return EventSignalBase::connectStateless
      (static_cast<WObject::Method>(method), o, s);
  else
    return impl_.connect(std::bind(method, target), target);
}
*/

template <typename... A>
void JSignal<A...>::connect(const std::string& function)
{
  if (!canAutoLearn()) {
    Wt::log("error") << "JSignal: connect(const std::string&): signal does "
      "not collect JavaScript from slots";
    return;
  }

  EventSignalBase::connect(function);
}

template <typename... A>
void JSignal<A...>::connect(const char *function)
{
  if (!canAutoLearn()) {
    Wt::log("error") << "JSignal: connect(const std::string&): signal does "
      "not collect JavaScript from slots";
    return;
  }

  EventSignalBase::connect(function);
}

template <typename... A>
void JSignal<A...>::connect(JSlot& slot)
{
  if (!canAutoLearn()) {
    Wt::log("error") << "JSignal: connect(JSlot): signal does not collect "
      "JavaScript from slots";
    return;
  }

  EventSignalBase::connect(slot);
}

template <typename... A>
Wt::Signals::connection JSignal<A...>::connect(WObject *target,
					       void (WObject::*method)())
{
  exposeSignal();
  WStatelessSlot *s = target->isStateless(method);
  if (canAutoLearn() && s)
    return EventSignalBase::connectStateless(method, target, s);
  else
    return impl_.connect(std::bind(method, target), target);
}

template <typename... A>
void JSignal<A...>::emit(A... args) const
{
  impl_.emit(args...);
}

template <typename... A>
void JSignal<A...>::operator()(A... args) const
{
  emit(args...);
}

template <typename... A>
bool JSignal<A...>::isConnected() const
{
  return impl_.isConnected() || EventSignalBase::isConnected();
}

template <typename... A>
int JSignal<A...>::argumentCount() const
{
  return sizeof...(A);
}

namespace Impl {

/*
 * Unmarshalling a single JSignal argument from JavaScript
 */
template <typename T>
void unMarshal(const JavaScriptEvent& jse, int argi, T& t) {
  if ((unsigned)argi >= jse.userEventArgs.size()) {
    Wt::log("error") << "JSignal: missing JavaScript argument:" << argi;
    return;
  }

  std::string v = jse.userEventArgs[argi];
  WString::checkUTF8Encoding(v);

  std::stringstream ss;
  ss.str(v);
  ss >> t;
  if (ss.fail())
    Wt::log("error") << "JSignal: bad argument format: '"
                     << jse.userEventArgs[argi] << "' for C++ type '"
                     << typeid(T).name() << "'";
}

extern WT_API void unMarshal(const JavaScriptEvent &jse, int argi, std::string& s);
extern WT_API void unMarshal(const JavaScriptEvent& jse, int argi, WString& s);
extern WT_API void unMarshal(const JavaScriptEvent& jse, int argi, NoClass& nc);
extern WT_API void unMarshal(const JavaScriptEvent& jse, int, WMouseEvent& e);
extern WT_API void unMarshal(const JavaScriptEvent& jse, int, WKeyEvent& e);
extern WT_API void unMarshal(const JavaScriptEvent& jse, int, WTouchEvent& e);

/*
 * Recursive functions to unmarshal all arguments to a tuple
 */
template<std::size_t I = 0, typename... A>
inline typename std::enable_if<I == sizeof...(A), void>::type
unMarshal(const JavaScriptEvent& /*jse*/, std::tuple<A...>& /*t*/)
{ }

template<std::size_t I = 0, typename... A>
inline typename std::enable_if<I < sizeof...(A), void>::type
unMarshal(const JavaScriptEvent& jse, std::tuple<A...>& t)
{
  unMarshal(jse, I, std::get<I>(t));
  unMarshal<I + 1, A...>(jse, t);
}

/*
 * Template magick to generate an integer sequence
 */
template<int ...> struct seq {};
template<int N, int ...S> struct gens : gens<N-1, N-1, S...> {};
template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };

/*
 * Stores a template-pack tuple with an indirect dispatch to JSignal::emit()
 */
template <typename... A>
struct Holder {
  std::tuple<A...> args;
  const JSignal<A...>& signal;

  Holder(const JSignal<A...>& s) : signal(s) { }

  void dispatch() {
    doEmit(typename gens<sizeof...(A)>::type());
  }

  template<int ...S>
  void doEmit(seq<S...>) {
    signal.emit(std::get<S>(args)...);
  }
};

}

template <typename... A>
void JSignal<A...>::processDynamic(const JavaScriptEvent& jse) const
{
  Impl::Holder<A...> h(*this);
  Impl::unMarshal(jse, h.args);
  h.dispatch();
}

#endif // WT_CNOR

}

#endif // WUSER_SIGNAL_H_
