#pragma once
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stack>

#include <typeinfo>
#include <functional>
#include <memory>
#include <atomic>
#include <string>
#include <stdexcept>

// code from https://gpfault.net/posts/dependency-injection-cpp.txt.html

template <class InstanceType, class Deleter, class ...Deps>
using instance_factory_function =
std::unique_ptr<InstanceType, Deleter>(*)(Deps*...);

namespace detail {
    // Maps types to objects of ValueType
    template <class ValueType>
    class type_map {
        using Container = std::unordered_map<int, ValueType>;
    public:
        using iterator = typename Container::iterator;
        using const_iterator = typename Container::const_iterator;

        iterator begin() { return container_.begin(); }
        iterator end() { return container_.end(); }
        const_iterator begin() const { return container_.begin(); }
        const_iterator end() const { return container_.end(); }
        const_iterator cbegin() const { return container_.cbegin(); }
        const_iterator cend() const { return container_.cend(); }

        template <class Key>
        iterator find() { return container_.find(type_id<Key>()); }
        template <class Key>
        const_iterator find() const { return container_.find(type_id<Key>()); }
        template <class Key>
        void put(ValueType&& value) {
            container_[type_id<Key>()] = std::forward<ValueType>(value);
        }

        template <class Key>
        static int type_id() {
            static int id = ++last_type_id_;
            return id;
        }

    private:
        static std::atomic<int> last_type_id_;
        Container container_;
    };

    template <class T>
    std::atomic<int> type_map<T>::last_type_id_(0);
} // namespace detail


class abstract_instance_container {
public:
    virtual ~abstract_instance_container() = default;

    // The type_map holding the instance container has the
    // type information, so we'll be able to safely convert
    // the pointer from void* back to its original type.
    void* get() { return raw_ptr_; }

    abstract_instance_container(abstract_instance_container&& other) {
        *this = std::move(other);
    }

    abstract_instance_container& operator=(
        abstract_instance_container&& other) {
        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = nullptr;
        return *this;
    }

protected:
    explicit abstract_instance_container(void* raw_ptr) :
        raw_ptr_(raw_ptr) {}

private:
    void* raw_ptr_;
};


template <class T, class Deleter>
class instance_container : public abstract_instance_container {
public:
    ~instance_container() override = default;

    explicit instance_container(std::unique_ptr<T, Deleter>&& p) :
        abstract_instance_container(p.get()),
        pointer_(std::move(p)) {}

    instance_container(instance_container&& other) {
        *this = std::move(other);
    }

    instance_container& operator=(instance_container&& other) {
        pointer_ = std::move(other);
        abstract_instance_container::operator=(std::move(other));
        return *this;
    }

private:
    std::unique_ptr<T, Deleter> pointer_;
};


template <class T, class Deleter>
std::unique_ptr<abstract_instance_container>
wrap_into_instance_container(
    std::unique_ptr<T, Deleter>&& ptr) {
    return std::make_unique<instance_container<T, Deleter>>(
        std::move(ptr));
}



class injector {
    friend class di_config;
public:
    injector(injector&& other) { *this = std::move(other); }
    injector& operator=(injector&& other) { instance_map_ = std::move(other.instance_map_); return *this; }

    template <class T, class Dependent = std::nullptr_t>
    T* get_instance() const;

    template <class InstanceType, class Deleter, class ...Deps>
    std::unique_ptr<InstanceType, Deleter> inject(
        instance_factory_function<InstanceType, Deleter, Deps...> instance_factory) const;

    injector() = default; // JIM: Made ctor public
private:
    
    using instance_map =
        detail::type_map<std::unique_ptr<abstract_instance_container>>;
    instance_map instance_map_;
};


class di_config {
public:
    template <class InstanceType, class Deleter, class ...Deps>
    void add(
        instance_factory_function<InstanceType, Deleter, Deps...>
        instance_factory);
    injector build_injector();
private:
    using initializer_fn = std::function<void(injector&)>;
    struct node_info {
        // Mark is needed for the toposort algorithm.
        enum class mark {
            UNMARKED, TEMP, PERM
        };
        mark mark_;

        // Name of the service class, 
        // needed to display more useful error messages
        std::string debug_type_name_;

        // A function that invokes the instance factory
        // and adds the created service to the given
        // injector.
        initializer_fn initializer_;
        bool has_iniliatizer_ = false;

        // List of nodes adjacent to this one.
        std::unordered_set<int> dependents_;        
    };

    std::unordered_map<int, node_info> graph_;

private:
    void toposort_visit_node(
        int node_id,
        std::unordered_set<int>* unmarked_nodes,
        std::stack <di_config::initializer_fn*>* output);
};


template <class T, class Dependent>
T* injector::get_instance() const {
    auto it = instance_map_.find<T>();
    if (it == instance_map_.end()) {
        std::string dependencyName = std::string(typeid(T).name());
        std::string dependentName = std::string(typeid(Dependent).name());
        throw std::runtime_error(
            dependencyName + ": unsatisfied dependency of " + dependentName);
    }
    return static_cast<T*>(it->second->get());
}



template <class InstanceType, class Deleter, class ...Deps>
std::unique_ptr<InstanceType, Deleter> injector::inject(
    instance_factory_function<InstanceType, Deleter, Deps...>
    instance_factory) const {
    return
        instance_factory(
            get_instance<typename std::remove_const<Deps>::type,
            typename std::remove_const<InstanceType>::type>()...);
}

template <class ...Args>
inline void passthru(Args... args) {}

template<class InstanceType, class Deleter, class ...Deps>
inline void di_config::add(instance_factory_function<InstanceType, Deleter, Deps...> instance_factory)
{
    int instance_type_id =
        detail::type_map<node_info>::type_id<typename std::remove_const<InstanceType>::type>();
    node_info& new_node_info = graph_[instance_type_id];
    new_node_info.initializer_ =
        [instance_factory](injector& inj) {
        auto instance = wrap_into_instance_container(inj.inject(instance_factory));
        inj.instance_map_.put<InstanceType>(std::move(instance));
    };
    new_node_info.has_iniliatizer_ = true;
    new_node_info.debug_type_name_ = typeid(typename std::remove_const<InstanceType>::type).name();
    passthru(
        graph_[detail::type_map<node_info>::type_id<typename std::remove_const<Deps>::type>()]
        .dependents_
        .insert(instance_type_id)...);
}
