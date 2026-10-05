/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_sift.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:10:53 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:10:55 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	heap_sift_up(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!heap_compare(&heap->nodes[index], &heap->nodes[parent]))
			break ;
		heap_swap(&heap->nodes[index], &heap->nodes[parent]);
		index = parent;
	}
}

void	heap_sift_down(t_heap *heap, int index)
{
	int	left;
	int	right;
	int	smallest;

	while (1)
	{
		left = index * 2 + 1;
		right = index * 2 + 2;
		smallest = index;
		if (left < heap->size
			&& heap_compare(&heap->nodes[left], &heap->nodes[smallest]))
			smallest = left;
		if (right < heap->size
			&& heap_compare(&heap->nodes[right], &heap->nodes[smallest]))
			smallest = right;
		if (smallest == index)
			break ;
		heap_swap(&heap->nodes[index], &heap->nodes[smallest]);
		index = smallest;
	}
}

int	heap_find_coder_index(t_heap *heap, int coder_id)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		if (heap->nodes[i].coder_id == coder_id)
			return (i);
		i++;
	}
	return (-1);
}

int	heap_remove_coder(t_heap *heap, int coder_id)
{
	int	index;

	index = heap_find_coder_index(heap, coder_id);
	if (index == -1)
		return (0);
	heap->size--;
	heap->nodes[index] = heap->nodes[heap->size];
	if (index < heap->size)
	{
		heap_sift_down(heap, index);
		heap_sift_up(heap, index);
	}
	return (1);
}
