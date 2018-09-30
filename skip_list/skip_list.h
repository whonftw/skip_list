#pragma once
#include <random>
#include <optional>
#include <vector>
#include <memory>
#include <string>

namespace wh_util
{
	template<typename T>
	struct node {
		std::optional<T> value;
		std::size_t level;

		node(std::size_t level, std::optional<T> value = {});
		std::vector<std::shared_ptr<node<T>>> next;
	};

	template<typename T>
	node<T>::node(std::size_t level, std::optional<T> value) 
		: value{ value }, level{ level }, next{ level + 1 }
	{
	}

	template<typename T>
	class skip_list {
	private:
		std::unique_ptr<node<T>> m_head;
		std::size_t m_size;
		float m_probability;

		std::size_t max_node_level() const;
		std::size_t compute_node_level() const;
	public:
		skip_list(float probability = 0.5);

		void display_elements() const;
		bool empty() const;
		std::size_t size() const;
		bool contains(const T&) const;

		void insert(T);
		void clear();
		void remove(const T&);
	};

	// ******** Constructor ********
	template<typename T>
	skip_list<T>::skip_list(float probability) : m_probability{ probability }, m_size{ 0 }
	{
		m_head = std::make_unique<node<T>>(0);
	}

	template<typename T>
	std::size_t skip_list<T>::compute_node_level() const {
		std::random_device rand;
		std::mt19937 rand_engine{ rand() };
		std::bernoulli_distribution coin_distribution{ m_probability };
		std::size_t level = 0;
		while (level <= max_node_level() && level <= m_head->level + 1 && coin_distribution(rand_engine))
			++level;
		return level;
	}

	template<typename T>
	inline std::size_t skip_list<T>::max_node_level() const {
		return (m_size >> 2) + 1;
	}

	template<typename T>
	void skip_list<T>::insert(T element)
	{
		int layer = m_head->level;
		node<T>* current = m_head.get();
		std::vector<node<T>*> to_fix(layer + 1);
		while (layer >= 0)
		{
			if (current->next[layer] == nullptr || current->next[layer]->value.value() > element)
				to_fix[layer--] = current;
			else
				current = current->next[layer].get();
			if (current->value.has_value() && current->value.value() == element)
				return;
		}

		std::shared_ptr<node<T>> new_node = std::make_shared<node<T>>(compute_node_level(), element);
		for (unsigned int i = 0; i <= new_node->level && i < to_fix.size(); ++i)
		{
			new_node->next[i] = to_fix[i]->next[i];
			to_fix[i]->next[i] = new_node;
		}

		if (new_node->level > m_head->level)
		{
			m_head->next.push_back(new_node);
			m_head->level += 1;
		}
		++m_size;
	}

	template<typename T>
	inline void skip_list<T>::clear()
	{
		m_head = std::make_unique<node<T>>(0);
		m_size = 0;
	}

	template<typename T>
	inline void skip_list<T>::remove(const T & element)
	{
		node<T>* current = m_head.get();
		int layer = current->level;
		std::vector<node<T>*> to_fix(layer + 1);
		while (layer >= 0)
		{
			if (current->next[layer] == nullptr || current->next[layer]->value.value() >= element)
				to_fix[layer--] = current;
			else
				current = current->next[layer].get();
		}
		current = current->next[0].get();
		if (current != nullptr && current->value.has_value() && current->value.value() == element)
		{
			for (unsigned int i = 0; i <= m_head->level; ++i)
			{
				if (to_fix[i]->next[i].get() != current)
					break;
				to_fix[i]->next[i] = current->next[i];
			}
			while (m_head->level > 0 && m_head->next[m_head->level] == nullptr)
			{
				m_head->next.pop_back();
				m_head->level -= 1;
			}
		}
	}

	template<typename T>
	inline void skip_list<T>::display_elements() const
	{
		if (empty())
			return;
		for (int i = m_head->level; i >= 0; --i)
		{
			auto current = m_head->next[i];
			while (current != nullptr)
			{
				std::cout << current->value.value() << " ";
				current = current->next[i];
			}
			std::cout << "\n";
		}
	}

	template<typename T>
	inline bool skip_list<T>::empty() const
	{
		return m_size == 0;
	}

	template<typename T>
	inline std::size_t skip_list<T>::size() const
	{
		return m_size;
	}

	template<typename T>
	bool skip_list<T>::contains(const T & element) const
	{
		node<T>* current = m_head.get();
		for (int i = m_head->level; i >= 0; --i)
		{
			while (current->next[i] && current->next[i]->value.value() < element)
				current = current->next[i].get();
		}
		current = current->next[0].get();
		if (current && current->value.value() == element)
			return true;
		return false;
	}
}

