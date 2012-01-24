/**
 * @file binary_heap.h
 *
 * The class that implements a binary heap
 *
 * @author Dahua Lin
 */

#ifdef _MSC_VER
#pragma once
#endif

#ifndef BINARY_HEAP_H_
#define BINARY_HEAP_H_

#include <bcslib/base/basic_defs.h>
#include <vector>
#include <functional>


namespace bcs
{

	/**
	 * Note: the node index is from 1 to size.
	 */
	template<typename T>
	class consecutive_binary_tree
	{
	public:
		typedef T value_type;
		typedef std::vector<T> container_type;
		typedef typename container_type::size_type size_type;
		typedef typename container_type::difference_type difference_type;
		typedef typename container_type::reference reference;
		typedef typename container_type::const_reference const_reference;
		typedef typename container_type::pointer pointer;
		typedef typename container_type::const_pointer const_pointer;
		typedef typename container_type::iterator iterator;
		typedef typename container_type::const_iterator const_iterator;

		struct handle
		{
			size_type id;

			handle() : id(0) { }

			handle(size_type id_) : id(id_) { }

			size_type index() const
			{
				return id - 1;
			}

			bool is_nil() const
			{
				return id == 0;
			}

			bool non_nil() const
			{
				return id > 0;
			}

			bool operator == (const handle& rhs) const
			{
				return id == rhs.id;
			}

			bool operator != (const handle& rhs) const
			{
				return id != rhs.id;
			}
		};

	public:
		size_type size() const
		{
			return m_nodes.size();
		}

		bool empty() const
		{
			return m_nodes.empty();
		}

		void reserve(size_type cap)
		{
			m_nodes.reserve(cap);
		}

		void push(const T& e)
		{
			m_nodes.push_back(e);
		}

		void pop()
		{
			m_nodes.pop_back();
		}

	public:
		const_reference root_value() const
		{
			return m_nodes.front();
		}

		reference root_value()
		{
			return m_nodes.front();
		}

		const_reference back_value() const
		{
			return m_nodes.back();
		}

		reference back_value()
		{
			return m_nodes.back();
		}

		const_reference operator() (handle node) const
		{
			return m_nodes[node.index()];
		}

		reference operator() (handle node)
		{
			return m_nodes[node.index()];
		}

		const_iterator begin() const
		{
			return m_nodes.begin();
		}

		const_iterator end() const
		{
			return m_nodes.end();
		}

		iterator begin()
		{
			return m_nodes.begin();
		}

		iterator end()
		{
			return m_nodes.end();
		}

	public:
		handle root() const
		{
			return 1;
		}

		handle back() const
		{
			return size();
		}

		handle last_parent() const
		{
			return size() >> 1;
		}

		handle parent(handle v) const
		{
			return v.id >> 1;
		}

		handle left_child(handle v) const
		{
			size_type id = v.id << 1;
			return id <= size() ? id : 0;
		}

		handle right_child(handle v) const
		{
			size_type id = v.id << 1;
			return id < size() ? id + 1 : 0;
		}

		void get_children(handle v, handle& lc, handle& rc) const
		{
			size_type id = v.id << 1;
			size_type s = size();

			if (id > s)
			{
				lc = 0;
				rc = 0;
			}
			else if (id == s)
			{
				lc = id;
				rc = 0;
			}
			else // id < s
			{
				lc = id;
				rc = id + 1;
			}
		}

		bool is_non_root(handle v) const
		{
			return v.id > 1;
		}

	private:
		container_type m_nodes;
	};



	template<typename T,
		class Container=std::vector<T>,
		class Compare=std::less<T> >
	class binary_heap
	{
	public:
		typedef Container container_type;
		typedef Compare compare_type;

		typedef T value_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef typename container_type::reference reference;
		typedef typename container_type::const_reference const_reference;
		typedef typename container_type::pointer pointer;
		typedef typename container_type::const_pointer const_pointer;
		typedef typename container_type::iterator iterator;
		typedef typename container_type::const_iterator const_iterator;
		typedef typename container_type::allocator_type allocator_type;

		typedef consecutive_binary_tree<size_type> tree_type;
		typedef tree_type::handle node_type;
		typedef std::vector<node_type> node_map_type;

	public:
		binary_heap(const container_type& elements, const compare_type& compare = compare_type())
		: m_elements(elements), m_compare(compare)
		{
			init_node_map();
		}

		binary_heap(const container_type& elements, bool do_makeheap, const compare_type& compare = compare_type())
		: m_elements(elements), m_compare(compare)
		{
			init_node_map();

			if (do_makeheap)
			{
				make_heap(elements.size());
			}
		}

	public:
		// information retrieval

		size_type size() const
		{
			return m_btree.size();
		}

		bool empty() const
		{
			return m_btree.empty();
		}

		const_reference root() const
		{
			return m_elements[m_btree.root_value()];
		}

		const_reference get_by_node(node_type u) const
		{
			return m_elements[m_btree(u)];
		}

	public:
		// interaction with elements

		bool compare(const T& x, const T& y) const
		{
			return m_compare(x, y);
		}

		node_type node(size_type idx) const
		{
			return m_node_map[idx];
		}

		bool in_heap(size_type idx) const
		{
			return m_node_map[idx].non_nil();
		}

		void update_up(size_type idx) // pre-condition: in_heap(idx)
		{
			bubble_up(m_node_map[idx], m_elements[idx]);
		}

		void update_down(size_type idx) // pre-condition: in_heap(idx)
		{
			bubble_down(m_node_map[idx], m_elements[idx]);
		}

		void enroll(size_type idx) // pre-condition: !in_heap(idx)
		{
			// push to the last node of the tree
			m_btree.push(idx);

			// attach to node-map
			node_type last_node = m_btree.back();
			m_node_map[idx] = last_node;

			// adjust position
			if (m_btree.size() > 1)
			{
				bubble_up(last_node, m_elements[idx]);
			}
		}

		void pop_root()
		{
			size_type n = m_btree.size();

			if (n > 0)
			{
				// detach root-node from node-map
				m_node_map[m_btree.root_value()] = node_type();

				if (n > 1)
				{
					// put back to root
					size_type i = m_btree.root_value() = m_btree.back_value();
					m_node_map[i] = m_btree.root();

					// pop the back
					m_btree.pop();

					// adjust position
					bubble_down(m_btree.root(), m_elements[i]);
				}
				else
				{
					// simply pop the back
					m_btree.pop();
				}

			}
		}

	public:
		// make heap

		void make_heap(size_type n)
		{
			for (size_type i = 0; i < n; ++i)
			{
				m_btree.push(i);
				m_node_map[i] = m_btree.back();
			}

			do_make_heap();
		}

		template<typename InputIterator>
		void make_heap(InputIterator first, InputIterator last)
		{
			for (; first != last; ++first)
			{
				size_type i = (size_type)(*first);

				m_btree.push(i);
				m_node_map[i] = m_btree.back();
			}

			do_make_heap();
		}



	public:

		// for inspection & debug

		const tree_type& tree() const
		{
			return m_btree;
		}

		const node_map_type& node_map() const
		{
			return m_node_map;
		}

		const container_type& elements() const
		{
			return m_elements;
		}


	private:
		void init_node_map()
		{
			size_type n = m_elements.size();
			m_node_map.reserve(n);
			for (size_type i = 0; i < n; ++i)
			{
				m_node_map.push_back(node_type());
			}
		}


		void do_make_heap()
		{
			if (m_btree.size() > 1)
			{
				size_type last_nonleaf_id = m_btree.last_parent().id;
				for (size_type id = last_nonleaf_id; id > 0; --id)
				{
					bubble_down(id, get_by_node(id));
				}
			}
		}

		void bubble_up(node_type u, const value_type& e)
		{
			bool cont = true;

			while (cont && m_btree.is_non_root(u))
			{
				node_type p = m_btree.parent(u);
				if (compare(e, get_by_node(p)))
				{
					u = swap(u, p);
				}
				else
				{
					cont = false;
				}
			}
		}

		void bubble_down(node_type u, const value_type& e)
		{
			bool cont = true;

			node_type last_nl = m_btree.last_parent();
			while (cont && u.id <= last_nl.id)
			{
				cont = false;

				node_type lc, rc;
				m_btree.get_children(u, lc, rc);

				if (rc.non_nil())
				{
					const_reference le = get_by_node(lc);
					const_reference re = get_by_node(rc);

					if (compare(le, re))
					{
						if (compare(le, e))
						{
							u = swap(u, lc);
							cont = true;
						}
					}
					else
					{
						if (compare(re, e))
						{
							u = swap(u, rc);
							cont = true;
						}
					}
				}
				else if (lc.non_nil())
				{
					const_reference le = get_by_node(lc);
					if (compare(le, e))
					{
						u = swap(u, lc);
						cont = true;
					}
				}
			}
		}

		node_type swap(node_type u, node_type v)
		{
			size_type ui = m_btree(u);
			size_type vi = m_btree(v);

			m_node_map[ui] = v;
			m_node_map[vi] = u;

			m_btree(u) = vi;
			m_btree(v) = ui;

			return v;
		}

	private:
		const container_type& m_elements;
		tree_type m_btree;
		node_map_type m_node_map;

		Compare m_compare;

	}; // end class binary_heap


	template<typename T, class Container, class Compare>
	void update_element(Container& container, bcs::binary_heap<T, Container, Compare>& heap,
			const typename Container::size_type& idx, const T& v)
	{
		T v0 = container[idx];
		container[idx] = v;

		if (heap.compare(v, v0))
		{
			heap.update_up(idx);
		}
		else
		{
			heap.update_down(idx);
		}
	}


}

#endif /* STATIC_BINARY_TREE_H_ */
