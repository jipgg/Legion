#include "gui.h"
#include <algorithm>
#include "engine.h"
#include <stdexcept>
#include <ranges>
using gui::UDim2;
namespace gui {
void Element::set_parent(const Shared& new_parent) {
    new_parent->children_.push_back(shared_from_this());
    if (auto old_parent = parent_.lock()) {
        old_parent->remove_child(this);
    }
    parent_ = new_parent;
}
void Element::render_descendants(const SDL_Rect& parent_rect) const {
    const SDL_Rect my_rect = to_screen_rect(parent_rect);
    for (const auto& child : children_) {
        child->render(my_rect);
        child->render_descendants(my_rect);
    }
}
void Element::adopt_child(Shared&& child) {
    if (auto old_parent = child->parent_.lock()) {
        old_parent->remove_child(child.get());
    }
    children_.emplace_back(child);
    child->parent_ = shared_from_this();
}
void Element::remove_child(Element* child_ptr) {
    auto child_ptr_matches = [&child_ptr](Shared& child) {
        return child.get() == child_ptr;
    };
    auto it = std::ranges::find_if(children_, child_ptr_matches);
    if (it == children_.end()) {
        throw std::out_of_range("child not found.");
        return;
    }
    if (it->get() != children_.back().get()) {
        it->swap(children_.back());
    }
    children_.pop_back();
}
Element::Weak Element::parent() const {
    return parent_;
}
std::span<const Element::Shared> Element::children() const {
    return children_;
}
}
