#include "DIContainer.h"
#include <stack>
//#include "DIContainer.h"
//
//template <class T, class Dependent>
//T* injector::get_instance() const {
//    auto it = instance_map_.find<T>();
//    if (it == instance_map_.end()) {
//        throw std::runtime_error(
//            std::string(typeid(T).name()) + ": unsatisfied dependency of " + std::string(typeid(Dependent).name()));
//    }
//    return static_cast<T*>(it->second->get());
//}
//
//

injector di_config::build_injector()
{
    injector inj;
    std::stack<initializer_fn*> initializers;

    std::unordered_set<int> unmarked_nodes;
    for (auto& node : graph_) {
        node.second.mark_ = node_info::mark::UNMARKED;
        unmarked_nodes.insert(node.first);
    }
    while (!unmarked_nodes.empty()) {
        int node_id = *unmarked_nodes.begin();
        toposort_visit_node(node_id, &unmarked_nodes, &initializers);
    }

    while (!initializers.empty()) {
        (*initializers.top())(inj);
        initializers.pop();
    }
    return std::move(inj);
}

void di_config::toposort_visit_node(int node_id,
    std::unordered_set<int>* unmarked_nodes,
    std::stack <di_config::initializer_fn*>* output)
{
    node_info& info = graph_[node_id];
    if (info.mark_ == node_info::mark::TEMP) {
        throw std::runtime_error(info.debug_type_name_ + " appears to be part of a cycle");
    }
    else if (info.mark_ == node_info::mark::UNMARKED) {
        unmarked_nodes->erase(node_id);
        info.mark_ = node_info::mark::TEMP;
        for (int dependent : info.dependents_) {
            toposort_visit_node(dependent, unmarked_nodes, output);
        }
        info.mark_ = node_info::mark::PERM;
        if (info.has_iniliatizer_) {
            // if has_initializer_ is false, it means someone depends on this
            // node, but an instance factory for this node has not been provided.
            // This will result in an injection error later.
            output->push(&info.initializer_);
        }
    }
}

