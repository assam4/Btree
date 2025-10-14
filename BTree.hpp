#pragma once

#include <deque>
#include <algorithm>
#include <iostream>
#include "Wrapper.hpp"

namespace Btree {
	template <typename T, size_t N = 3>
	class   Node final {
		public:
			/**
				@brief Variable of failure
				@details used as a return value when the find() function fails
			*/
			inline static  WrapperType<T>	nres;

			/**
				@brief ctors, dtor, assignments
				@details functions for manage(create,move and destroy) Node<T> objects
				@details this class is not copyable (Can be changed in newest versions)
			*/
			Node()	= default;
			~Node()	= default;
			Node(const Node&)	= delete;
			Node<T>&    operator=(const Node<T>&)	= delete;

			Node(WrapperType<T>&& val, Node<T>* prev = nullptr): m_prev(prev) {
				m_values.push_back(std::move(val));
			}
			Node(Node<T>* parent): m_prev(parent) {
			} 

			Node(Node<T>&& oth)
					: m_values(std::move(oth.m_values))
					, m_left(std::move(oth.m_left))
					, m_middle(std::move(oth.m_middle))
					, m_right(std::move(oth.m_right))
					, m_helper(std::move(oth.m_helper))
					, m_prev(std::move(oth.m_prev)) {
			}

			Node<T>&    operator=(Node<T>&& oth) {
				if (this != &oth) {
					m_values = std::move(oth.m_values);
					m_left = std::move(oth.m_left);
					m_middle = std::move(oth.m_middle);
					m_right = std::move(oth.m_right);
					m_helper = std::move(oth.m_helper);
					m_prev = std::move(oth.m_prev);
				}
				return *this;
			}

			/**
				@brief User functionality
				@details With the help of these functions, the user can interact with the tree.
				 * push() - insert new element into tree
				 * find() - search in tree by element(if tree doesn't contain element return value is a nres(Wrapper<T>) object)
				 * clear() - recursive data cleaner  
				 * empty() - returns true if tree doesn't contain element
				 * size() - each node returns his hierarchical size of elements (root returns all)
			*/

			void push(const T& elem) {
				if (hasNodes())  
					searchInsertLocation(elem);   
				else
					insertIntoNode(elem);
			}

			const WrapperType<T>&	find(const T& key) const noexcept {
				for (auto& x: m_values)
						if (x == key)
							return x;
				if (hasNodes()) {
					auto it = m_values.begin();
					if (*it > key)
						return m_left->find(key);
					else if (m_middle && ++it != m_values.end() && it->hasCustomValue() && *it > key)
						return m_middle->find(key);
					else
						return m_right->find(key);
				}
				return nres;
			}

			void	clear() {
				if (m_left) {
					m_left->clear(); m_left.reset();
				}
				if (m_middle) {
					m_middle->clear(); m_middle.reset();
				}
				if (m_right) {
					m_right->clear(); m_right.reset();
				}
				if (m_helper) {
					m_helper->clear(); m_helper.reset();
				}
				m_values.clear();
			}

			void print(std::ostream& os = std::cout) const {
				if (m_left) m_left->print(os);
        		if (m_middle) m_middle->print(os);
				if (m_right) m_right->print(os);
				os << "[ ";
				for (const auto& it: m_values)
					os << "\"" << it << "\" ";
				os << "]";
				if (hasNodes())
					os << "   -- Node --";
				std::cout << std::endl;
			}

			bool	empty() const noexcept(noexcept(m_values.empty())) {
				return m_values.empty();
			}

			size_t	size() const noexcept { 
				size_t	size {m_values.size()};
				if (size) {
					if (m_left)		size += m_left->size();
					if (m_middle)	size += m_middle->size();
					if (m_right)	size += m_right->size();
				} 
				return size;
			}

			bool	contains(const T& key) const noexcept {
				return (find(key) != Node<T>::nres);
			}
		private:

			bool	hasNodes() const noexcept {
				return (static_cast<bool>(m_left) || static_cast<bool>(m_right)
						|| static_cast<bool>(m_middle) || static_cast<bool>(m_helper));
			}

			bool	full() const noexcept(noexcept(m_values.size() == N)) {
				return (m_values.size() == N);
			}

			void	searchInsertLocation(const T& elem) {
				auto it = m_values.begin();
				if (*it > elem)
					m_left->push(elem);
				else if (m_middle && ++it != m_values.end() && it->hasCustomValue() && *it > elem)
					m_middle->push(elem);
				else
					m_right->push(elem);
			}

			void	insertIntoNode(const T& elem) {
				m_values.push_back(WrapperType<T>(elem));
				std::sort(m_values.begin(), m_values.end());
				if (full()) 
					reConstruction();
			}

			void	reConstruction() {
				if (!m_prev)
					division();
				else if (m_prev->m_values.size() == 1)
					m_prev->createMiddle();
				else
					m_prev->reCompute();
			}

			void	division() {
				auto temp_l = std::make_unique<Node<T> >(std::move(m_values.front()), this);
				m_values.pop_front();
				auto temp_r = std::make_unique<Node<T> >(std::move(m_values.back()), this);
				m_values.pop_back();
				if (hasNodes()) {
					temp_l->m_left = std::move(m_left); temp_l->m_left->m_prev = temp_l.get();
					temp_l->m_right = std::move(m_middle); temp_l->m_right->m_prev = temp_l.get();
					temp_r->m_left = std::move(m_right); temp_r->m_left->m_prev = temp_r.get();
					temp_r->m_right = std::move(m_helper); temp_r->m_right->m_prev = temp_r.get();
				}
				m_left = std::move(temp_l);
				m_right = std::move(temp_r);
			}

			void	createMiddle() {
				if (m_right->full()) {
					m_middle = std::make_unique<Node<T> >(std::move(m_right->m_values.front()), this);
					m_right->m_values.pop_front();
					m_values.push_back(std::move(m_right->m_values.front()));
					m_right->m_values.pop_front();
					if (m_right->hasNodes()) {
						m_middle->m_left = std::exchange(m_right->m_left, std::move(m_right->m_right));
						m_middle->m_left->m_prev = m_middle.get();
						m_middle->m_right = std::move(m_right->m_middle);
						m_middle->m_right->m_prev = m_middle.get();
						m_right->m_right = std::move(m_right->m_helper);
					}
				}
				else if (m_left->full()) {
					m_middle = std::make_unique<Node<T> >(std::move(m_left->m_values.back()), this);
					m_left->m_values.pop_back();
					m_values.push_front(std::move(m_left->m_values.back()));
					m_left->m_values.pop_back();
					if (m_left->hasNodes()) {
						m_middle->m_left = std::exchange(m_left->m_right, std::move(m_left->m_middle));
						m_middle->m_left->m_prev = m_middle.get();
						m_middle->m_right = std::move(m_left->m_helper);
						m_middle->m_right->m_prev = m_middle.get();
					}
				}
			}

			void	reCompute() {
				if (m_left->full()) {
					m_helper = std::make_unique<Node<T> >(std::move(m_left->m_values.back()), this);	
					m_left->m_values.pop_back();                                          													  				
					std::swap(m_middle, m_helper);										 				
					std::swap(m_right, m_helper);														
					if (m_left->hasNodes()) {
						m_middle->m_left = std::exchange(m_left->m_right, std::move(m_left->m_middle));
						m_middle->m_left->m_prev = m_middle.get();
						m_middle->m_right = std::move(m_left->m_helper);
						m_middle->m_right->m_prev = m_middle.get();
					}
					auto temp = std::move(m_left->m_values.back());
					m_left->m_values.pop_back();
					insertIntoNode(static_cast<T>(temp));
				}
				else if (m_right->full()) {
					m_helper = std::make_unique<Node<T> >(std::move(m_right->m_values.back()), this);
					m_right->m_values.pop_back();
					if (m_right->hasNodes()) {
						m_helper->m_left = std::move(m_right->m_right); m_helper->m_left->m_prev = m_helper.get();
						m_helper->m_right = std::move(m_right->m_helper); m_helper->m_right->m_prev = m_helper.get();
						m_right->m_right = std::move(m_right->m_middle);
					}
					auto temp = std::move(m_right->m_values.back());
					m_right->m_values.pop_back();
					insertIntoNode(static_cast<T>(temp));
				}
				else {
					m_helper = std::make_unique<Node<T> >(std::move(m_middle->m_values.back()), this);
					m_middle->m_values.pop_back();
					std::swap(m_right, m_helper);
					if(m_middle->hasNodes()) {
						m_right->m_left = std::move(m_middle->m_right);
						m_right->m_left->m_prev = m_right.get();
						m_right->m_right = std::move(m_middle->m_helper);
						m_right->m_right->m_prev = m_right.get();
						m_middle->m_right = std::move(m_middle->m_middle);
					}
					auto temp = std::move(m_middle->m_values.back());
					m_middle->m_values.pop_back();
					insertIntoNode(static_cast<T>(temp));
				}
			}

		private:
			std::deque<WrapperType<T> >		m_values;
			std::unique_ptr<Node<T> >       m_left;
			std::unique_ptr<Node<T> >       m_middle;
			std::unique_ptr<Node<T> >       m_right;
			std::unique_ptr<Node<T> >       m_helper;
			Node<T>*                        m_prev{};
	};
}