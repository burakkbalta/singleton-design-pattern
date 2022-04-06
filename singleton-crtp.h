#ifndef SINGLETON_CRTP_H
#define SINGLETON_CRTP_H

#include <type_traits>
#include <cassert>

namespace bb {
namespace singleton {
namespace singleton_crtp {

/**
 * This interface should be inherited by derived class which has main constructor that
 * want to be called first when we create singleton class from derived class. 
 * Checking for main constructor call is done according to below parameter.
 */
struct IMainCtor {
	bool IsMainCtorCalled{false};
};

/**
 * Traits which can be used to check whether type T has a variable named IsMainCtorCalled.
 * If it has it inherits true_type, otherwise false_type.
 */
template <typename T, typename = decltype(std::declval<IMainCtor>().IsMainCtorCalled)>
struct has_MainCtor : std::false_type {};

template <typename T>
struct has_MainCtor<T, decltype(std::declval<T>().IsMainCtorCalled)> : std::true_type {};

/**
 * 
 * @param t Pointer of type T 
 * @return True or False. If type T has main constructor which is not called first, 
 *		it returns false, otherwise true.
 */
template<typename T>
inline typename std::enable_if<has_MainCtor<T>::value, bool>::type
CheckIsMainCtorCalled(T* t) {
	return t->IsMainCtorCalled;
}
/**
 * 
 * @param ...If template specialization does not occur, this function is called.
 * @return true.
 */
inline bool CheckIsMainCtorCalled(...) {
	return true;
}

/**
 * Utility class that is used to provide uniqueness from any class that is given
 * as template parameter T. 
 * Main idea is to guarantee Singleton Pattern for class T using 
 * curiously recurring template pattern(CRTP). 
 * This class should be inherited publicly.
 */
template <class T>
class SingletonCRTP {
public:
	/**
	 * Template parameter is purified. It remains just a type for a class.
	 */
	using Type = typename std::remove_reference<typename std::remove_cv<typename std::remove_all_extents<T>::type>::type>::type;
	/**
	 * Static assertion occurs if the template parameter is not a class.
	 */
	static_assert(std::is_class<Type>::value, "Template parameter must be class.");
	
	/**
	 * Create singleton instance with Type T. 
	 * @param args Number of arguments to call constructor function of singleton class from a class T. 
	 * @return Reference of Type T.
	 */
	template <typename... Args>
	static Type& CreateSingleton(Args... args) {		
		if(!instance) {
			instance = new Type(std::forward<Args>(args)...);
		}
		assert(CheckIsMainCtorCalled(instance));
		return *instance;
	}
	
	/**
	 * Get singleton instance of type T. 
	 * @return static instance of type T. 
	 */
	static Type* GetInstance() {
		assert(("Function of --CreateSingleton-- already should be called.", instance != nullptr));
		return instance;
	}
	
	/**
	 * -> Operator overloading. 
	 * @return Pointer to derived class that is singleton.
	 */
	Type* operator ->() {
		return &**this;
	}
	
	/**
	 * -> Operator overloading. 
	 * @return Constant pointer to derived class that is singleton.
	 */
	const Type* operator ->() const {
		return &**this;
	}
	
	/**
	 * * Operator overloading. 
	 * @return Reference to derived class that is singleton.
	 */
	Type& operator *() {
		return *instance;
	}
	
	/**
	 * * Operator overloading. 
	 * @return Constant reference to derived class that is singleton.
	 */
	const Type& operator *() const {
		return *instance;
	}
	
	SingletonCRTP(const SingletonCRTP&) = delete;
	SingletonCRTP& operator=(const SingletonCRTP&) = delete;
	SingletonCRTP(SingletonCRTP&&) = delete;
	SingletonCRTP& operator=(SingletonCRTP&&) = delete;
	SingletonCRTP(SingletonCRTP& ) = delete;
	SingletonCRTP& operator=(SingletonCRTP&) = delete;
	
protected:
	/**
	 * Constructor that is used by derived class. 
	 */
	SingletonCRTP() {
		instance = static_cast<Type*>(this);
	}
	
	/**
	 * Destructor that is used by derived class. 
	 */
	virtual ~SingletonCRTP() {
		delete instance;
		instance = nullptr;
	}
	
private:
	static Type* instance;
};

template <class T>
typename SingletonCRTP<T>::Type* SingletonCRTP<T>::instance = nullptr;

}
}
}

#endif /* SINGLETON_CRTP_H */

